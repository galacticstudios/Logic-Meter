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

#include <stdio.h>

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#include "gfx/hal/inc/gfx_draw_arc.h"
#include "gfx/hal/inc/gfx_math.h"

#define MAX_TICK_LABEL_DIGITS 10
#define MAX_TICK_LABEL_VALUE 9999999999

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_PIE_CHART,
    DRAW_BORDER,
};

static void drawBackground(laPieChartWidget* chart);
static void drawPieChart(laPieChartWidget* chart);
static void drawBorder(laPieChartWidget* chart);

static void nextState(laPieChartWidget* chart)
{
    switch(chart->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(chart->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                chart->widget.drawState = DRAW_BACKGROUND;
                chart->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            chart->widget.drawState = DRAW_PIE_CHART;
            chart->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawPieChart;
    
            return;
        }
        case DRAW_PIE_CHART:
        {            
            if(chart->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                chart->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                chart->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            chart->widget.drawState = DONE;
            chart->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laPieChartWidget* chart)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)chart);
    
    nextState(chart);
}

static void drawPieChart(laPieChartWidget* chart)
{
    GFX_Rect chartRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)chart);
    unsigned int i;
    int32_t startAngle, centerAngle;
    uint32_t totalValue = 0;
    
    GFX_Point p;
    p.x = chart->widget.rect.width/2;
    p.y = chart->widget.rect.height/2;
    
    startAngle = chart->startAngle;
    
    while (startAngle < 0)
    {
        startAngle += 360;
    }
    
    if (startAngle > 360)
    {
        startAngle %= 360;
    }
    
    
    laUtils_PointToLayerSpace((laWidget*)chart, &p);
    
    chartRect.x = 0;
    chartRect.y = 0;
    chartRect.width = chart->widget.rect.width;
    chartRect.height = chart->widget.rect.height;
    
    laUtils_RectToLayerSpace((laWidget*)chart, &chartRect);
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &chartRect) == GFX_TRUE)
    {
        GFX_RectClip(&chartRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, &clipRect);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, chart->widget.scheme->foreground);
        
        //Get the total
        for (i = 0; i < chart->pieArray.size; i++) 
        {
            laPieChartPie * pie = laArray_Get(&chart->pieArray, i);
            
            totalValue += pie->value;
        }

        //Draw the pies
        for (i = 0; i < chart->pieArray.size; i++) 
        {
            GFX_Point pieCenter = p;
            laPieChartPie * pie = laArray_Get(&chart->pieArray, i);

            if (pie->scheme != NULL)
            {
                GFX_Set(GFXF_DRAW_COLOR, pie->scheme->foreground);
                GFX_Set(GFXF_DRAW_GRADIENT_COLOR, pie->scheme->foreground & 0xffffff00, pie->scheme->foreground, NULL, NULL);
            }
            else
            {
                GFX_Set(GFXF_DRAW_COLOR, chart->widget.scheme->foreground);
                GFX_Set(GFXF_DRAW_GRADIENT_COLOR, chart->widget.scheme->foreground & 0xffffff00, chart->widget.scheme->foreground, NULL, NULL);
            }

            GFX_Set(GFXF_DRAW_THICKNESS, pie->radius);
            
            if (chart->centerAngle < 0)
            {
                centerAngle = - ((int32_t) (((float) (-chart->centerAngle) * (float) pie->value)/(float) totalValue + 0.5));
            }
            else
            {
                centerAngle = (int32_t) (((float) (chart->centerAngle) * (float) pie->value)/(float) totalValue + 0.5);
            }
            
                        
            if (pie->radius > 0)
            {                
                if (pie->offset != 0)
                {
                    GFX_Point offsetPoint;
                    int32_t midAngle;

                    //Find center angle of pie
                    midAngle = startAngle + centerAngle/2;

                    if (midAngle > 360)
                        midAngle %= 360;

                    //Find points of 
                    GFX_PolarToXY(pie->offset, midAngle , &offsetPoint);

                    pieCenter.x += offsetPoint.x;
                    pieCenter.y -= offsetPoint.y;
                }

                GFX_DrawArc(pieCenter.x, pieCenter.y, pie->radius, startAngle, centerAngle);

                if (chart->labelsVisible == LA_TRUE && chart->stringTable != NULL)
                {
                    GFX_Point centerPoint;
                    int32_t midAngle;
                    char strbuff[MAX_TICK_LABEL_DIGITS];
                    laString str;
                    GFX_Rect textRect, bounds, clipRect, drawRect;
                    uint32_t value = pie->value;

                    //Protect from overflow
                    if (value < MAX_TICK_LABEL_VALUE)
                    {
                        sprintf(strbuff, "%d", (int) value);
                    }
                    else
                    {
                        sprintf(strbuff, "---");
                    }

                    str = laString_CreateFromCharBuffer(strbuff, 
                    GFXU_StringFontIndexLookup(chart->stringTable, chart->labelsStringID, 0));

                    //Get the string rectangle
                    bounds = laUtils_WidgetLocalRect((laWidget*)chart);
                    laString_GetRect(&str, &textRect);

                    GFX_RectClip(&textRect, &bounds, &drawRect);

                    //Find center angle of pie
                    midAngle = startAngle + centerAngle/2;

                    if (midAngle > 360)
                        midAngle %= 360;

                    GFX_PolarToXY(pie->offset + chart->labelsOffset, midAngle , &centerPoint);

                    //Orient the text rectangle based on the label quadrant
                    if (midAngle < 10)
                    {
                        drawRect.x = p.x + centerPoint.x;
                        drawRect.y = p.y - centerPoint.y - (textRect.height/2);
                    }
                    else if (midAngle <= 80)
                    {
                        drawRect.x = p.x + centerPoint.x;
                        drawRect.y = p.y - centerPoint.y - textRect.height;
                    }
                    else if (midAngle <= 100)
                    {
                        drawRect.x = p.x + centerPoint.x - (textRect.width/2);
                        drawRect.y = p.y - centerPoint.y - textRect.height;
                    }
                    else if (midAngle <= 170)
                    {
                        drawRect.x = p.x + centerPoint.x - textRect.width;
                        drawRect.y = p.y - centerPoint.y - textRect.height;
                    }
                    else if (midAngle <= 190)
                    {
                        drawRect.x = p.x + centerPoint.x - textRect.width;
                        drawRect.y = p.y - centerPoint.y - (textRect.height/2);
                    }
                    else if (midAngle <= 260)
                    {
                        drawRect.x = p.x + centerPoint.x - textRect.width;
                        drawRect.y = p.y - centerPoint.y;
                    }
                    else if (midAngle <= 280)
                    {
                        drawRect.x = p.x + centerPoint.x - (textRect.width/2);
                        drawRect.y = p.y - centerPoint.y;
                    }
                    else if (midAngle <= 350)
                    {
                        drawRect.x = p.x + centerPoint.x;
                        drawRect.y = p.y - centerPoint.y;
                    }
                    else
                    {
                        drawRect.x = p.x + centerPoint.x;
                        drawRect.y = p.y - centerPoint.y - (textRect.height/2);
                    }

    //                drawRect.x = p.x + centerPoint.x;
    //                drawRect.y = p.y - centerPoint.y;

                    GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);

                    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
                    GFX_Set(GFXF_DRAW_COLOR, chart->widget.scheme->text);

                    laString_DrawClipped(&str,
                                         clipRect.x,
                                         clipRect.y,
                                         clipRect.width,
                                         clipRect.height,
                                         drawRect.x,
                                         drawRect.y,
                                         NULL);

                    laString_Destroy(&str);

                }
            }
            
            startAngle += centerAngle;
            while (startAngle < 0)
            {
                startAngle += 360;
            }

        }        
    }

    nextState(chart);
}

static void drawBorder(laPieChartWidget* chart)
{    
    if(chart->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)chart);
    else if(chart->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)chart);
    
    nextState(chart);
}

void _laPieChartWidget_Paint(laPieChartWidget* chart)
{
    laContext* context = laContext_GetActive();
    
    if(chart->widget.scheme == NULL)
    {
        chart->widget.drawState = DONE;
        
        return;
    }
    
    if(chart->widget.drawState == NOT_STARTED)
        nextState(chart);
    
    while(chart->widget.drawState != DONE)
    {
        chart->widget.drawFunc((laWidget*)chart);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_ARC_WIDGET_ENABLED
