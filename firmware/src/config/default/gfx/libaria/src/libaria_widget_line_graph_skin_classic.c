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


#include "gfx/libaria/inc/libaria_widget_line_graph.h"

#if LA_LINE_GRAPH_WIDGET_ENABLED

#include <stdio.h>

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#include "gfx/hal/inc/gfx_draw_arc.h"
#include "gfx/hal/inc/gfx_draw_rect.h"
#include "gfx/hal/inc/gfx_util.h"

#define MAX_TICK_LABEL_DIGITS 10
#define MAX_TICK_LABEL_VALUE 999999999
#define LABEL_OFFSET_MIN_PIX 5

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_LINE_GRAPH,
    DRAW_STRING,
    WAIT_STRING,    
    DRAW_BORDER,
};

static void drawBackground(laLineGraphWidget* graph);
static void drawLineGraph(laLineGraphWidget* graph);
static void drawString(laLineGraphWidget* graph);
static void drawBorder(laLineGraphWidget* graph);
static void waitString(laLineGraphWidget* btn);


static void nextState(laLineGraphWidget* graph)
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
            graph->widget.drawState = DRAW_LINE_GRAPH;
            graph->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawLineGraph;
    
            return;
        }
        case DRAW_LINE_GRAPH:
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

static void drawBackground(laLineGraphWidget* graph)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)graph);
    
    nextState(graph);
}

//Gets the superset (largest) draw rectangle for the labels. 
static void getValueLabelMaxDrawRect(laLineGraphWidget* graph, GFX_Rect * rect)
{
    *rect = GFX_Rect_Zero;
    //Calculate the offset for the value labels
    if (graph->valueAxisLabelsVisible && 
        graph->stringTable != NULL)
    {
        GFX_Rect minLabelRect = {0}, maxLabelRect = {0};
        char strbuff[MAX_TICK_LABEL_DIGITS];
        laString str;

        if (graph->minValue < 0)
        {
            //Protect from overflow
            if (graph->minValue > -MAX_TICK_LABEL_VALUE)
            {
                sprintf(strbuff, "%d", (int) graph->minValue);
            }
            else
            {
                sprintf(strbuff, "---");
            }

            str = laString_CreateFromCharBuffer(strbuff, 
                    GFXU_StringFontIndexLookup(graph->stringTable, graph->ticksLabelsStringID, 0));
            laString_GetRect(&str, &minLabelRect);

            laString_Destroy(&str);
        }

        //Protect from overflow
        if (graph->maxValue < MAX_TICK_LABEL_VALUE) 
        {
            sprintf(strbuff, "%d", (int) graph->maxValue);
        } else 
        {
            sprintf(strbuff, "---");
        }

        str = laString_CreateFromCharBuffer(strbuff,
                GFXU_StringFontIndexLookup(graph->stringTable, graph->ticksLabelsStringID, 0));
        laString_GetRect(&str, &maxLabelRect);

        laString_Destroy(&str);

        rect->width = (maxLabelRect.width > minLabelRect.width) ? 
                    (maxLabelRect.width) : 
                    (minLabelRect.width);

        rect->height = (maxLabelRect.height > minLabelRect.height) ? 
                    (maxLabelRect.height) : 
                    (minLabelRect.height);
    }
}

//Gets the maximum draw rectangle for the category labels
static void getCategoryLabelMaxDrawRect(laLineGraphWidget* graph, GFX_Rect * rect)
{
    uint32_t categoryIndex;
    laLineGraphCategory * category;
    GFX_Rect textRect;
    
    *rect = GFX_Rect_Zero;
    
    for (categoryIndex = 0; 
         (categoryIndex < graph->categories.size); 
         categoryIndex++)
    {
        category = laArray_Get(&graph->categories, categoryIndex);
        if (category == NULL)
            return;
        
        laString_GetRect(&category->text, &textRect);
        
        if (textRect.height > rect->height)
            rect->height = textRect.height;
                    
        if (textRect.width > rect->width)
            rect->width = textRect.width;
    }

}

static void drawTickLabelWithValue(laLineGraphWidget* graph, GFX_Point tickPoint, laRelativePosition position, int32_t value)
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

static void drawSeriesPoint(laLineGraphDataSeries * series, GFX_Point point)
{
    GFX_Set(GFXF_DRAW_COLOR, series->scheme->foreground);
    switch(series->pointType)
    {
        case LINE_GRAPH_DATA_POINT_CIRCLE:
        {
            if (series->fillPoints == LA_TRUE)
                GFX_Set(GFXF_DRAW_THICKNESS, series->pointSize/2);
            else
                GFX_Set(GFXF_DRAW_THICKNESS, 1);

            GFX_DrawArc(point.x, point.y, series->pointSize/2, 0, 360);

            break;
        }
        case LINE_GRAPH_DATA_POINT_SQUARE:
        {
            if (series->fillPoints == LA_TRUE)
                GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
            else
                GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
            
            GFX_DrawRect(point.x - series->pointSize/2, 
                         point.y - series->pointSize/2, 
                         series->pointSize, 
                         series->pointSize);

            break;
        }
        case LINE_GRAPH_DATA_POINT_NONE: //Do nothing
        default:
            break;
    }    
}

//Gets the rectangle of the graph area (without labels or ticks)
void _laLineGraphWidget_GetGraphRect(laLineGraphWidget* graph,
                                           GFX_Rect * graphRect)
{
    GFX_Point p;
    laMargin margin;
    GFX_Rect widgetRect, valueLabelMaxRect, categoryLabelMaxRect;
    
    widgetRect = laUtils_WidgetLocalRect((laWidget*)graph);
    laUtils_RectToLayerSpace((laWidget*)graph, &widgetRect);

    p.x = widgetRect.x;
    p.y = widgetRect.y;
    
    valueLabelMaxRect = GFX_Rect_Zero;
    *graphRect = GFX_Rect_Zero;
    categoryLabelMaxRect = GFX_Rect_Zero;
    
    margin = graph->widget.margin;
    graphRect->x = p.x + margin.left;
    graphRect->width = widgetRect.width - margin.left - margin.right;

    if (graph->valueAxisTicksVisible)
    {
        switch (graph->valueAxisTicksPosition)
        {
            case LINE_GRAPH_TICK_OUT:
            {
                graphRect->x += graph->tickLength;
                graphRect->width -= graph->tickLength;
                break;
            }
            case LINE_GRAPH_TICK_CENTER:
            {
                graphRect->x += graph->tickLength/2;
                graphRect->width -= graph->tickLength/2;
                break;
            }
            default:
                break;
        }
    }

    graphRect->y = p.y + margin.top;
    graphRect->height = widgetRect.height - margin.top - margin.bottom;

    if (graph->valueAxisLabelsVisible)
    {
        getValueLabelMaxDrawRect(graph, &valueLabelMaxRect);

        graphRect->y += valueLabelMaxRect.height / 2 + LABEL_OFFSET_MIN_PIX;
        graphRect->height -= valueLabelMaxRect.height / 2  + LABEL_OFFSET_MIN_PIX;
    }

    if (graph->categAxisTicksVisible)
    {
        switch (graph->categAxisTicksPosition)
        {
            case LINE_GRAPH_TICK_OUT:
            {
                graphRect->height -= graph->tickLength;
                break;
            }
            case LINE_GRAPH_TICK_CENTER:
            {
                graphRect->height -= graph->tickLength/2;
                break;
            }
            default:
                break;
        }
    }

    if (graph->categAxisLabelsVisible)
    {
        getCategoryLabelMaxDrawRect(graph, &categoryLabelMaxRect);
    }

    if (categoryLabelMaxRect.height > (valueLabelMaxRect.height / 2))
    {
        graphRect->height -= (categoryLabelMaxRect.height +  LABEL_OFFSET_MIN_PIX);
    }
    else if (valueLabelMaxRect.height != 0)
    {
        graphRect->height -= (valueLabelMaxRect.height / 2 +  LABEL_OFFSET_MIN_PIX);
    }

    if (graph->fillValueArea == LA_TRUE)
    {
        if ((categoryLabelMaxRect.width / 2) > valueLabelMaxRect.width)
        {
            graphRect->x += (categoryLabelMaxRect.width/2 + LABEL_OFFSET_MIN_PIX * 2);
            graphRect->width -= (categoryLabelMaxRect.width/2 + LABEL_OFFSET_MIN_PIX * 2);
        }
        else
        {
            graphRect->x += (valueLabelMaxRect.width + LABEL_OFFSET_MIN_PIX * 2);
            graphRect->width -= (valueLabelMaxRect.width + LABEL_OFFSET_MIN_PIX * 2);
        }

        graphRect->width -= (categoryLabelMaxRect.width/2 + LABEL_OFFSET_MIN_PIX);
    }
    else if (valueLabelMaxRect.width != 0)
    {
        graphRect->x += (valueLabelMaxRect.width + LABEL_OFFSET_MIN_PIX * 2);
        graphRect->width -= (valueLabelMaxRect.width + LABEL_OFFSET_MIN_PIX * 2);
    }
}

static void drawLineGraph(laLineGraphWidget* graph)
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
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, LA_TRUE);
        
        _laLineGraphWidget_GetGraphRect(graph, &graphRect);
        
        pixelsPerUnit = (float) graphRect.height / ((float) graph->maxValue - (float) graph->minValue);
        
        //Fill the base
        if (graph->fillGraphArea)
        {
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->base);
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
            GFX_DrawRect(graphRect.x, graphRect.y, graphRect.width, graphRect.height);
        }

        //Adjust the horizontal borders if ticks are shown
        if (graph->valueAxisTicksVisible)
        {
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
            switch (graph->valueAxisTicksPosition)
            {
                case LINE_GRAPH_TICK_OUT:
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
                case LINE_GRAPH_TICK_CENTER:
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
                    case LINE_GRAPH_TICK_OUT:
                    {
                        drawPoint.x -= graph->tickLength;
                        break;
                    }
                    case LINE_GRAPH_TICK_CENTER:
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
        originPoint = _laLineGraphWidget_GetOriginPoint(graph);
        if (graph->minValue < 0 && graph->maxValue > 0)
        {
            originValue = 0;
        }
        else if (graph->minValue >= 0)
        {
            originValue = graph->minValue;
        }
        else if (graph->maxValue <= 0)
        {
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
                    case LINE_GRAPH_TICK_OUT:
                    {
                        drawPoint.x = graphRect.x - graph->tickLength;
                        break;
                    }
                    case LINE_GRAPH_TICK_CENTER:
                    {
                        drawPoint.x = graphRect.x - graph->tickLength / 2;
                        break;
                    }
                    case LINE_GRAPH_TICK_IN:
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
        
        //Draw the fills if needed
        if (graph->fillValueArea == LA_TRUE &&
            graph->categories.size > 0 && 
            graph->dataSeries.size > 0)
        {
            uint32_t categoryIndex = 0, seriesIndex = 0;
            laLineGraphDataSeries * series;
            laLineGraphCategory * category;

            if (graph->stacked == LA_TRUE)
            {
                for (categoryIndex = 0; 
                     (categoryIndex < graph->categories.size); 
                     categoryIndex++)
                {
                    category = laArray_Get(&graph->categories, categoryIndex);
                    category->stackValue = 0;
                }

                for (seriesIndex = 0; seriesIndex < graph->dataSeries.size; seriesIndex++)
                {
                    GFX_Point prevPoint = originPoint;
                    GFX_Point valuePoint = originPoint;
                    int32_t topValue = 0;

                    series = laArray_Get(&graph->dataSeries, seriesIndex);

                    for (categoryIndex = 0; 
                         (categoryIndex < graph->categories.size); 
                         categoryIndex++)
                    {
                        int32_t * valuePtr = NULL;
                        int32_t value;

                        if (categoryIndex < series->data.size)
                            valuePtr = laArray_Get(&series->data, categoryIndex);

                        if (valuePtr == NULL)
                            value = 0;
                        else
                            value = *valuePtr;

                        category = laArray_Get(&graph->categories, categoryIndex);

                        valuePoint.x = originPoint.x + 
                                      ((categoryIndex * graphRect.width) / (graph->categories.size - 1));

                        topValue = category->stackValue;
                        topValue += value;

                        valuePoint.y = originPoint.y - (int32_t) ((float) (topValue - originValue) * pixelsPerUnit);

                        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
                        GFX_Set(GFXF_DRAW_COLOR, series->scheme->base);

                        if (categoryIndex > 0)
                        {
                            laLineGraphCategory * prevCategory = laArray_Get(&graph->categories, categoryIndex - 1);
                            int32_t * prevOldValue;
                            //Get the four points
                            GFX_Point nowOld, nowNew; //Old and new points of current category
                            GFX_Point prevOld, prevNew; //old and new points of previous category
                            GFX_Point scanPointA;
                            GFX_Point scanPointB;

                            nowNew = valuePoint;
                            nowOld = nowNew;

                            prevNew = prevPoint;
                            prevOld = prevNew;

                            nowOld.y = originPoint.y - (int32_t) ((float) (category->stackValue  - originValue) * pixelsPerUnit);

                            prevOldValue = laArray_Get(&series->data, categoryIndex - 1);
                            prevOld.y = originPoint.y - (int32_t) ((float) (prevCategory->stackValue - *prevOldValue  - originValue) * pixelsPerUnit);

                            //Fill the region using vertical lines for now
                            for (scanPointA.x = prevNew.x; scanPointA.x < nowNew.x; scanPointA.x++)
                            {
                                scanPointA.y = GFX_UtilGetYGivenXOnLine(prevNew, nowNew, scanPointA.x);
                                if (scanPointA.y < graphRect.y)
                                    scanPointA.y = graphRect.y;
                                else if (scanPointA.y > graphRect.y + graphRect.height)
                                    scanPointA.y = graphRect.y + graphRect.height;

                                scanPointB.x = scanPointA.x;
                                scanPointB.y = GFX_UtilGetYGivenXOnLine(prevOld, nowOld, scanPointA.x);
                                if (scanPointB.y < graphRect.y)
                                    scanPointB.y = graphRect.y;
                                else if (scanPointB.y > graphRect.y + graphRect.height)
                                    scanPointB.y = graphRect.y + graphRect.height;

                                GFX_DrawLine(scanPointA.x,
                                             scanPointA.y,
                                             scanPointB.x,
                                             scanPointB.y);
                            }
                        }

                        category->stackValue = topValue;
                        prevPoint = valuePoint;
                    }
                }
            }
            else 
            {
                //Do the fills first, then plot the points/lines
                if (graph->fillValueArea == LA_TRUE)
                {
                    for (seriesIndex = 0; seriesIndex < graph->dataSeries.size; seriesIndex++)
                    {
                        GFX_Point prevPoint = originPoint;
                        GFX_Point valuePoint = originPoint;

                        series = laArray_Get(&graph->dataSeries, seriesIndex);
                        if (series == NULL)
                            continue;

                        for (categoryIndex = 0; 
                             (categoryIndex < graph->categories.size &&
                             categoryIndex < series->data.size); 
                             categoryIndex++)
                        {
                            int32_t * valuePtr;
                            int32_t value;

                            valuePtr = laArray_Get(&series->data, categoryIndex);
                            if (valuePtr == NULL)
                                continue;

                            value = *valuePtr;

                            if (graph->fillValueArea == LA_TRUE  &&
                                graph->categories.size > 1)
                            {
                                valuePoint.x = originPoint.x + 
                                          ((categoryIndex * graphRect.width) / (graph->categories.size - 1));
                            }
                            else
                            {
                                valuePoint.x = originPoint.x + 
                                          (((categoryIndex + 1) * graphRect.width) / (graph->categories.size + 1));    
                            }

                            if (value >= originValue)
                            {
                                valuePoint.y = originPoint.y - (int32_t) ((float)(value - originValue) * pixelsPerUnit);
                            }
                            //draw the points below the origin value
                            else if (value < originValue)
                            {
                                valuePoint.y = originPoint.y + (int32_t) ((float)(originValue - value) * pixelsPerUnit);
                            }

                            if (categoryIndex > 0)
                            {
                                GFX_Point scanPoint = prevPoint;
                                GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
                                GFX_Set(GFXF_DRAW_COLOR, series->scheme->base);

                                while(scanPoint.x <= valuePoint.x)
                                {
                                    scanPoint.y = GFX_UtilGetYGivenXOnLine(prevPoint, valuePoint, scanPoint.x);
                                    if (scanPoint.y <= graphRect.y)
                                        scanPoint.y = graphRect.y;
                                    else if (scanPoint.y >= graphRect.y + graphRect.height)
                                        scanPoint.y = graphRect.y + graphRect.height;

                                    GFX_DrawLine(scanPoint.x,
                                                 scanPoint.y,
                                                 scanPoint.x,
                                                 originPoint.y);

                                    scanPoint.x++;
                                }
                            }

                            prevPoint = valuePoint;
                        }
                    }
                }                
            }
        }
        
        //Draw the origin line
        if (originValue == 0)
        {
            GFX_Point drawPoint = {0};
            drawPoint = originPoint;

            //Draw the origin line
            GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
            if (graph->valueAxisTicksVisible)
            {
                switch (graph->valueAxisTicksPosition)
                {
                    case LINE_GRAPH_TICK_OUT:
                    {
                        drawPoint.x -= graph->tickLength;
                        break;
                    }
                    case LINE_GRAPH_TICK_CENTER:
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
        
        //Draw the category ticks
        if ((graph->categAxisTicksVisible == LA_TRUE) && (graph->categories.size > 0))
        {
            uint32_t categoryIndex = 0;
            GFX_Point drawPoint = originPoint;
            uint32_t length = graph->tickLength;
            
            switch (graph->categAxisTicksPosition)
            {
                case LINE_GRAPH_TICK_OUT:
                {
                    drawPoint.y = originPoint.y;
                    break;
                }
                case LINE_GRAPH_TICK_CENTER:
                {
                    drawPoint.y = originPoint.y - graph->tickLength / 2;
                    break;
                }
                case LINE_GRAPH_TICK_IN:
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
            
            for (categoryIndex = 0; 
                 ((drawPoint.x < graphRect.x + graphRect.width) && 
                 (categoryIndex < graph->categories.size)); 
                 categoryIndex++)
            {
                if (graph->fillValueArea == LA_TRUE &&
                    graph->categories.size > 1)
                {
                    drawPoint.x = graphRect.x + categoryIndex * graphRect.width / (graph->categories.size - 1);
                }
                else
                {
                    drawPoint.x = graphRect.x + (categoryIndex + 1) * graphRect.width / (graph->categories.size + 1);
                }
                GFX_DrawLine(drawPoint.x , drawPoint.y, drawPoint.x, drawPoint.y + length);
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
                case LINE_GRAPH_TICK_OUT:
                {
                    drawPoint.x = graphRect.x - subTickLength;
                    break;
                }
                case LINE_GRAPH_TICK_CENTER:
                {
                    drawPoint.x = graphRect.x - subTickLength / 2;
                    break;
                }
                case LINE_GRAPH_TICK_IN:
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
        
        //Draw the borders
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, graph->widget.scheme->foreground);
        GFX_DrawLine(graphRect.x, graphRect.y, graphRect.x, graphRect.y + graphRect.height); //Left
        GFX_DrawLine(graphRect.x + graphRect.width, graphRect.y, graphRect.x + graphRect.width, graphRect.y + graphRect.height); //Right
        GFX_DrawLine(graphRect.x, graphRect.y, graphRect.x + graphRect.width, graphRect.y); //Top            
        GFX_DrawLine(graphRect.x, graphRect.y + graphRect.height, graphRect.x + graphRect.width, graphRect.y + graphRect.height); //Bottom

        //Draw the points/lines
        if (graph->categories.size > 0 && graph->dataSeries.size > 0)
        {
            uint32_t categoryIndex = 0, seriesIndex = 0;
            laLineGraphDataSeries * series;
            laLineGraphCategory * category;
            
            if (graph->stacked == LA_TRUE)
            {
                for (categoryIndex = 0; 
                     categoryIndex < graph->categories.size; 
                     categoryIndex++)
                {
                    category = laArray_Get(&graph->categories, categoryIndex);
                    category->stackValue = 0;
                }

                for (seriesIndex = 0; seriesIndex < graph->dataSeries.size; seriesIndex++)
                {
                    GFX_Point valuePoint = originPoint;
                    int32_t topValue = 0;

                    series = laArray_Get(&graph->dataSeries, seriesIndex);

                    for (categoryIndex = 0; 
                         categoryIndex < graph->categories.size; 
                         categoryIndex++)
                    {

                        int32_t * valuePtr = NULL;
                        int32_t value;

                        if (categoryIndex < series->data.size)
                            valuePtr = laArray_Get(&series->data, categoryIndex);

                        value = (valuePtr == NULL) ? 0 : *valuePtr;

                        category = laArray_Get(&graph->categories, categoryIndex);

                        if (graph->fillValueArea == LA_TRUE &&
                            graph->categories.size > 1)
                        {
                            valuePoint.x = originPoint.x + 
                                      ((categoryIndex * graphRect.width) / (graph->categories.size - 1));
                        }
                        else
                        {
                            valuePoint.x = originPoint.x + 
                                      (((categoryIndex + 1) * graphRect.width) / (graph->categories.size + 1));    
                        }

                        topValue = category->stackValue;

                        topValue += value;

                        valuePoint.y = originPoint.y - (int32_t) ((float) (topValue - originValue) * pixelsPerUnit);

                        if (series->drawLines == LA_TRUE &&
                            categoryIndex > 0)
                        {
                            GFX_Point start = {-1, -1}, end;
                            laLineGraphCategory * prevCategory = laArray_Get(&graph->categories, categoryIndex - 1);

                            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
                            GFX_Set(GFXF_DRAW_COLOR, series->scheme->foreground);
                            
                            if (graph->fillValueArea == LA_TRUE &&
                                graph->categories.size > 1)
                            {
                                start.x = originPoint.x + 
                                    ((categoryIndex - 1)* graphRect.width) / (graph->categories.size - 1);
                            }
                            else
                            {
                                start.x = originPoint.x + 
                                  (((categoryIndex) * graphRect.width) / (graph->categories.size + 1));    
                            }

                            start.y = originPoint.y - (int32_t) ((float) (prevCategory->stackValue  - originValue) * pixelsPerUnit);
                        
                            end.x = valuePoint.x;
                            end.y = valuePoint.y;

                            if (start.y < graphRect.y)
                            {

                                start.x = GFX_UtilGetXGivenYOnLine(start,
                                                             valuePoint,
                                                             graphRect.y);
                                start.y = graphRect.y;

                            }
                            else if (start.y > graphRect.y + graphRect.height)
                            {
                                start.x = GFX_UtilGetXGivenYOnLine(start,
                                                             valuePoint,
                                                             graphRect.y + graphRect.height);
                                start.y = graphRect.y + graphRect.height;
                            }

                            if (end.y < graphRect.y)
                            {
                                end.x = GFX_UtilGetXGivenYOnLine(end,
                                                                 start,
                                                                 graphRect.y);
                                end.y = graphRect.y;
                            }
                            else if (end.y > graphRect.y + graphRect.height)
                            {
                                end.x = GFX_UtilGetXGivenYOnLine(end,
                                                           start,
                                                           graphRect.y + graphRect.height);
                                end.y = graphRect.y + graphRect.height;
                            }

                            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
                            GFX_Set(GFXF_DRAW_COLOR, series->scheme->foreground);

                            if (start.x != end.x || start.y != end.y)
                                GFX_DrawLine(start.x,
                                             start.y,
                                             end.x,
                                             end.y);
                        }

                        //Draw the point if it's within graph area
                        if ((series->pointType != LINE_GRAPH_DATA_POINT_NONE) &&
                            (valuePoint.y >= graphRect.y) && 
                            (valuePoint.y <= graphRect.y + graphRect.height))
                        {
                            drawSeriesPoint(series, valuePoint);
                        }

                        category->stackValue = topValue;
                    }
                }
            }
            //not stacked
            else
            {
                //determine the width of each category + L & R margins
                for (seriesIndex = 0; seriesIndex < graph->dataSeries.size; seriesIndex++)
                {
                    GFX_Point prevPoint = originPoint;
                    GFX_Point valuePoint = originPoint;

                    series = laArray_Get(&graph->dataSeries, seriesIndex);
                    if (series == NULL)
                        continue;

                    for (categoryIndex = 0; 
                         (categoryIndex < graph->categories.size &&
                         categoryIndex < series->data.size); 
                         categoryIndex++)
                    {
                        int32_t * valuePtr;
                        int32_t value;
                        
                        valuePtr = laArray_Get(&series->data, categoryIndex);
                        if (valuePtr == NULL)
                            continue;

                        value = *valuePtr;
                        
                        if (graph->fillValueArea == LA_TRUE &&
                            graph->categories.size > 1)
                        {
                            valuePoint.x = originPoint.x + 
                                      ((categoryIndex * graphRect.width) / (graph->categories.size - 1));
                        }
                        else
                        {
                            valuePoint.x = originPoint.x + 
                                      (((categoryIndex + 1) * graphRect.width) / (graph->categories.size + 1));    
                        }
                        
                        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
                        GFX_Set(GFXF_DRAW_COLOR, series->scheme->foreground);
                        if (value >= originValue)
                        {
                            valuePoint.y = originPoint.y - (int32_t) ((float)(value - originValue) * pixelsPerUnit);
                        }
                        //draw the points below the origin value
                        else if (value < originValue)
                        {
                            valuePoint.y = originPoint.y + (int32_t) ((float)(originValue - value) * pixelsPerUnit);
                        }
                        
                        if (series->drawLines == LA_TRUE &&
                            categoryIndex > 0)
                        {
                            GFX_Point start, end;

                            start = prevPoint;
                            end.x = valuePoint.x;
                            end.y = valuePoint.y;

                            if (start.y < graphRect.y)
                            {
                                start.y = graphRect.y;
                                start.x = GFX_UtilGetXGivenYOnLine(prevPoint,
                                                               valuePoint,
                                                               start.y);
                            }
                            else if (start.y > graphRect.y + graphRect.height)
                            {
                                start.y = graphRect.y + graphRect.height;
                                start.x = GFX_UtilGetXGivenYOnLine(prevPoint,
                                                               valuePoint,
                                                               start.y);
                            }

                            if (end.y < graphRect.y)
                            {
                                end.y = graphRect.y;
                                end.x = GFX_UtilGetXGivenYOnLine(prevPoint,
                                                             valuePoint,
                                                             end.y);
                            }
                            else if (end.y > graphRect.y + graphRect.height)
                            {
                                end.y = graphRect.y + graphRect.height;
                                end.x = GFX_UtilGetXGivenYOnLine(prevPoint,
                                                             valuePoint,
                                                             end.y);
                            }

                            //Draw lines
                            if (start.x != end.x || start.y != end.y)
                            {
                                GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
                                GFX_Set(GFXF_DRAW_COLOR, series->scheme->foreground);

                                GFX_DrawLine(start.x,
                                                     start.y,
                                                     end.x,
                                                     end.y);
                            }
                        }
                        
                        //Draw the point if it's within graph area
                        if (series->pointType != LINE_GRAPH_DATA_POINT_NONE && 
                            valuePoint.y >= graphRect.y &&
                            valuePoint.y <= graphRect.y + graphRect.height)
                        {
                            drawSeriesPoint(series, valuePoint);
                        }

                        prevPoint = valuePoint;
                    }
                }
            }
        }
    }

    nextState(graph);
}

static void _laLineGraphWidget_GetCategoryTextRect(laLineGraphWidget* graph,
                                           uint32_t categoryIndex,
                                           const GFX_Rect * graphRect,
                                           GFX_Rect * textRect,
                                           GFX_Rect * drawRect)
{
    laLineGraphCategory * category;
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
        case LINE_GRAPH_TICK_OUT:
        {
            textRect->y = graphRect->y + graphRect->height + graph->tickLength + LABEL_OFFSET_MIN_PIX;
            break;
        }
        case LINE_GRAPH_TICK_CENTER:
        {
            textRect->y = graphRect->y + graphRect->height + graph->tickLength / 2 + LABEL_OFFSET_MIN_PIX;
            break;
        }
        case LINE_GRAPH_TICK_IN:
        {
            textRect->y = graphRect->y + graphRect->height + LABEL_OFFSET_MIN_PIX;
            break;
        }
        default:
            break;
    }

    if (graph->fillValueArea == LA_TRUE  &&
        graph->categories.size > 1)
    {
        textRect->x = graphRect->x + 
                     ((categoryIndex)* graphRect->width / (graph->categories.size - 1)) - textRect->width/2;
    }
    else
    {
        textRect->x = graphRect->x + 
                     ((categoryIndex + 1)* graphRect->width / (graph->categories.size + 1)) - textRect->width/2;
    }

    drawRect->x = textRect->x;
    drawRect->y = textRect->y;
}

static void drawString(laLineGraphWidget* graph)
{
    GFX_Rect textRect, drawRect, clipRect, graphRect;
    laLineGraphCategory * category;
    laLayer* layer = laUtils_GetLayer((laWidget*)graph);
    uint32_t categoryIndex = 0;

    if (graph->categAxisLabelsVisible)
    {
    
        _laLineGraphWidget_GetGraphRect(graph, &graphRect);
        
        for (categoryIndex = 0; 
             (categoryIndex < graph->categories.size); 
             categoryIndex++)
        {
            category = laArray_Get(&graph->categories, categoryIndex);
            if (category == NULL)
                return;
                
            _laLineGraphWidget_GetCategoryTextRect(graph,
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

static void waitString(laLineGraphWidget* graph)
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

static void drawBorder(laLineGraphWidget* graph)
{    
    if(graph->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)graph);
    else if(graph->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)graph);
    
    nextState(graph);
}

void _laLineGraphWidget_Paint(laLineGraphWidget* graph)
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

#endif // LA_LINE_GRAPH_WIDGET_ENABLED
