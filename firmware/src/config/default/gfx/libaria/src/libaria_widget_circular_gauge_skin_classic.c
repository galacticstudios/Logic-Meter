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


#include "gfx/libaria/inc/libaria_widget_circular_gauge.h"

#if LA_CIRCULAR_GAUGE_WIDGET_ENABLED

#include <stdlib.h>
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
#define LABEL_OFFSET_MIN_PIX 2

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_CIRCULAR_GAUGE,
    DRAW_BORDER,
};

static void drawBackground(laCircularGaugeWidget* gauge);
static void drawCircularGauge(laCircularGaugeWidget* gauge);
static void drawBorder(laCircularGaugeWidget* gauge);

static void nextState(laCircularGaugeWidget* gauge)
{
    switch(gauge->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(gauge->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                gauge->widget.drawState = DRAW_BACKGROUND;
                gauge->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            gauge->widget.drawState = DRAW_CIRCULAR_GAUGE;
            gauge->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawCircularGauge;
    
            return;
        }
        case DRAW_CIRCULAR_GAUGE:
        {            
            if(gauge->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                gauge->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                gauge->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            gauge->widget.drawState = DONE;
            gauge->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laCircularGaugeWidget* gauge)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)gauge);
    
    nextState(gauge);
}

void drawTickLabelsAtAngleWithValue(laCircularGaugeWidget * gauge, 
                                    int tickAbsAngle,
                                    int radius,
                                    laCircularGaugeWidgetLabelPosition position,
                                    int value)
{
    GFX_Point labelPoint;
    GFX_Point p;
    laLayer* layer = laUtils_GetLayer((laWidget*)gauge);

    p.x = laWidget_GetWidth((laWidget*) gauge)/2;
    p.y = laWidget_GetHeight((laWidget*) gauge)/2;
    
    laUtils_PointToLayerSpace((laWidget*)gauge, &p);

    while (tickAbsAngle < 0)
    {
        tickAbsAngle += 360;
    }
    
    if (tickAbsAngle >= 360)
    {
        tickAbsAngle %= 360;
    }
    
    GFX_PolarToXY(radius, tickAbsAngle , &labelPoint);
    
    //Draw the tick labels
    if (gauge->stringTable != NULL)
    {
        char strbuff[MAX_TICK_LABEL_DIGITS];
        laString str;
        GFX_Rect textRect, bounds, clipRect, drawRect;

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
            GFXU_StringFontIndexLookup(gauge->stringTable, gauge->ticksLabelsStringID, 0));
        
        //Get the string rectangle
        bounds = laUtils_WidgetLocalRect((laWidget*)gauge);
        laString_GetRect(&str, &textRect);

        GFX_RectClip(&textRect, &bounds, &drawRect);
        
        //Orient the text rectangle based on the tick quadrant
        if (position == CIRCULAR_GAUGE_LABEL_OUTSIDE)
        {
            if (tickAbsAngle < 10)
            {
                drawRect.x = p.x + labelPoint.x;
                drawRect.y = p.y - labelPoint.y - (textRect.height/2);
            }
            else if (tickAbsAngle <= 80)
            {
                drawRect.x = p.x + labelPoint.x;
                drawRect.y = p.y - labelPoint.y - textRect.height;
            }
            else if (tickAbsAngle <= 100)
            {
                drawRect.x = p.x + labelPoint.x - (textRect.width/2);
                drawRect.y = p.y - labelPoint.y - textRect.height;
            }
            else if (tickAbsAngle <= 170)
            {
                drawRect.x = p.x + labelPoint.x - textRect.width;
                drawRect.y = p.y - labelPoint.y - textRect.height;
            }
            else if (tickAbsAngle <= 190)
            {
                drawRect.x = p.x + labelPoint.x - textRect.width;
                drawRect.y = p.y - labelPoint.y - (textRect.height/2);
            }
            else if (tickAbsAngle <= 260)
            {
                drawRect.x = p.x + labelPoint.x - textRect.width;
                drawRect.y = p.y - labelPoint.y;
            }
            else if (tickAbsAngle <= 280)
            {
                drawRect.x = p.x + labelPoint.x - (textRect.width/2);
                drawRect.y = p.y - labelPoint.y;
            }
            else if (tickAbsAngle <= 350)
            {
                drawRect.x = p.x + labelPoint.x;
                drawRect.y = p.y - labelPoint.y;
            }
            else
            {
                drawRect.x = p.x + labelPoint.x;
                drawRect.y = p.y - labelPoint.y - (textRect.height/2);
            }
        }
        else
        {
            if (tickAbsAngle < 10)
            {
                drawRect.x = p.x + labelPoint.x - textRect.width;
                drawRect.y = p.y - labelPoint.y - textRect.height/2;
            }
            else if (tickAbsAngle <= 80)
            {
                drawRect.x = p.x + labelPoint.x - textRect.width;
                drawRect.y = p.y - labelPoint.y;
            }
            else if (tickAbsAngle <= 100)
            {
                drawRect.x = p.x + labelPoint.x - textRect.width/2;
                drawRect.y = p.y - labelPoint.y;
            }            
            else if (tickAbsAngle <= 170)
            {
                drawRect.x = p.x + labelPoint.x;
                drawRect.y = p.y - labelPoint.y;
            }
            else if (tickAbsAngle <= 190)
            {
                drawRect.x = p.x + labelPoint.x;
                drawRect.y = p.y - labelPoint.y - textRect.height/2;
            }
            else if (tickAbsAngle <= 260)
            {
                drawRect.x = p.x + labelPoint.x;
                drawRect.y = p.y - labelPoint.y - textRect.height;
            }
            else if (tickAbsAngle <= 280)
            {
                drawRect.x = p.x + labelPoint.x - textRect.height/2;
                drawRect.y = p.y - labelPoint.y - textRect.height;
            }
            else if (tickAbsAngle <= 350)
            {
                drawRect.x = p.x + labelPoint.x - textRect.width;
                drawRect.y = p.y - labelPoint.y - textRect.height;
            }             
            else
            {
                drawRect.x = p.x + labelPoint.x - textRect.width;
                drawRect.y = p.y - labelPoint.y - textRect.height/2;
            }            
        }
        
        //Clip to layer drawing rect
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);

        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->text);

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

static void drawTicksAtAngle(laCircularGaugeWidget* gauge, 
                             uint32_t radius,
                             uint32_t length,
                             int32_t tickAbsAngle)
{
    GFX_Point outerPoint, innerPoint;
    GFX_Point p;

    p.x = laWidget_GetWidth((laWidget*) gauge)/2;
    p.y = laWidget_GetHeight((laWidget*) gauge)/2;
    
    laUtils_PointToLayerSpace((laWidget*)gauge, &p);

    while (tickAbsAngle < 0)
    {
        tickAbsAngle += 360;
    }
    
    if (tickAbsAngle >= 360)
    {
        tickAbsAngle %= 360;
    }
    
    GFX_PolarToXY(radius, tickAbsAngle , &outerPoint);
    GFX_PolarToXY(radius - length, tickAbsAngle , &innerPoint);
    
    GFX_DrawLine(p.x + outerPoint.x, p.y - outerPoint.y, p.x + innerPoint.x, p.y - innerPoint.y);
}

static void drawCircularGauge(laCircularGaugeWidget* gauge)
{
    GFX_Point p;
    GFX_Rect gaugeRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)gauge);
    float degPerUnit;
    unsigned int i;
    
    p.x = laWidget_GetWidth((laWidget*) gauge)/2;
    p.y = laWidget_GetHeight((laWidget*) gauge)/2;
    
    laUtils_PointToLayerSpace((laWidget*)gauge, &p);
    
    gaugeRect.x = 0;
    gaugeRect.y = 0;
    gaugeRect.width = laWidget_GetWidth((laWidget*) gauge);
    gaugeRect.height = laWidget_GetHeight((laWidget*) gauge);

    degPerUnit = (float) gauge->centerAngle / (float) (abs(gauge->endValue - gauge->startValue));
    
    laUtils_RectToLayerSpace((laWidget*)gauge, &gaugeRect);
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &gaugeRect) == GFX_TRUE)
    {
        GFX_RectClip(&gaugeRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->foreground);

        //Ddraw the arcs
        for (i = 0; i < gauge->arcsArray.size; i++)
        {
            laCircularGaugeArc * arc = laArray_Get(&gauge->arcsArray, i);

            if (arc->scheme != NULL)
            {
                GFX_Set(GFXF_DRAW_COLOR, arc->scheme->foreground);
                GFX_Set(GFXF_DRAW_GRADIENT_COLOR, arc->scheme->foreground & 0xffffff00, arc->scheme->foreground, NULL, NULL);
            }
            else
            {
                GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->foreground);
                GFX_Set(GFXF_DRAW_GRADIENT_COLOR, gauge->widget.scheme->foreground & 0xffffff00, gauge->widget.scheme->foreground, NULL, NULL);
            }

            if (arc->type == VALUE_ARC)
            {
                arc->startAngle =  gauge->startAngle + (int32_t) (degPerUnit * (float) (arc->startValue - gauge->startValue));
                arc->endAngle = gauge->startAngle  + (int32_t) (degPerUnit * (float)(arc->endValue - gauge->startValue));
            }

            GFX_Set(GFXF_DRAW_THICKNESS, arc->thickness);
            GFX_DrawArc(p.x, p.y, arc->radius, arc->startAngle, arc->endAngle - arc->startAngle);
        }
        
        //Draw the minor ticks
        if (gauge->ticksArray.size > 0)
        {
            int32_t tickAngleOffset;
            int32_t unitOffset;
            laCircularGaugeTick * ticks;
            
            for (i = 0; i < gauge->ticksArray.size; i++)
            {
                ticks = laArray_Get(&gauge->ticksArray, i);
                unitOffset = 0;

                if (ticks->startValue >= gauge->startValue &&
                    ticks->endValue <= gauge->endValue &&
                    ticks->interval > 0 &&
                    ticks->radius > 0 &&
                    ticks->length > 0)
                {
                    if (ticks->scheme != NULL)
                    {
                        GFX_Set(GFXF_DRAW_COLOR, ticks->scheme->foreground);
                    }
                    else
                    {
                        GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->foregroundInactive);
                    }
                    
                    //draw start and middle ticks
                    while((ticks->startValue + unitOffset) < ticks->endValue)
                    {
                        tickAngleOffset = (int) ((float) (ticks->startValue - gauge->startValue + unitOffset) * degPerUnit);

                        drawTicksAtAngle(gauge, ticks->radius, ticks->length, gauge->startAngle + tickAngleOffset);

                        unitOffset += ticks->interval;
                    }

                    //draw tick at end
                    tickAngleOffset = (int) ((float) (ticks->endValue - gauge->startValue) * degPerUnit);
                    drawTicksAtAngle(gauge, ticks->radius, ticks->length, gauge->startAngle + tickAngleOffset);
                }
            }
        }
        
        //draw the minor tick labels
        if (gauge->labelsArray.size > 0)
        {
            int32_t tickAngleOffset;
            int32_t unitOffset;
            laCircularGaugeLabel * labels;
            
            for (i = 0; i < gauge->labelsArray.size; i++)
            {
                labels = laArray_Get(&gauge->labelsArray, i);
                unitOffset = 0;

                if (labels->startValue >= gauge->startValue &&
                    labels->endValue <= gauge->endValue &&
                    labels->interval > 0 &&
                    labels->radius > 0)
                {
                    if (labels->scheme != NULL)
                    {
                        GFX_Set(GFXF_DRAW_COLOR, labels->scheme->text);
                    }
                    else
                    {
                        GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->text);
                    }
                    
                    //draw start and middle ticks
                    while((labels->startValue + unitOffset) < labels->endValue)
                    {
                        tickAngleOffset = (int) ((float) (labels->startValue - gauge->startValue + unitOffset) * degPerUnit);

                        drawTickLabelsAtAngleWithValue(gauge, 
                                                       gauge->startAngle + tickAngleOffset,
                                                       labels->radius,
                                                       labels->position,
                                                       labels->startValue + unitOffset);

                        unitOffset += labels->interval;
                    }

                    //draw tick at end
                    tickAngleOffset = (int) ((float) (labels->endValue - gauge->startValue) * degPerUnit);
                    if (tickAngleOffset != 360)
                        drawTickLabelsAtAngleWithValue(gauge, 
                                                       gauge->startAngle + tickAngleOffset,
                                                       labels->radius,
                                                       labels->position,
                                                       labels->endValue);
                }
            }
        }        

        //Draw the major ticks.
        if (gauge->ticksVisible == LA_TRUE)
        {
            int32_t tickAngleOffset = 0;
            int32_t unitOffset = gauge->tickValue;

            //Draw the starting tick
            GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->foregroundInactive);
            drawTicksAtAngle(gauge, gauge->radius, gauge->tickLength, gauge->startAngle);
            
            while((gauge->startValue + unitOffset) < gauge->endValue)
            {
                tickAngleOffset = (int32_t) ((float) unitOffset * degPerUnit);

                drawTicksAtAngle(gauge, gauge->radius, gauge->tickLength, gauge->startAngle + tickAngleOffset);

                unitOffset += gauge->tickValue;
            } 

            //Draw end tick, if not a full circle
            if (gauge->centerAngle != 360 && gauge->centerAngle != -360)
            {
                drawTicksAtAngle(gauge, gauge->radius, gauge->tickLength, gauge->startAngle + gauge->centerAngle);
            }
        }
        
        //Draw the major tick labels
        if (gauge->tickLabelsVisible == LA_TRUE)
        {
            int32_t tickAngleOffset = 0;
            int32_t unitOffset = gauge->tickValue;

            //Draw the starting tick
            GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->text);
            drawTickLabelsAtAngleWithValue(gauge, 
                                           gauge->startAngle,
                                           gauge->radius + LABEL_OFFSET_MIN_PIX,
                                           CIRCULAR_GAUGE_LABEL_OUTSIDE,
                                           gauge->startValue);
            
            while((gauge->startValue + unitOffset) < gauge->endValue)
            {
                tickAngleOffset = (int32_t) ((float) unitOffset * degPerUnit);

                drawTickLabelsAtAngleWithValue(gauge, 
                                               gauge->startAngle + tickAngleOffset,
                                               gauge->radius + LABEL_OFFSET_MIN_PIX,
                                               CIRCULAR_GAUGE_LABEL_OUTSIDE,
                                               gauge->startValue + unitOffset);

                unitOffset += gauge->tickValue;
            } 

            //Draw end tick, if not a full circle
            if (gauge->centerAngle != 360 && gauge->centerAngle != -360)
            {
                drawTickLabelsAtAngleWithValue(gauge, 
                                               gauge->startAngle + gauge->centerAngle,
                                               gauge->radius + LABEL_OFFSET_MIN_PIX,
                                               CIRCULAR_GAUGE_LABEL_OUTSIDE,
                                               gauge->endValue);
            }
        }

        //Draw the hand
        if (gauge->handVisible == LA_TRUE)
        {
            GFX_Point outerPoint, innerPoint;
            int32_t handAngleOffset = 0, handAbsAngle;
            int32_t unitOffset = 0;

            unitOffset = gauge->value - gauge->startValue;
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

            innerPoint.x = 0;
            innerPoint.y = 0;
            
            GFX_PolarToXY(gauge->handRadius, handAbsAngle, &outerPoint);
            
            //Draw the center circle
            if (gauge->centerCircleVisible == LA_TRUE)
            {
                GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->foreground);
                GFX_Set(GFXF_DRAW_GRADIENT_COLOR, gauge->widget.scheme->foreground & 0xffffff00, gauge->widget.scheme->foreground, NULL, NULL);
                GFX_Set(GFXF_DRAW_THICKNESS, gauge->centerCircleThickness);
                GFX_DrawArc(p.x, p.y, gauge->centerCircleRadius, 0, 360);
                GFX_PolarToXY(gauge->centerCircleRadius, handAbsAngle, &innerPoint);
            }
            
            GFX_Set(GFXF_DRAW_COLOR, gauge->widget.scheme->foreground);
            GFX_DrawLine(p.x + outerPoint.x, p.y - outerPoint.y, p.x + innerPoint.x, p.y - innerPoint.y);
        }
    }

    nextState(gauge);
}

static void drawBorder(laCircularGaugeWidget* gauge)
{    
    if(gauge->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)gauge);
    else if(gauge->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)gauge);
    
    nextState(gauge);
}

void _laCircularGaugeWidget_Paint(laCircularGaugeWidget* gauge)
{
    laContext* context = laContext_GetActive();
    
    if(gauge->widget.scheme == NULL)
    {
        gauge->widget.drawState = DONE;
        
        return;
    }
    
    if(gauge->widget.drawState == NOT_STARTED)
        nextState(gauge);
    
    while(gauge->widget.drawState != DONE)
    {
        gauge->widget.drawFunc((laWidget*)gauge);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_CIRCULAR_GAUGE_WIDGET_ENABLED
