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


#include "gfx/libaria/inc/libaria_widget_arc.h"
#include "gfx/libaria/inc/libaria_widget_circular_gauge.h"

#if LA_CIRCULAR_GAUGE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_array.h"
#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_layer.h"

#include "gfx/hal/inc/gfx_math.h"

#define DEFAULT_WIDTH           101
#define DEFAULT_HEIGHT          101

#define DEFAULT_ORIGIN_X        50
#define DEFAULT_ORIGIN_Y        50

#define DEFAULT_RADIUS          50
#define DEFAULT_START_ANGLE     225
#define DEFAULT_CENTER_ANGLE    -270
#define DEFAULT_START_VALUE     0
#define DEFAULT_END_VALUE       100
#define DEFAULT_VALUE           50
#define DEFAULT_TICK_INC        10
#define DEFAULT_TICK_LENGTH     5
#define DEFAULT_HAND_LENGTH     40

#define DEFAULT_CENTER_CIRCLE_RADIUS 5
#define DEFAULT_CENTER_CIRCLE_THICKNESS 2

//Invalidates the quadrant where needle is located
void invalidateHandQuadrant(laCircularGaugeWidget* gauge)
{
    GFX_Rect damagedRect;
    
    int32_t handAngleOffset = 0, handAbsAngle;
    int32_t unitOffset = 0;
    float degPerUnit;
    GFX_Point p = {0};
    
    laUtils_PointToLayerSpace((laWidget*)gauge, &p);

    unitOffset = gauge->value - gauge->startValue;
    degPerUnit = (float) gauge->centerAngle / (float) (abs(gauge->endValue - gauge->startValue));
    handAngleOffset = (int32_t) ((float) unitOffset * degPerUnit);

    handAbsAngle = gauge->startAngle + handAngleOffset;
    while(handAbsAngle < 0)
    {
        handAbsAngle += 360;
    }

    if (handAbsAngle >= 360)
    {
        handAbsAngle %= 360;
    }
    
    if (handAbsAngle > 270)
    {
        //Invalidate Q4
        damagedRect.width = laWidget_GetWidth((laWidget*) gauge)/2;
        damagedRect.height = laWidget_GetHeight((laWidget*) gauge)/2;
        
        damagedRect.x = p.x + damagedRect.width;
        damagedRect.y = p.y + damagedRect.height;
    }
    else if (handAbsAngle == 270)
    {
        //Invalidate Q3 & Q4
        damagedRect.width = laWidget_GetWidth((laWidget*) gauge);
        damagedRect.height = laWidget_GetHeight((laWidget*) gauge)/2;
        
        damagedRect.x = p.x;
        damagedRect.y = p.y + damagedRect.height;
    }
    else if (handAbsAngle > 180)
    {
        //Invalidate Q3
        damagedRect.width = laWidget_GetWidth((laWidget*) gauge)/2;
        damagedRect.height = laWidget_GetHeight((laWidget*) gauge)/2;
        
        damagedRect.x = p.x;
        damagedRect.y = p.y + damagedRect.height;
    }
    else if (handAbsAngle == 180)
    {
        //Invalidate Q2 & Q3
        damagedRect.width = laWidget_GetWidth((laWidget*) gauge)/2;
        damagedRect.height = laWidget_GetHeight((laWidget*) gauge);
        
        damagedRect.x = p.x;
        damagedRect.y = p.y;
    }
    else if (handAbsAngle > 90)
    {
        //Invalidate Q2
        damagedRect.width = laWidget_GetWidth((laWidget*) gauge)/2;
        damagedRect.height = laWidget_GetHeight((laWidget*) gauge)/2;
        
        damagedRect.x = p.x;
        damagedRect.y = p.y;
    }
    else if (handAbsAngle == 90)
    {
        //Invalidate Q1 & Q2
        damagedRect.width = laWidget_GetWidth((laWidget*) gauge);
        damagedRect.height = laWidget_GetHeight((laWidget*) gauge)/2;
        
        damagedRect.x = p.x;
        damagedRect.y = p.y;        
    }
    else if (handAbsAngle > 0)
    {
        //Invalidate Q1
        damagedRect.width = laWidget_GetWidth((laWidget*) gauge)/2;
        damagedRect.height = laWidget_GetHeight((laWidget*) gauge)/2;
        
        damagedRect.x = p.x + damagedRect.width;
        damagedRect.y = p.y;           
    }
    else if (handAbsAngle == 0)
    {
        //Invalidate Q4 & Q1
        damagedRect.width = laWidget_GetWidth((laWidget*) gauge)/2;
        damagedRect.height = laWidget_GetHeight((laWidget*) gauge);
        
        damagedRect.x = p.x + damagedRect.width;
        damagedRect.y = p.y;  
    }
        
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)gauge),
		                  &damagedRect,
					      LA_FALSE);
}

//Invalidates the needle rectangle
static void invalidateHandRect(laCircularGaugeWidget* gauge)
{
    GFX_Rect drawRect;
    
    GFX_Point outerPoint;
    int32_t handAngleOffset = 0, handAbsAngle;
    int32_t unitOffset = 0;
    float degPerUnit;
    GFX_Point p;
    
    p.x = laWidget_GetWidth((laWidget*) gauge)/2;
    p.y = laWidget_GetHeight((laWidget*) gauge)/2;
    
    laUtils_PointToLayerSpace((laWidget*)gauge, &p);

    unitOffset = gauge->value - gauge->startValue;
    degPerUnit = (float) gauge->centerAngle / (float) (abs(gauge->endValue - gauge->startValue));
    handAngleOffset = (int32_t) ((float) unitOffset * degPerUnit);

    handAbsAngle = gauge->startAngle + handAngleOffset;
    while(handAbsAngle < 0)
    {
        handAbsAngle += 360;
    }

    if (handAbsAngle >= 360)
    {
        handAbsAngle %= 360;
    }
    
    GFX_PolarToXY(gauge->handRadius, handAbsAngle, &outerPoint);

  
    if (p.x > (outerPoint.x + p.x))
    {
        drawRect.x = (outerPoint.x + p.x);
        drawRect.width = -outerPoint.x;
    }
    else 
    {
        drawRect.x = p.x;
        drawRect.width = outerPoint.x;
    }
    
    if (p.y > p.y - outerPoint.y)
    {
        drawRect.y = p.y - outerPoint.y;
        drawRect.height = outerPoint.y;
    }
    else 
    {
        drawRect.y = p.y;
        drawRect.height = -outerPoint.y;
    }
    
    drawRect.x -= 2;
    drawRect.y -= 2;
    drawRect.width += 4;
    drawRect.height += 4;
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)gauge),
		                  &drawRect,
					      LA_FALSE);
}

void _laCircularGaugeWidget_Constructor(laCircularGaugeWidget* gauge)
{
    _laWidget_Constructor((laWidget*)gauge);

    gauge->widget.type = LA_WIDGET_CIRCULAR_GAUGE;
    
    gauge->widget.destructor = (laWidget_Destructor_FnPtr)&_laCircularGaugeWidget_Destructor;

    // override base class methods
    gauge->widget.paint = (laWidget_Paint_FnPtr)&_laCircularGaugeWidget_Paint;

    gauge->widget.rect.width = DEFAULT_WIDTH;
    gauge->widget.rect.height = DEFAULT_HEIGHT;

    gauge->widget.borderType = LA_WIDGET_BORDER_NONE;
    gauge->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;

    //default gauge values
    gauge->value = DEFAULT_VALUE;
    gauge->radius = DEFAULT_RADIUS;
    gauge->startValue = DEFAULT_START_VALUE;
    gauge->endValue = DEFAULT_END_VALUE;
    gauge->startAngle = DEFAULT_START_ANGLE;
    gauge->centerAngle = DEFAULT_CENTER_ANGLE;

    //default tick properties
    gauge->ticksVisible = LA_TRUE;
    gauge->tickValue = DEFAULT_TICK_INC;
    gauge->tickLabelsVisible = LA_TRUE;
    gauge->tickLength = DEFAULT_TICK_LENGTH;

    //default hand properties
    gauge->handVisible = LA_TRUE;
    gauge->handRadius = DEFAULT_HAND_LENGTH;
    
    gauge->centerCircleVisible = LA_TRUE;
    gauge->centerCircleRadius = DEFAULT_CENTER_CIRCLE_RADIUS;
    gauge->centerCircleThickness = DEFAULT_CENTER_CIRCLE_THICKNESS;
    
    //Advanced configuration
    laArray_Create(&gauge->arcsArray);
    laArray_Create(&gauge->ticksArray);
    laArray_Create(&gauge->labelsArray);

}

void _laCircularGaugeWidget_Destructor(laCircularGaugeWidget* gauge)
{
    laArray_Destroy(&gauge->arcsArray);
    laArray_Destroy(&gauge->ticksArray);
    laArray_Destroy(&gauge->labelsArray);
    _laWidget_Destructor((laWidget*)gauge);
}

laCircularGaugeWidget* laCircularGaugeWidget_New()
{
    laCircularGaugeWidget* gauge = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    gauge = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laCircularGaugeWidget));

    if(gauge == NULL)
        return NULL;
    
    _laCircularGaugeWidget_Constructor(gauge);

    return gauge;
}

uint32_t laCircularGaugeWidget_GetRadius(laCircularGaugeWidget* gauge)
{
    if(gauge == NULL)
        return 0;
        
    return gauge->radius;
}

laResult laCircularGaugeWidget_SetRadius(laCircularGaugeWidget* gauge, uint32_t rad)
{
    if(gauge == NULL)
        return LA_FAILURE;
        
    if(gauge->radius == rad)
        return LA_SUCCESS;
        
    gauge->radius = rad;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

int32_t laCircularGaugeWidget_GetStartAngle(laCircularGaugeWidget* gauge)
{
    if(gauge == NULL)
        return 0;
        
    return gauge->startAngle;
    
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_SetStartAngle(laCircularGaugeWidget* gauge, int32_t angle)
{
    if(gauge == NULL)
        return LA_FAILURE;
        
    if(gauge->startAngle == angle)
        return LA_SUCCESS;
        
    gauge->startAngle = angle;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

int32_t laCircularGaugeWidget_GetCenterAngle(laCircularGaugeWidget* gauge)
{
    if(gauge == NULL)
        return 0;
        
    return gauge->centerAngle;
}

laResult laCircularGaugeWidget_SetCenterAngle(laCircularGaugeWidget* gauge, int32_t angle)
{
    if(gauge == NULL)
        return LA_FAILURE;

    if(gauge->centerAngle == angle)
        return LA_SUCCESS;
        
    if (angle == 0)
        return LA_FAILURE;
    
    gauge->centerAngle = angle;

    if (gauge->centerAngle < 0)
        gauge->dir = CIRCULAR_GAUGE_DIR_CLOCKWISE;
    else
        gauge->dir = CIRCULAR_GAUGE_DIR_COUNTER_CLOCKWISE;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laCircularGaugeWidgetDir laCircularGaugeWidget_GetDirection(laCircularGaugeWidget* gauge)
{
    if(gauge == NULL)
        return CIRCULAR_GAUGE_DIR_CLOCKWISE;
        
    return gauge->dir;
}

laResult laCircularGaugeWidget_SetDirection(laCircularGaugeWidget* gauge, 
                                            laCircularGaugeWidgetDir dir)
{
    if(gauge == NULL)
        return LA_FAILURE;

    if(gauge->dir == dir)
        return LA_SUCCESS;
        
    gauge->dir = dir;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_AddValueArc(laCircularGaugeWidget* gauge, 
                                      int32_t startValue, 
                                      int32_t endValue,
                                      uint32_t radius,
                                      uint32_t thickness,
                                      laScheme* scheme)
{
    laCircularGaugeArc * arc;
    if(gauge == NULL)
        return LA_FAILURE;

    if (thickness > radius)
        return LA_FAILURE;

    if (startValue > endValue)
        return LA_FAILURE;

    if (startValue < gauge->startValue || endValue > gauge->endValue)
        return LA_FAILURE;
    
    arc = laContext_GetActive()->memIntf.heap.malloc(sizeof(laCircularGaugeArc));
    if (arc == NULL)
        return LA_FAILURE;

    arc->type = VALUE_ARC;
    arc->radius = radius;
    arc->thickness = thickness;
    arc->scheme = scheme;
    arc->startValue = startValue;
    arc->endValue = endValue;
    arc->startAngle = 0;
    arc->endAngle = 0;

    laArray_PushBack(&gauge->arcsArray, arc);

    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_AddAngularArc(laCircularGaugeWidget* gauge, 
                                      int32_t startAngle, 
                                      int32_t endAngle,
                                      uint32_t radius,
                                      uint32_t thickness,
                                      laScheme* scheme)
{
    laCircularGaugeArc * arc;
    if(gauge == NULL)
        return LA_FAILURE;

    if (scheme == NULL)
        return LA_FAILURE;

    if (radius > gauge->radius)
        return LA_FAILURE;

    if (thickness > radius)
        return LA_FAILURE;
/*
    if (startAngle < 0)
        return LA_FAILURE;

    if (endAngle > 360)
        return LA_FAILURE;
*/
    arc = laContext_GetActive()->memIntf.heap.malloc(sizeof(laCircularGaugeArc));
    if (arc == NULL)
        return LA_FAILURE;

    arc->type = ANGLE_ARC;
    arc->radius = radius;
    arc->thickness = thickness;
    arc->scheme = scheme;
    arc->startAngle = startAngle;
    arc->endAngle = endAngle;
    arc->startValue = 0;
    arc->endValue = 0;

    laArray_PushBack(&gauge->arcsArray, arc);

    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_DeleteArcs(laCircularGaugeWidget* gauge)
{
    laArray_Destroy(&gauge->arcsArray);

    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_AddMinorTicks(laCircularGaugeWidget* gauge, 
                                             int32_t startValue, 
                                             int32_t endValue,
                                             uint32_t radius,
                                             uint32_t length,
                                             uint32_t interval,
                                             laScheme* scheme)
{
    laCircularGaugeTick * tick;
    if(gauge == NULL)
        return LA_FAILURE;

    if (radius > gauge->radius)
        return LA_FAILURE;

    if (length > radius)
        return LA_FAILURE;

    if (startValue < gauge->startValue || 
        startValue > gauge->endValue)
        return LA_FAILURE;
    
    if (endValue < gauge->startValue || 
        endValue > gauge->endValue)
        return LA_FAILURE;
    
    tick = laContext_GetActive()->memIntf.heap.malloc(sizeof(laCircularGaugeTick));
    if (tick == NULL)
        return LA_FAILURE;

    tick->radius = radius;
    tick->length = length;
    tick->startValue = startValue;
    tick->endValue = endValue;
    tick->interval = interval;
    tick->scheme = scheme;

    laArray_PushBack(&gauge->ticksArray, tick);

    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_DeleteMinorTicks(laCircularGaugeWidget* gauge)
{
    laArray_Destroy(&gauge->ticksArray);

    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_AddMinorTickLabels(laCircularGaugeWidget* gauge, 
                                                  int32_t startValue,
                                                  int32_t endValue,
                                                  uint32_t radius,
                                                  laCircularGaugeWidgetLabelPosition position,
                                                  uint32_t interval,
                                                  laScheme* scheme)
{
    laCircularGaugeLabel * label;
    if(gauge == NULL)
        return LA_FAILURE;

    if (radius > gauge->radius)
        return LA_FAILURE;

    if (position != CIRCULAR_GAUGE_LABEL_OUTSIDE &&
        position != CIRCULAR_GAUGE_LABEL_INSIDE)
        return LA_FAILURE;

    if (startValue < gauge->startValue || 
        startValue > gauge->endValue)
        return LA_FAILURE;
    
    if (endValue < gauge->startValue || 
        endValue > gauge->endValue)
        return LA_FAILURE;
    
    label = laContext_GetActive()->memIntf.heap.malloc(sizeof(laCircularGaugeLabel));
    if (label == NULL)
        return LA_FAILURE;

    label->radius = radius;
    label->position = position;
    label->startValue = startValue;
    label->endValue = endValue;
    label->interval = interval;
    label->scheme = scheme;

    laArray_PushBack(&gauge->labelsArray, label);

    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_DeleteMinorTickLabels(laCircularGaugeWidget* gauge)
{
    laArray_Destroy(&gauge->labelsArray);

    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

int32_t laCircularGaugeWidget_GetValue(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->value;
}

laResult laCircularGaugeWidget_SetValue(laCircularGaugeWidget* gauge, 
                                                   int32_t value)
{
    uint32_t bufferCount;
    
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->value == value)
        return LA_SUCCESS;
    
    if (value < gauge->startValue || value > gauge->endValue)
        return LA_FAILURE;
    
    bufferCount = laLayer_GetBufferCount(laUtils_GetLayer((laWidget*)gauge));
    
    //Optimized draw works better w/ single-buffered layers
    if (bufferCount == 1)
    {
        invalidateHandRect(gauge);
        gauge->value = value;
        invalidateHandRect(gauge);
    }
    else
    {
//        invalidateHandQuadrant(gauge);
        gauge->value = value;
//        invalidateHandQuadrant(gauge);
        laWidget_Invalidate((laWidget*)gauge);
    }
    
    return LA_SUCCESS;
}

int32_t laCircularGaugeWidget_GetStartValue(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->startValue;
}

laResult laCircularGaugeWidget_SetStartValue(laCircularGaugeWidget* gauge, 
                                                        int32_t value)
{
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->startValue == value)
        return LA_SUCCESS;

    gauge->startValue = value;

    laWidget_Invalidate((laWidget*)gauge);

    return LA_SUCCESS;
}

int32_t laCircularGaugeWidget_GetEndValue(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->endValue;
}

laResult laCircularGaugeWidget_SetEndValue(laCircularGaugeWidget* gauge, 
                                                        int32_t value)
{
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->endValue == value)
        return LA_SUCCESS;

    gauge->endValue = value;

    laWidget_Invalidate((laWidget*)gauge);

    return LA_SUCCESS;
}

laBool laCircularGaugeWidget_GetTicksVisible(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->ticksVisible;
}

laResult laCircularGaugeWidget_SetTicksVisible(laCircularGaugeWidget* gauge, 
                                                          laBool visible)
{
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->ticksVisible == visible)
        return LA_SUCCESS;

    gauge->ticksVisible = visible;

    laWidget_Invalidate((laWidget*)gauge);

    return LA_SUCCESS;
}

int32_t laCircularGaugeWidget_GetTickValue(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->tickValue;
}

laResult laCircularGaugeWidget_SetTickValue(laCircularGaugeWidget* gauge, 
                                            int32_t value)
{
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->tickValue == value)
        return LA_SUCCESS;
    
    gauge->tickValue = value;

    laWidget_Invalidate((laWidget*)gauge);

    return LA_SUCCESS;
}

uint32_t laCircularGaugeWidget_GetTickLength(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->tickLength;
}

laResult laCircularGaugeWidget_SetTickLength(laCircularGaugeWidget* gauge, 
                                            uint32_t length)
{
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->tickLength == length)
        return LA_SUCCESS;

    if (length > gauge->radius)
        return LA_FAILURE;
    
    gauge->tickLength = length;

    laWidget_Invalidate((laWidget*)gauge);

    return LA_SUCCESS;
}

laBool laCircularGaugeWidget_GetTickLabelsVisible(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->tickLabelsVisible;
}

laResult laCircularGaugeWidget_SetTickLabelsVisible(laCircularGaugeWidget* gauge, 
                                                    laBool visible)
{
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->tickLabelsVisible == visible)
        return LA_SUCCESS;

    gauge->tickLabelsVisible = visible;

    laWidget_Invalidate((laWidget*)gauge);

    return LA_SUCCESS;
}

laBool laCircularGaugeWidget_GetHandVisible(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->handVisible;
}

laResult laCircularGaugeWidget_SetHandVisible(laCircularGaugeWidget* gauge, 
                                              laBool visible)
{
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->handVisible == visible)
        return LA_SUCCESS;

    gauge->handVisible = visible;

    laWidget_Invalidate((laWidget*)gauge);

    return LA_SUCCESS;
}

uint32_t laCircularGaugeWidget_GetHandRadius(laCircularGaugeWidget* gauge)
{
    if(gauge == NULL)
        return 0;
        
    return gauge->handRadius;
}

laResult laCircularGaugeWidget_SetHandRadius(laCircularGaugeWidget* gauge, 
                                             uint32_t length)
{
    if(gauge == NULL)
        return LA_FAILURE;
        
    if(gauge->handRadius == length)
        return LA_SUCCESS;
    
    if (length > gauge->radius)
        return LA_FAILURE;
        
    gauge->handRadius = length;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laBool laCircularGaugeWidget_GetCenterCircleVisible(laCircularGaugeWidget* gauge)
{
    if (gauge == NULL)
        return 0;

    return gauge->centerCircleVisible;
}

laResult laCircularGaugeWidget_SetCenterCircleVisible(laCircularGaugeWidget* gauge, 
                                                      laBool visible)
{
    if (gauge == NULL)
        return LA_FAILURE;

    if (gauge->centerCircleVisible == visible)
        return LA_SUCCESS;

    gauge->centerCircleVisible = visible;

    laWidget_Invalidate((laWidget*)gauge);

    return LA_SUCCESS;
}

uint32_t laCircularGaugeWidget_GetCenterCircleRadius(laCircularGaugeWidget* gauge)
{
    if(gauge == NULL)
        return 0;
        
    return gauge->centerCircleRadius;
}

laResult laCircularGaugeWidget_SetCenterCircleRadius(laCircularGaugeWidget* gauge, 
                                                     uint32_t rad)
{
    if(gauge == NULL)
        return LA_FAILURE;
        
    if(gauge->centerCircleRadius == rad)
        return LA_SUCCESS;
    
    if (rad > gauge->radius)
        return LA_FAILURE;
    
    gauge->centerCircleRadius = rad;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

uint32_t laCircularGaugeWidget_GetCenterCircleThickness(laCircularGaugeWidget* gauge)
{
    if(gauge == NULL)
        return 0;
        
    return gauge->centerCircleThickness;
}

laResult laCircularGaugeWidget_SetCenterCircleThickness(laCircularGaugeWidget* gauge, uint32_t thickness)
{
    if(gauge == NULL)
        return LA_FAILURE;

    if (thickness > gauge->centerCircleRadius)
        return LA_FAILURE;

    if(gauge->centerCircleThickness == thickness)
        return LA_SUCCESS;
        
    gauge->centerCircleThickness = thickness;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_SetStringTable(laCircularGaugeWidget* gauge, 
                                              GFXU_StringTableAsset * stringTable)
{
    if(gauge == NULL)
        return LA_FAILURE;
        
    if(gauge->stringTable == stringTable)
        return LA_SUCCESS;
        
    gauge->stringTable = stringTable;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_SetTicksLabelsStringID(laCircularGaugeWidget* gauge, 
                                                      uint32_t stringID)
{
    if(gauge == NULL)
        return LA_FAILURE;
        
    if(gauge->ticksLabelsStringID == stringID)
        return LA_SUCCESS;
        
    gauge->ticksLabelsStringID = stringID;
    
    laWidget_Invalidate((laWidget*)gauge);
        
    return LA_SUCCESS;
}

laResult laCircularGaugeWidget_SetValueChangedEventCallback(laCircularGaugeWidget* gauge,
                                                             laCircularGaugeWidget_ValueChangedEvent cb)
{
    if(gauge == NULL || gauge->cb == cb)
        return LA_FAILURE;

    gauge->cb = cb;
    
    return LA_SUCCESS;
}


#endif // LA_CIRCULAR_GAUGE_WIDGET_ENABLED
