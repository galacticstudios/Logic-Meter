// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END


#include "gfx/libaria/inc/libaria_widget_pie_chart.h"

#if LA_PIE_CHART_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/hal/inc/gfx_math.h"

#define DEFAULT_WIDTH           101
#define DEFAULT_HEIGHT          101

#define DEFAULT_RADIUS          50
#define DEFAULT_LABEL_OFFSET    40
#define DEFAULT_START_ANGLE     0
#define DEFAULT_CENTER_ANGLE    180
#define DEFAULT_VALUE           10

static void touchDown(laPieChartWidget* chart, laInput_TouchDownEvent* evt);
static void touchUp(laPieChartWidget* chart, laInput_TouchUpEvent* evt);
static void touchMoved(laPieChartWidget* chart, laInput_TouchMovedEvent* evt);

void _laPieChartWidget_Constructor(laPieChartWidget* chart)
{
    _laWidget_Constructor((laWidget*)chart);

    chart->widget.type = LA_WIDGET_ARC;
    
    chart->widget.destructor = (laWidget_Destructor_FnPtr)&_laPieChartWidget_Destructor;

    // override base class methods
    chart->widget.paint = (laWidget_Paint_FnPtr)&_laPieChartWidget_Paint;

    chart->widget.rect.width = DEFAULT_WIDTH;
    chart->widget.rect.height = DEFAULT_HEIGHT;

    chart->widget.borderType = LA_WIDGET_BORDER_NONE;
    chart->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;

    chart->startAngle = DEFAULT_START_ANGLE;
    chart->centerAngle = DEFAULT_CENTER_ANGLE;
    
    chart->labelsVisible = LA_TRUE;
    chart->labelsOffset = DEFAULT_LABEL_OFFSET;
    
    laArray_Create(&chart->pieArray);

    chart->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&touchDown;
    chart->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&touchUp;
    chart->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&touchMoved;
}

void _laPieChartWidget_Destructor(laPieChartWidget* chart)
{
    laArray_Destroy(&chart->pieArray);
    _laWidget_Destructor((laWidget*)chart);
}

laPieChartWidget* laPieChartWidget_New()
{
    laPieChartWidget* chart = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    chart = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laPieChartWidget));

    if(chart == NULL)
        return NULL;
    
    _laPieChartWidget_Constructor(chart);

    return chart;
}

int32_t laPieChartWidget_GetStartAngle(laPieChartWidget* chart)
{
    if(chart == NULL)
        return 0;
        
    return chart->startAngle;
    
    return LA_SUCCESS;
}

laResult laPieChartWidget_SetStartAngle(laPieChartWidget* chart, int32_t angle)
{
    if(chart == NULL)
        return LA_FAILURE;
        
    if(chart->startAngle == angle)
        return LA_SUCCESS;
        
    chart->startAngle = angle;
    
    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

int32_t laPieChartWidget_GetCenterAngle(laPieChartWidget* chart)
{
    if(chart == NULL)
        return 0;
        
    return chart->centerAngle;
}

laResult laPieChartWidget_SetCenterAngle(laPieChartWidget* chart, int32_t angle)
{
    if(chart == NULL)
        return LA_FAILURE;

    if(chart->centerAngle == angle)
        return LA_SUCCESS;
        
    chart->centerAngle = angle;
    
    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

laResult laPieChartWidget_AddEntry(laPieChartWidget* chart, 
                                 int32_t * index)
{
    laPieChartPie * pie;
    laResult retval;
    
    if (chart == NULL)
        return LA_FAILURE;
    
    pie = laContext_GetActive()->memIntf.heap.malloc(sizeof(laPieChartPie));
    if (pie == NULL)
        return LA_FAILURE;
    
    pie->offset = 0;
    pie->radius = DEFAULT_RADIUS;
    pie->scheme = laWidget_GetScheme((laWidget*) pie);
    pie->value = DEFAULT_VALUE;
    
    retval = laArray_PushBack(&chart->pieArray, pie);
    if (retval == LA_FAILURE)
        return LA_FAILURE;

    if (index != NULL)
        *index = chart->pieArray.size - 1;

    laWidget_Invalidate((laWidget*)chart);
    
    return LA_SUCCESS;
}

laResult laPieChartWidget_DeletePies(laPieChartWidget* chart)
{
    laArray_Destroy(&chart->pieArray);

    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

uint32_t laPieChartWidget_GetEntryValue(laPieChartWidget* chart, uint32_t index)
{
    laPieChartPie * pie;
    
    if(chart == NULL)
        return 0;
    
    pie = (laPieChartPie *) laArray_Get(&chart->pieArray, index);
    if (pie == NULL)
        return 0;
    
    return pie->value;
}

laResult laPieChartWidget_SetEntryValue(laPieChartWidget* chart, uint32_t index, uint32_t value)
{
    laPieChartPie * pie;
    
    if(chart == NULL)
        return LA_FAILURE;
        
    pie = (laPieChartPie *) laArray_Get(&chart->pieArray, index);
    if (pie == NULL)
        return LA_FAILURE;
    
    if (pie->value == value)
        return LA_SUCCESS;
    
    pie->value = value;
    
    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

uint32_t laPieChartWidget_GetEntryRadius(laPieChartWidget* chart, uint32_t index)
{
    laPieChartPie * pie;
    
    if(chart == NULL)
        return 0;
    
    pie = (laPieChartPie *) laArray_Get(&chart->pieArray, index);
    if (pie == NULL)
        return 0;
    
    return pie->radius;
}

laResult laPieChartWidget_SetEntryRadius(laPieChartWidget* chart, uint32_t index, uint32_t rad)
{
    laPieChartPie * pie;
    
    if(chart == NULL)
        return LA_FAILURE;
        
    pie = (laPieChartPie *) laArray_Get(&chart->pieArray, index);
    if (pie == NULL)
        return LA_FAILURE;
    
    if (pie->radius == rad)
        return LA_SUCCESS;
    
    pie->radius = rad;
    
    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

uint32_t laPieChartWidget_GetEntryOffset(laPieChartWidget* chart, uint32_t index)
{
    laPieChartPie * pie;
    
    if(chart == NULL)
        return 0;
    
    pie = (laPieChartPie *) laArray_Get(&chart->pieArray, index);
    if (pie == NULL)
        return 0;
    
    return pie->offset;
}

laResult laPieChartWidget_SetEntryOffset(laPieChartWidget* chart, uint32_t index, uint32_t offset)
{
    laPieChartPie * pie;
    
    if(chart == NULL)
        return LA_FAILURE;
        
    pie = (laPieChartPie *) laArray_Get(&chart->pieArray, index);
    if (pie == NULL)
        return LA_FAILURE;
    
    if (pie->offset == offset)
        return LA_SUCCESS;
    
    pie->offset = offset;
    
    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

laResult laPieChartWidget_SetEntryScheme(laPieChartWidget* chart, uint32_t index, laScheme * scheme)
{
    laPieChartPie * pie;
    
    if(chart == NULL)
        return LA_FAILURE;
        
    pie = (laPieChartPie *) laArray_Get(&chart->pieArray, index);
    if (pie == NULL)
        return LA_FAILURE;
    
    if (pie->scheme == scheme)
        return LA_SUCCESS;
    
    pie->scheme = scheme;
    
    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

laScheme * laPieChartWidget_GetEntryScheme(laPieChartWidget* chart, uint32_t index)
{
    laPieChartPie * pie;
    
    if(chart == NULL)
        return 0;
    
    pie = (laPieChartPie *) laArray_Get(&chart->pieArray, index);
    if (pie == NULL)
        return 0;
    
    return pie->scheme;
}

laResult laPieChartWidget_SetPressedEventCallback(laPieChartWidget* chart,
                                                  laPieChartWidget_PressedEvent cb)
{
    if (cb == NULL)
        return LA_FAILURE;

    chart->pressedCallback = cb;
        
    return LA_SUCCESS;
}

laResult laPieChartWidget_SetStringTable(laPieChartWidget * chart,
                                         GFXU_StringTableAsset * stringTable)
{
    if(chart == NULL)
        return LA_FAILURE;
        
    if(chart->stringTable == stringTable)
        return LA_SUCCESS;
        
    chart->stringTable = stringTable;
    
    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

laResult laPieChartWidget_SetLabelsStringID(laPieChartWidget* chart, 
                                                 uint32_t stringID)
{
    if(chart == NULL)
        return LA_FAILURE;
        
    if(chart->labelsStringID == stringID)
        return LA_SUCCESS;
        
    chart->labelsStringID = stringID;
    
    laWidget_Invalidate((laWidget*)chart);
        
    return LA_SUCCESS;
}

laBool laPieChartWidget_GetLabelsVisible(laPieChartWidget* chart)
{
    if (chart == NULL)
        return 0;

    return chart->labelsVisible;
}

laResult laPieChartWidget_SetLabelsVisible(laPieChartWidget* chart, 
                                           laBool visible)
{
    if (chart == NULL)
        return LA_FAILURE;

    if (chart->labelsVisible == visible)
        return LA_SUCCESS;

    chart->labelsVisible = visible;

    laWidget_Invalidate((laWidget*) chart);

    return LA_SUCCESS;
}

uint32_t laPieChartWidget_GetLabelsOffset(laPieChartWidget* chart)
{
    if (chart == NULL)
        return 0;

    return chart->labelsOffset;
}

laResult laPieChartWidget_SetLabelsOffset(laPieChartWidget* chart, 
                                           uint32_t offset)
{
    if (chart == NULL)
        return LA_FAILURE;

    if (chart->labelsOffset == offset)
        return LA_SUCCESS;

    chart->labelsOffset = offset;

    laWidget_Invalidate((laWidget*) chart);

    return LA_SUCCESS;
}

static int32_t getPieIndexAtPoint(laPieChartWidget* chart, GFX_Point pnt)
{
    int32_t tempAngle, centerAngle;
    uint32_t pointAngle;
    uint32_t startAngle, endAngle;
    uint32_t i;
    uint32_t totalValue = 0;

    tempAngle = chart->startAngle;
    
    while (tempAngle < 0)
    {
        tempAngle += 360;
    }

    startAngle = tempAngle;
    
    if (startAngle > 360)
    {
        startAngle %= 360;
    }

    if (pnt.x > 0 && pnt.y > 0)
    {
        //Q1
        pointAngle = (int32_t) ((double) GFX_Atan((double) pnt.y / pnt.x) * (double)(180 / 3.1416));
    }
    else if (pnt.x < 0 && pnt.y > 0)
    {
        //Q2
        pointAngle = (int32_t) ((double) GFX_Atan((double) pnt.y / pnt.x) * (double)(180 / 3.1416));
        pointAngle = 180 + pointAngle;
    }
    else if (pnt.x > 0 && pnt.y < 0)
    {
        //Q4
        pointAngle = (int32_t) ((double) GFX_Atan((double) pnt.y / pnt.x) * (double)(180 / 3.1416));
        pointAngle = 360 + pointAngle;
    }
    else if (pnt.x < 0 && pnt.y < 0)
    {
        //Q3
        pointAngle = (int32_t) ((double) GFX_Atan((double) pnt.y / pnt.x) * (double)(180 / 3.1416));
        pointAngle = 180 + pointAngle;
    }
    else if (pnt.x == 0 && pnt.y >= 0)
    {
        // +y
        pointAngle = 90;
    }
    else if (pnt.x == 0 && pnt.y < 0)
    {
        // -y
        pointAngle = 270;
    }
    else if (pnt.y == 0 && pnt.x > 0)
    {
        // +x
        pointAngle = 0;
    }
    else
    {
        // -x
        pointAngle = 180;
    }

    //Get the total
    for (i = 0; i < chart->pieArray.size; i++) 
    {
        laPieChartPie * pie = laArray_Get(&chart->pieArray, i);
        
        totalValue += pie->value;
    }

    //Test the angle if it's in a pie
    for (i = 0; i < chart->pieArray.size; i++) 
    {
        laPieChartPie * pie = laArray_Get(&chart->pieArray, i);

        if (chart->centerAngle < 0)
        {
            centerAngle = - ((int32_t) (((float) (-chart->centerAngle) * (float) pie->value)/(float) totalValue + 0.5));
            tempAngle = startAngle + centerAngle;

            while (tempAngle < 0)
            {
                tempAngle += 360;
            }

            endAngle = tempAngle;

            if (endAngle > 360)
                endAngle %= 360;

            if (startAngle >= endAngle)
            {
                if (pointAngle <= startAngle && pointAngle >= endAngle)
                {
                    if (((uint32_t) ((pnt.y * pnt.y) + (pnt.x * pnt.x)) <= 
                        (pie->radius + pie->offset) * (pie->radius + pie->offset)))
                    {
                        return i;
                    }
                }
            }
            //Pie overlaps the 0-deg axis
            else
            {
                if (pointAngle <= startAngle || pointAngle >= endAngle)
                {
                    if (((uint32_t) ((pnt.y * pnt.y) + (pnt.x * pnt.x)) <= 
                        (pie->radius + pie->offset) * (pie->radius + pie->offset)))
                    {
                        return i;
                    }
                }
            }
        }
        else
        {
            centerAngle = (int32_t) (((float) (chart->centerAngle) * (float) pie->value)/(float) totalValue + 0.5);
            endAngle = startAngle + centerAngle;

            if (endAngle > 360)
                endAngle %= 360;

            if (startAngle <= endAngle)
            {
                if (pointAngle <= endAngle && 
                    pointAngle >= startAngle)
                {
                    if (((uint32_t) ((pnt.y * pnt.y) + (pnt.x * pnt.x)) <= 
                        (pie->radius + pie->offset) * (pie->radius + pie->offset)))
                    {
                        return i;
                    }
                }
            }
            //Pie overlaps the 0-deg axis
            else
            {
                if (pointAngle <= endAngle || pointAngle >= startAngle)
                {
                    if (((uint32_t) ((pnt.y * pnt.y) + (pnt.x * pnt.x)) <= 
                        (pie->radius + pie->offset) * (pie->radius + pie->offset)))
                    {
                        return i;
                    }
                }
            }
        }

        startAngle = endAngle;
    } 

    return -1;
    
}

static void touchDown(laPieChartWidget* chart, laInput_TouchDownEvent* evt)
{
    GFX_Point pnt;
    int32_t index;

    pnt.x = evt->x;
    pnt.y = evt->y;

    //Adjust point relative to widget local
    laUtils_PointScreenToLocalSpace((laWidget*)chart, &pnt);

    //Adjust point relative widget center/origin
    pnt.x -= chart->widget.rect.width/2 ; 
    pnt.y = chart->widget.rect.height/2 - pnt.y;

    index = getPieIndexAtPoint(chart, pnt);
    if (index != -1)
    {
        evt->event.accepted = LA_TRUE;
        
        if (chart->pressedCallback != NULL)  
            chart->pressedCallback(chart, index);
    }
}

static void touchUp(laPieChartWidget* chart, laInput_TouchUpEvent* evt)
{
}

static void touchMoved(laPieChartWidget* chart, laInput_TouchMovedEvent* evt)
{
}

#endif // LA_ARC_WIDGET_ENABLED