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


#include "gfx/libaria/inc/libaria_widget_bar_graph.h"

#if LA_BAR_GRAPH_WIDGET_ENABLED

#include <stdio.h>

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_BAR_GRAPH,
    DRAW_STRING,
    WAIT_STRING,    
    DRAW_BORDER,
};

static void drawBackground(laBarGraphWidget* graph);
static void drawBarGraph(laBarGraphWidget* graph);
static void drawString(laBarGraphWidget* graph);
static void drawBorder(laBarGraphWidget* graph);
static void waitString(laBarGraphWidget* btn);

static void nextState(laBarGraphWidget* graph)
{
    switch(graph->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(graph->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                graph->widget.drawState = DRAW_BACKGROUND;
                graph->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            graph->widget.drawState = DRAW_BAR_GRAPH;
            graph->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBarGraph;
    
            return;
        }
        case DRAW_BAR_GRAPH:
        {            
            graph->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;
            graph->widget.drawState = DRAW_STRING;
            
            return;
        }
        case DRAW_STRING:
        {
            if(graph->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                graph->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                graph->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            graph->widget.drawState = DONE;
            graph->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laBarGraphWidget* graph)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)graph);
    
    nextState(graph);
}

static void drawTickLabelWithValue(laBarGraphWidget* graph, GFX_Point tickPoint, laRelativePosition position, int32_t value)
{
    #define TICK_LABEL_MARGIN 3
    char strbuff[MAX_TICK_LABEL_DIGITS];
    laString str;
    GFX_Rect textRect, widgetRect, clipRect, drawRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)graph);

    if (graph->stringTable == NULL)
        return;

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
        GFXU_StringFontIndexLookup(graph->stringTable, graph->ticksLabelsStringID, 0));

    //Get the string rectangle
    widgetRect = laUtils_WidgetLocalRect((laWidget*)graph);
    laString_GetRect(&str, &textRect);
    
    GFX_RectClip(&textRect, &widgetRect, &drawRect);
    
    if (position == LA_RELATIVE_POSITION_LEFTOF)
    {
        drawRect.x = tickPoint.x - textRect.width - LABEL_OFFSET_MIN_PIX;
        drawRect.y = tickPoint.y - textRect.height/2;
    }
    else if (position == LA_RELATIVE_POSITION_RIGHTOF)
    {
        drawRect.x = tickPoint.x + textRect.width + LABEL_OFFSET_MIN_PIX;
        drawRect.y = tickPoint.y - textRect.height/2;
    }
    else
    {
        drawRect.x = tickPoint.x;
        drawRect.y = tickPoint.y;
    }
    
    GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->text);
    
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

static void drawBarGraph(laBarGraphWidget* graph)
{
    GFX_Rect widgetRect, clipRect, graphRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)graph);
    float pixelsPerUnit;
    
    widgetRect = laUtils_WidgetLocalRect((laWidget*)graph);
    laUtils_RectToLayerSpace((laWidget*)graph, &widgetRect);
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &widgetRect) == GFX_TRUE)
    {
        GFX_Point originPoint = {0};
        int32_t originValue = 0;
        
        GFX_RectClip(&widgetRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
        
        _laBarGraphWidget_GetGraphRect(graph, &graphRect);
        
        pixelsPerUnit = (float) graphRect.height / ((float) graph->maxValue - (float) graph->minValue);
        
        //Fill the base
        if (graph->fillGraphArea)
        {
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->base);
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
            GFX_DrawRect(graphRect.x, graphRect.y, graphRect.width, graphRect.height);
        }

        //Draw the graph borders
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
        GFX_DrawLine(graphRect.x, graphRect.y, graphRect.x, graphRect.y + graphRect.height); //Left
        GFX_DrawLine(graphRect.x + graphRect.width, graphRect.y, graphRect.x + graphRect.width, graphRect.y + graphRect.height); //Right
        
        //Adjust the horizontal borders if ticks are shown
        if (graph->valueAxisTicksVisible)
        {
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
            switch (graph->valueAxisTicksPosition)
            {
                case BAR_GRAPH_TICK_OUT:
                {
                    GFX_DrawLine(graphRect.x - graph->tickLength, 
                                 graphRect.y, 
                                 graphRect.x + graphRect.width , 
                                 graphRect.y); //Top            
                    GFX_DrawLine(graphRect.x - graph->tickLength, 
                                 graphRect.y + graphRect.height, 
                                 graphRect.x + graphRect.width, 
                                 graphRect.y + graphRect.height); //Bottom
                    break;
                }
                case BAR_GRAPH_TICK_CENTER:
                {
                    GFX_DrawLine(graphRect.x - graph->tickLength / 2, 
                                 graphRect.y, 
                                 graphRect.x + graphRect.width , 
                                 graphRect.y); //Top            
                    GFX_DrawLine(graphRect.x - graph->tickLength / 2, 
                                 graphRect.y + graphRect.height, 
                                 graphRect.x + graphRect.width, 
                                 graphRect.y + graphRect.height); //Bottom
                    break;
                }
                default:
                    break;
            }
        }

        //Draw the labels for min/max values
        if (graph->valueAxisLabelsVisible)
        {
            GFX_Point drawPoint = {0};
            
            drawPoint.x = graphRect.x;
            
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->text);

            if (graph->valueAxisTicksVisible)
            {
                switch (graph->valueAxisTicksPosition)
                {
                    case BAR_GRAPH_TICK_OUT:
                    {
                        drawPoint.x -= graph->tickLength;
                        break;
                    }
                    case BAR_GRAPH_TICK_CENTER:
                    {
                        drawPoint.x -= graph->tickLength / 2;
                        break;
                    }
                    default:
                        break;
                }
            }
            
            drawPoint.y = graphRect.y;
            drawTickLabelWithValue(graph, drawPoint, LA_RELATIVE_POSITION_LEFTOF, graph->maxValue);

            drawPoint.y = graphRect.y + graphRect.height;
            drawTickLabelWithValue(graph, drawPoint, LA_RELATIVE_POSITION_LEFTOF, graph->minValue);
        }
        
        //Determine the origin point
        originPoint.x = graphRect.x;
        if (graph->minValue < 0 && graph->maxValue > 0)
        {
            GFX_Point drawPoint = {0};
            originPoint.y = graphRect.y + (int32_t) ((float) graph->maxValue * pixelsPerUnit);
            originValue = 0;

            drawPoint = originPoint;

            //Draw the origin line
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
            if (graph->valueAxisTicksVisible)
            {
                switch (graph->valueAxisTicksPosition)
                {
                    case BAR_GRAPH_TICK_OUT:
                    {
                        drawPoint.x -= graph->tickLength;
                        break;
                    }
                    case BAR_GRAPH_TICK_CENTER:
                    {
                        drawPoint.x -= graph->tickLength/2;
                        break;
                    }
                    default:
                        break;
                }
                
                GFX_DrawLine(drawPoint.x, drawPoint.y, graphRect.x + graphRect.width, drawPoint.y);
            }
            else
            {
                GFX_DrawLine(drawPoint.x, drawPoint.y, graphRect.x + graphRect.width, drawPoint.y);
            }

            if (graph->valueAxisLabelsVisible)
            {
                GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->text);
                drawTickLabelWithValue(graph, drawPoint, LA_RELATIVE_POSITION_LEFTOF, originValue);
            }
        }
        else if (graph->minValue >= 0)
        {
            originPoint.y = graphRect.y + graphRect.height;
            originValue = graph->minValue;
        }
        else if (graph->maxValue <= 0)
        {
            originPoint.y = graphRect.y;
            originValue = graph->maxValue;
        }

        //Draw the ticks
        if (graph->valueAxisTicksVisible || graph->valueGridlinesVisible)
        {
            GFX_Point drawPoint = {0}, drawEndPoint = {0};
            int32_t value = 0;

            if (graph->valueAxisTicksVisible)
            {
                switch (graph->valueAxisTicksPosition)
                {
                    case BAR_GRAPH_TICK_OUT:
                    {
                        drawPoint.x = graphRect.x - graph->tickLength;
                        break;
                    }
                    case BAR_GRAPH_TICK_CENTER:
                    {
                        drawPoint.x = graphRect.x - graph->tickLength / 2;
                        break;
                    }
                    case BAR_GRAPH_TICK_IN:
                    {
                        drawPoint.x = graphRect.x;
                        break;
                    }
                    default:
                        break;
                }
                
                drawEndPoint.x = drawPoint.x + graph->tickLength;
                
            }
            else
            {
                drawPoint.x = graphRect.x;
                drawEndPoint = drawPoint;
            }
            
            if (graph->valueGridlinesVisible)
            {
                drawEndPoint.x = graphRect.x + graphRect.width;
            }

            //Start drawing the positive ticks or gridlines
            if (graph->maxValue > 0)
            {
                drawPoint.y = originPoint.y;
                GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
                GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
                for (value = originValue + graph->tickInterval; 
                     (value < graph->maxValue); 
                     value += graph->tickInterval)
                {
                    drawPoint.y = originPoint.y - (int32_t) ((float)(value - originValue) * pixelsPerUnit);
                    
                    if(drawPoint.y > graphRect.y) 
                    {
                        GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
                        GFX_DrawLine(drawPoint.x, drawPoint.y, drawEndPoint.x, drawPoint.y);

                        if (graph->valueAxisLabelsVisible)
                        {
                            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->text);
                            drawTickLabelWithValue(graph, drawPoint, LA_RELATIVE_POSITION_LEFTOF, value);
                        }
                    }
                }
            }
            
            //Start drawing the negative ticks or gridlines
            if (graph->minValue < 0)
            {
                drawPoint.y = originPoint.y;

                GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
                for (value = originValue - graph->tickInterval; 
                     (value > graph->minValue); 
                     value -= graph->tickInterval)
                {
                    drawPoint.y = originPoint.y + (int32_t) ((float) (value - originValue) * -pixelsPerUnit);
                    
                    if (drawPoint.y < (graphRect.y + graphRect.height))
                    {
                        GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
                        GFX_DrawLine(drawPoint.x, drawPoint.y, drawEndPoint.x, drawPoint.y);
                    
                        if (graph->valueAxisLabelsVisible)
                        {
                           GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->text);
                           drawTickLabelWithValue(graph, drawPoint, LA_RELATIVE_POSITION_LEFTOF, value);
                        }
                    }
                }
            }
        }

        //Draw the subticks, only if major ticks are also visible
        if (graph->valueAxisSubticksVisible && graph->valueAxisTicksVisible)
        {
            GFX_Point drawPoint = {0};
            uint32_t subTickLength = graph->tickLength/2;
            int32_t value = 0;

            
            drawPoint.y = originPoint.y;
            switch (graph->valueAxisSubticksPosition)
            {
                case BAR_GRAPH_TICK_OUT:
                {
                    drawPoint.x = graphRect.x - subTickLength;
                    break;
                }
                case BAR_GRAPH_TICK_CENTER:
                {
                    drawPoint.x = graphRect.x - subTickLength / 2;
                    break;
                }
                case BAR_GRAPH_TICK_IN:
                {
                    drawPoint.x = graphRect.x;
                    break;
                }
                default:
                    break;
            }
            
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);

            // Draw the subticks above the origin point
            for (value = originValue + graph->subtickInterval; 
                 (value < graph->maxValue); 
                 value += graph->subtickInterval)
            {
                drawPoint.y = originPoint.y - (int32_t) ((float) (value - originValue) * pixelsPerUnit);

                if (drawPoint.y > graphRect.y)
                    GFX_DrawLine(drawPoint.x, drawPoint.y, drawPoint.x + subTickLength, drawPoint.y);                
            }

            //Start drawing the negative ticks or gridlines
            drawPoint.y = originPoint.y;
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
            for (value = originValue - graph->subtickInterval; 
                 value > graph->minValue; 
                 value -= graph->subtickInterval)
            {
                drawPoint.y = originPoint.y + (int32_t) ((float) (originValue - value) * pixelsPerUnit); 
                if (drawPoint.y < (graphRect.y + graphRect.height))
                    GFX_DrawLine(drawPoint.x, drawPoint.y, drawPoint.x + subTickLength, drawPoint.y);                
            }
        }
        
        //Draw the category ticks
        if ((graph->categAxisTicksVisible == LA_TRUE) && (graph->categories.size > 0))
        {
            uint32_t categoryIndex = 0;
            GFX_Point drawPoint = originPoint;
            uint32_t length = graph->tickLength;
            
            switch (graph->categAxisTicksPosition)
            {
                case BAR_GRAPH_TICK_OUT:
                {
                    drawPoint.y = originPoint.y;
                    break;
                }
                case BAR_GRAPH_TICK_CENTER:
                {
                    drawPoint.y = originPoint.y - graph->tickLength / 2;
                    break;
                }
                case BAR_GRAPH_TICK_IN:
                {
                    drawPoint.y = originPoint.y - graph->tickLength;
                    break;
                }
                default:
                    break;
            }
            
            if (drawPoint.y < graphRect.y)
            {
                length = originPoint.y - graphRect.y;
                drawPoint.y = graphRect.y;
            }
            
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
            
            for (categoryIndex = 1; 
                 ((drawPoint.x < graphRect.x + graphRect.width) && 
                 (categoryIndex < graph->categories.size)); 
                 categoryIndex++)
            {
                drawPoint.x = graphRect.x + categoryIndex * graphRect.width / graph->categories.size;
                GFX_DrawLine(drawPoint.x , drawPoint.y, drawPoint.x, drawPoint.y + length);
            }
        }
        
        //Draw the bars
        if (graph->categories.size > 0 && graph->dataSeries.size > 0)
        {
            if (graph->stacked == LA_TRUE)
            {
                uint32_t barWidthPixels;
                uint32_t categoryIndex = 0, seriesIndex = 0;
                laBarGraphDataSeries * series;
                int32_t * valuePtr;
                int32_t value;
                GFX_Rect valueRect;

                //determine the width each category
                barWidthPixels = (graphRect.width / graph->categories.size) / 3;
                
                for (categoryIndex = 0; 
                     (categoryIndex < graph->categories.size); 
                     categoryIndex++)
                {
                    int32_t bottomValue = 0, topValue = 0;
                    GFX_Point topPoint = originPoint, bottomPoint = originPoint;

                    for (seriesIndex = 0; seriesIndex < graph->dataSeries.size; seriesIndex++)
                    {
                        series = laArray_Get(&graph->dataSeries, seriesIndex);
                        
                        if (categoryIndex >= series->data.size)
                            continue;

                        valuePtr = laArray_Get(&series->data, categoryIndex);
                        if (valuePtr == NULL)
                            continue;
                        
                        value = *valuePtr;

                        valueRect.x = originPoint.x + 
                                      ((categoryIndex * graphRect.width) / graph->categories.size) + 
                                      (graphRect.width / (graph->categories.size * 2)) - 
                                      (barWidthPixels/2);
                        
                        //draw the bars above the origin value
                        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
                        GFX_Set(GFXF_DRAW_COLOR, series->scheme->foreground);

                        if (value > 0 && topValue < graph->maxValue)
                        {
                            topValue += value;

                            if (topValue < originValue)
                                continue;

                            if (topValue > graph->maxValue)
                                topValue = graph->maxValue;

                            valueRect.y = originPoint.y - (int32_t) ((float) (topValue - originValue) * pixelsPerUnit);
                            if (valueRect.y <= graphRect.y)
                                valueRect.y = graphRect.y;

                            GFX_DrawRect(valueRect.x, valueRect.y, barWidthPixels, topPoint.y - valueRect.y);

                            topPoint.y = valueRect.y;
                            
                        }
                        //draw the bars below if the origin value
                        else if (value < 0 && bottomValue > graph->minValue)
                        {
                            bottomValue += value;
                            bottomPoint.y += 1;

                            if (bottomValue > originValue)
                                continue;

                            valueRect.y = originPoint.y + (int32_t) ((float) (originValue - bottomValue) * pixelsPerUnit);
                            if (valueRect.y >= graphRect.y + graphRect.height)
                                valueRect.y = graphRect.y + graphRect.height;

                            GFX_DrawRect(valueRect.x, bottomPoint.y, barWidthPixels, valueRect.y - bottomPoint.y + 1);

                            bottomPoint.y = valueRect.y;
                        }
                        
                    }
                }
                
            }
            else
            {
                uint32_t barWidthPixels;
                uint32_t categoryIndex = 0, seriesIndex = 0;
                laBarGraphDataSeries * series;
                int32_t * valuePtr;
                int32_t value;
                GFX_Rect valueRect;

                //determine the width of each category + L & R margins
                barWidthPixels = (graphRect.width / 
                                  (graph->categories.size * (graph->dataSeries.size + 2)));

                for (seriesIndex = 0; seriesIndex < graph->dataSeries.size; seriesIndex++)
                {
                    series = laArray_Get(&graph->dataSeries, seriesIndex);
                    if (series == NULL)
                        continue;
                    
                    for (categoryIndex = 0; 
                         (categoryIndex < graph->categories.size &&
                          categoryIndex < series->data.size); 
                         categoryIndex++)
                    {
                        valuePtr = laArray_Get(&series->data, categoryIndex);
                        if (valuePtr == NULL)
                            continue;
                        
                        value = *valuePtr;
                        
                        valueRect.x = originPoint.x + 
                                      (categoryIndex * graphRect.width / graph->categories.size) + 
                                      seriesIndex * barWidthPixels + barWidthPixels;
                        
                        //draw the bars above the origin value
                        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
                        GFX_Set(GFXF_DRAW_COLOR, series->scheme->foreground);
                        if (value > originValue && graph->maxValue > originValue)
                        {
                            valueRect.y = originPoint.y - (int32_t) ((float)(value - originValue) * pixelsPerUnit);

                            if (valueRect.y <= graphRect.y)
                                valueRect.y = graphRect.y;

                            GFX_DrawRect(valueRect.x, valueRect.y, barWidthPixels, originPoint.y - valueRect.y);
                        }
                        //draw the bars below the origin value
                        else if (value < originValue && graph->minValue < originValue)
                        {
                            int32_t height;
                            valueRect.y = originPoint.y + 1;
                            
                            height = (int32_t) ((float)(originValue - value) * pixelsPerUnit);
                            if (height >= (graphRect.y + graphRect.height - originPoint.y))
                                height = graphRect.y + graphRect.height - originPoint.y;

                            GFX_DrawRect(valueRect.x, valueRect.y, barWidthPixels, height);
                        }
                    }
                }
            }
        }
        
        // Draw the horizontal borders
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
        GFX_DrawLine(graphRect.x, graphRect.y, graphRect.x + graphRect.width, graphRect.y); //Top            
        GFX_DrawLine(graphRect.x, graphRect.y + graphRect.height, graphRect.x + graphRect.width, graphRect.y + graphRect.height); //Bottom
    }

    nextState(graph);
}

static void _laBarGraphWidget_GetCategoryTextRect(laBarGraphWidget* graph,
                                           uint32_t categoryIndex,
                                           const GFX_Rect * graphRect,
                                           GFX_Rect * textRect,
                                           GFX_Rect * drawRect)
{
    laBarGraphCategory * category;
    GFX_Rect bounds;
    
    *textRect = GFX_Rect_Zero;
    *drawRect = GFX_Rect_Zero;
    
    if (categoryIndex >= graph->categories.size)
        return;
    
    category = laArray_Get(&graph->categories, categoryIndex);
    if (category == NULL)
        return;
    
    laString_GetRect(&category->text, textRect);
    
    bounds.x = 0;
    bounds.y = 0;
    bounds.width = graphRect->width;
    bounds.height = textRect->height;
    
    GFX_RectClip(textRect, &bounds, drawRect);

    //get center of category, arrange text below it
    switch (graph->categAxisTicksPosition)
    {
        case BAR_GRAPH_TICK_OUT:
        {
            textRect->y = graphRect->y + graphRect->height + graph->tickLength + LABEL_OFFSET_MIN_PIX;
            break;
        }
        case BAR_GRAPH_TICK_CENTER:
        {
            textRect->y = graphRect->y + graphRect->height + graph->tickLength / 2 + LABEL_OFFSET_MIN_PIX;
            break;
        }
        case BAR_GRAPH_TICK_IN:
        {
            textRect->y = graphRect->y + graphRect->height + LABEL_OFFSET_MIN_PIX;
            break;
        }
        default:
            break;
    }
                
    textRect->x = graphRect->x + 
                 (categoryIndex * graphRect->width / graph->categories.size) + 
                 (graphRect->width / (graph->categories.size * 2)) - (textRect->width / 2);

    drawRect->x = textRect->x;
    drawRect->y = textRect->y;
}

static void drawString(laBarGraphWidget* graph)
{
    GFX_Rect textRect, drawRect, clipRect, graphRect;
    laBarGraphCategory * category;
    laLayer* layer = laUtils_GetLayer((laWidget*)graph);
    uint32_t categoryIndex = 0;

    if (graph->categAxisLabelsVisible)
    {
    
        _laBarGraphWidget_GetGraphRect(graph, &graphRect);
        
        for (categoryIndex = 0; 
             (categoryIndex < graph->categories.size); 
             categoryIndex++)
        {
            category = laArray_Get(&graph->categories, categoryIndex);
            if (category == NULL)
                return;
                
            _laBarGraphWidget_GetCategoryTextRect(graph,
                                                  categoryIndex,
                                                  &graphRect,
                                                  &textRect,
                                                  &drawRect);
            
            if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
            {
                 GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);

                 GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
                 GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->text);
                 
                 laString_DrawClipped(&category->text,
                                      clipRect.x,
                                      clipRect.y,
                                      clipRect.width,
                                      clipRect.height,
                                      textRect.x,
                                      textRect.y,
                                      &graph->reader);
         
                 if(graph->reader != NULL)
                 {
                     graph->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
                     graph->widget.drawState = WAIT_STRING;

                     return;
                 }
             }
        }
    }

    nextState(graph);
}

static void waitString(laBarGraphWidget* graph)
{
    if(graph->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        graph->reader->run(graph->reader);
        
        return;
    }
    
    // free the reader
    graph->reader->memIntf->heap.free(graph->reader);
    graph->reader = NULL;
    
    graph->widget.drawState = DRAW_STRING;
    
    nextState(graph);
}

static void drawBorder(laBarGraphWidget* graph)
{    
    if(graph->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)graph);
    else if(graph->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)graph);
    
    nextState(graph);
}

void _laBarGraphWidget_Paint(laBarGraphWidget* graph)
{
    laContext* context = laContext_GetActive();
    
    if(graph->widget.scheme == NULL)
    {
        graph->widget.drawState = DONE;
        
        return;
    }
    
    if(graph->widget.drawState == NOT_STARTED)
        nextState(graph);
    
    while(graph->widget.drawState != DONE)
    {
        graph->widget.drawFunc((laWidget*)graph);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_BAR_GRAPH_WIDGET_ENABLED
