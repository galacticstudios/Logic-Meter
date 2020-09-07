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
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_layer.h"

#define DEFAULT_WIDTH           101
#define DEFAULT_HEIGHT          101

#define DEFAULT_TICK_LENGTH     5
#define DEFAULT_TICK_INTERVAL   10
#define DEFAULT_SUBTICK_INTERVAL   5
#define DEFAULT_MAX_VALUE       100
#define DEFAULT_MIN_VALUE       0

void _laBarGraphWidget_Constructor(laBarGraphWidget* graph)
{
    _laWidget_Constructor((laWidget*)graph);

    graph->widget.type = LA_WIDGET_ARC;
    
    graph->widget.destructor = (laWidget_Destructor_FnPtr)&_laBarGraphWidget_Destructor;

    // override base class methods
    graph->widget.paint = (laWidget_Paint_FnPtr)&_laBarGraphWidget_Paint;

    graph->widget.rect.width = DEFAULT_WIDTH;
    graph->widget.rect.height = DEFAULT_HEIGHT;

    graph->widget.borderType = LA_WIDGET_BORDER_NONE;
    graph->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;
    
    graph->tickLength = DEFAULT_TICK_LENGTH;
    graph->fillGraphArea = LA_TRUE;
    graph->stacked = LA_FALSE;
    graph->maxValue = DEFAULT_MAX_VALUE;
    graph->minValue = DEFAULT_MIN_VALUE;
    graph->tickInterval = DEFAULT_TICK_INTERVAL;
    graph->subtickInterval = DEFAULT_SUBTICK_INTERVAL;
    
    graph->valueGridlinesVisible = LA_TRUE;
    graph->valueAxisLabelsVisible = LA_TRUE;
    graph->valueAxisTicksVisible = LA_TRUE;
    graph->valueAxisSubticksVisible = LA_TRUE;
    graph->valueAxisTicksPosition = BAR_GRAPH_TICK_CENTER;
    graph->valueAxisSubticksPosition = BAR_GRAPH_TICK_CENTER;
    
    graph->categAxisLabelsVisible = LA_TRUE;
    graph->categAxisTicksVisible = LA_TRUE;
    graph->categAxisTicksPosition = BAR_GRAPH_TICK_CENTER;
    
    laArray_Create(&graph->dataSeries);
    laArray_Create(&graph->categories);
}

void _laBarGraphWidget_Destructor(laBarGraphWidget* graph)
{
    laBarGraphWidget_DestroyAll(graph);
    
    _laWidget_Destructor((laWidget*)graph);
}

//Gets the maximum draw rectangle for the category labels
static void getCategoryLabelMaxDrawRect(laBarGraphWidget* graph, GFX_Rect * rect)
{
    uint32_t categoryIndex;
    laBarGraphCategory * category;
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

//Gets the superset (largest) draw rectangle for the labels. 
static void getValueLabelMaxDrawRect(laBarGraphWidget* graph, GFX_Rect * rect)
{
    //Calculate the offset for the value labels
    if (graph->valueAxisLabelsVisible)
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

//Gets the rectangle of the graph area (without labels or ticks)
void _laBarGraphWidget_GetGraphRect(laBarGraphWidget* graph,
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
            case BAR_GRAPH_TICK_OUT:
            {
                graphRect->x += graph->tickLength;
                graphRect->width -= graph->tickLength;
                break;
            }
            case BAR_GRAPH_TICK_CENTER:
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

        graphRect->x += (valueLabelMaxRect.width + LABEL_OFFSET_MIN_PIX * 2);
        graphRect->width -= (valueLabelMaxRect.width + LABEL_OFFSET_MIN_PIX * 2);

        graphRect->y += valueLabelMaxRect.height / 2 + LABEL_OFFSET_MIN_PIX;
        graphRect->height -= valueLabelMaxRect.height / 2  + LABEL_OFFSET_MIN_PIX;
    }

    if (graph->categAxisTicksVisible)
    {
        switch (graph->categAxisTicksPosition)
        {
            case BAR_GRAPH_TICK_OUT:
            {
                graphRect->height -= graph->tickLength;
                break;
            }
            case BAR_GRAPH_TICK_CENTER:
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
}

laResult _laBarGraphWidget_GetBarRect(laBarGraphWidget* graph,
                                      uint32_t seriesID,
                                      uint32_t categoryID,
                                      GFX_Rect * rect)
{
    GFX_Rect graphRect;
    
    if (rect == NULL)
        return LA_FAILURE;
    
    _laBarGraphWidget_GetGraphRect(graph, &graphRect);
    
    rect->height = graphRect.height;
    rect->y = graphRect.y;

    if (graph->stacked == LA_TRUE)
    {
        rect->width = (graphRect.width / graph->categories.size) / 3;
        
        rect->x = graphRect.x + 
              ((categoryID * graphRect.width) / graph->categories.size) + 
              (graphRect.width / (graph->categories.size * 2)) - 
              (rect->width/2);
    }
    else
    {
        rect->width = (graphRect.width / 
                          (graph->categories.size * (graph->dataSeries.size + 2)));
        
        rect->x = graphRect.x + 
              (categoryID * graphRect.width / graph->categories.size) + 
              seriesID * rect->width + rect->width;

    }

    return LA_SUCCESS;
}

laBarGraphWidget* laBarGraphWidget_New()
{
    laBarGraphWidget* graph = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    graph = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laBarGraphWidget));

    if(graph == NULL)
        return NULL;
    
    _laBarGraphWidget_Constructor(graph);

    return graph;
}

uint32_t laBarGraphWidget_GetTickLength(laBarGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->tickLength;
}

laResult laBarGraphWidget_SetTickLength(laBarGraphWidget* graph, uint32_t length)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->tickLength == length)
        return LA_SUCCESS;
        
    graph->tickLength = length;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laBarGraphWidget_GetStacked(laBarGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->stacked;
}

laResult laBarGraphWidget_SetStacked(laBarGraphWidget* graph, laBool stacked)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->stacked == stacked)
        return LA_SUCCESS;
        
    graph->stacked = stacked;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

uint32_t laBarGraphWidget_GetMaxValue(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->maxValue;
}

laResult laBarGraphWidget_SetMaxValue(laBarGraphWidget* graph, laBarGraphValueAxis axis, int32_t value)
{
    if(graph == NULL)
        return LA_FAILURE;

    if (value <= graph->minValue)
        return LA_FAILURE;
    
    if(graph->maxValue == value)
        return LA_SUCCESS;
        
    graph->maxValue = value;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

uint32_t laBarGraphWidget_GetMinValue(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->minValue;
}

laResult laBarGraphWidget_SetMinValue(laBarGraphWidget* graph, laBarGraphValueAxis axis, int32_t value)
{
    if(graph == NULL)
        return LA_FAILURE;
    
    if (value >= graph->maxValue)
        return LA_FAILURE;

    if(graph->minValue == value)
        return LA_SUCCESS;
        
    graph->minValue = value;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laBarGraphWidget_GetValueAxisLabelsVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisLabelsVisible;
}

laResult laBarGraphWidget_SetValueAxisLabelsVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisLabelsVisible == visible)
        return LA_SUCCESS;
        
    graph->valueAxisLabelsVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laBarGraphWidget_GetFillGraphArea(laBarGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->fillGraphArea;
}

laResult laBarGraphWidget_SetFillGraphArea(laBarGraphWidget* graph, laBool fill)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->fillGraphArea == fill)
        return LA_SUCCESS;
        
    graph->fillGraphArea = fill;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laBarGraphWidget_GetGridlinesVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueGridlinesVisible;
}

laResult laBarGraphWidget_SetGridlinesVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueGridlinesVisible == visible)
        return LA_SUCCESS;
        
    graph->valueGridlinesVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laBarGraphWidget_GetValueAxisTicksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisTicksVisible;
}

laResult laBarGraphWidget_SetValueAxisTicksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisTicksVisible == visible)
        return LA_SUCCESS;
        
    graph->valueAxisTicksVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

uint32_t laBarGraphWidget_GetValueAxisTickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->tickInterval;
}

laResult laBarGraphWidget_SetValueAxisTickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis, uint32_t interval)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->tickInterval == interval)
        return LA_SUCCESS;
        
    graph->tickInterval = interval;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

uint32_t laBarGraphWidget_GetValueAxisSubtickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->subtickInterval;
}

laResult laBarGraphWidget_SetValueAxisSubtickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis, uint32_t interval)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->subtickInterval == interval)
        return LA_SUCCESS;
        
    graph->subtickInterval = interval;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laBarGraphWidget_GetValueAxisSubticksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisSubticksVisible;
}

laResult laBarGraphWidget_SetValueAxisSubticksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisSubticksVisible == visible)
        return LA_SUCCESS;
        
    graph->valueAxisSubticksVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laBarGraphWidget_GetCategoryAxisTicksVisible(laBarGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->categAxisTicksVisible;
}

laResult laBarGraphWidget_SetCategoryAxisTicksVisible(laBarGraphWidget* graph, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->categAxisTicksVisible == visible)
        return LA_SUCCESS;
        
    graph->categAxisTicksVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laResult laBarGraphWidget_AddCategory(laBarGraphWidget* graph, uint32_t * id)
{
    laBarGraphCategory * category;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    category = laContext_GetActive()->memIntf.heap.malloc(sizeof(laBarGraphCategory));
    if (category == NULL)
        return LA_FAILURE;
    
    laString_Initialize(&category->text);
    
    laArray_PushBack(&graph->categories, category);
    
    if (id != NULL)
    {
        *id = graph->categories.size;
    }
    
    laWidget_Invalidate((laWidget*)graph);
    
    return LA_SUCCESS;
}

laResult laBarGraphWidget_GetCategoryText(laBarGraphWidget* graph, uint32_t categoryID, laString * str)
{
    laBarGraphCategory * category;
    
    if(graph == NULL)
        return LA_FAILURE;
    
    if (categoryID >= graph->categories.size)
        return LA_FAILURE;
    
    category = laArray_Get(&graph->categories, categoryID);
    if (category == NULL)
        return LA_FAILURE;
    
    return laString_Copy(str, &category->text);
}

laResult laBarGraphWidget_SetCategoryText(laBarGraphWidget* graph, int32_t categoryID, laString str)
{
    laBarGraphCategory * category;
    
    if(graph == NULL)
        return LA_FAILURE;
    
    if (categoryID >= (int32_t) graph->categories.size)
        return LA_FAILURE;
    
    if (categoryID < 0)
        categoryID = graph->categories.size - 1;
    
    category = laArray_Get(&graph->categories, categoryID);
    if (category == NULL)
        return LA_FAILURE;
    
    if(laString_Copy(&category->text, &str) == LA_FAILURE)
        return LA_FAILURE;
    
    laWidget_Invalidate((laWidget*)graph);
    
    return LA_SUCCESS;
}

laResult laBarGraphWidget_AddSeries(laBarGraphWidget* graph, uint32_t * seriesID)
{
    laBarGraphDataSeries * series;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    series = laContext_GetActive()->memIntf.heap.malloc(sizeof(laBarGraphDataSeries));
    if (series == NULL)
        return LA_FAILURE;

    laArray_Create(&series->data);
    series->axis = BAR_GRAPH_AXIS_0;
    series->scheme = graph->widget.scheme;
    
    laArray_PushBack(&graph->dataSeries, series);
    
    if (seriesID != NULL)
    {
        *seriesID = graph->dataSeries.size;
    }
    
    laWidget_Invalidate((laWidget*)graph);
    
    return LA_SUCCESS;
}

laResult laBarGraphWidget_AddDataToSeries(laBarGraphWidget* graph, uint32_t seriesID, int32_t value, uint32_t * index)
{
    laBarGraphDataSeries * series;
    int32_t * data;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    if (seriesID >= graph->dataSeries.size)
        return LA_FAILURE;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FAILURE;
    
    data = laContext_GetActive()->memIntf.heap.malloc(sizeof(data));
    if (data == NULL)
        return LA_FAILURE;
    
    *data = value;
    
    laArray_PushBack(&series->data, (void *) data);

    if (index != NULL)
    {
       *index = series->data.size - 1;
    }
    
    laWidget_Invalidate((laWidget*)graph);
    
    return LA_SUCCESS;
}

laResult laBarGraphWidget_SetDataInSeries(laBarGraphWidget* graph, 
                                          uint32_t seriesID,
                                          uint32_t index,
                                          int32_t value)
{
    laBarGraphDataSeries * series;
    int32_t * data;
    GFX_Rect damagedRect;
    GFX_PipelineMode pipelineMode;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    if (seriesID >= graph->dataSeries.size)
        return LA_FAILURE;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FAILURE;

    if (index >= series->data.size)
        return LA_FAILURE;

    data = laArray_Get(&series->data, index);
    if (data == NULL)
        return LA_FAILURE;
    
    if (value == *data)
        return LA_SUCCESS;
    
    *data = value;

    //If GPU is enabled, just invalidate the whole widget
    GFX_Get(GFXF_DRAW_PIPELINE_MODE, &pipelineMode);
    if (pipelineMode == GFX_PIPELINE_GPU)
    {
        laWidget_Invalidate((laWidget*)graph);
    }
    else
    {
        _laBarGraphWidget_GetBarRect(graph,
                                     seriesID,
                                     index,
                                     &damagedRect);


        laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)graph),
                              &damagedRect,
                              LA_FALSE);
    }
    
    return LA_SUCCESS;
}

laScheme * laBarGraphWidget_GetSeriesScheme(laBarGraphWidget* graph, uint32_t seriesID)
{
    laBarGraphDataSeries * series;
    
    if(graph == NULL)
        return NULL;
    
    if (seriesID >= graph->dataSeries.size)
        return NULL;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return NULL;
    
    return series->scheme;
}

laResult laBarGraphWidget_SetSeriesScheme(laBarGraphWidget* graph, int32_t seriesID, laScheme * scheme)
{
    laBarGraphDataSeries * series;
    
    if(graph == NULL || scheme == NULL)
        return LA_FAILURE;
    
    if (seriesID >= (int32_t) graph->dataSeries.size)
        return LA_FAILURE;
    
    if (seriesID < 0)
        seriesID = graph->dataSeries.size - 1;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FAILURE;
    
    series->scheme = scheme;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laResult laBarGraphWidget_DestroyAll(laBarGraphWidget* graph)
{
    laBarGraphCategory * category;
    laBarGraphDataSeries * series;
    uint32_t i;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    // Free category string data
    for (i = 0; i < graph->categories.size; i++)
    {
        category = laArray_Get(&graph->categories, i);
        laString_Destroy(&category->text);
    }

    //Free the category array and contents
    laArray_Destroy(&graph->categories);
    
    // Free category string data
    for (i = 0; i < graph->dataSeries.size; i++)
    {
        series = laArray_Get(&graph->dataSeries, i);
        if (series != NULL)
        {
            //Destroy the data list and containers
            laArray_Destroy(&series->data);
             //Destroy the pointer list
            laArray_Destroy(&series->data);
        }
    }
    
    laArray_Destroy(&graph->dataSeries);
    
    laWidget_Invalidate((laWidget*)graph);
    
    return LA_SUCCESS;
}

laResult laBarGraphWidget_SetStringTable(laBarGraphWidget* graph, 
                                         GFXU_StringTableAsset * stringTable)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->stringTable == stringTable)
        return LA_SUCCESS;
        
    graph->stringTable = stringTable;
    
    laWidget_Invalidate((laWidget*) graph);
        
    return LA_SUCCESS;
}

laResult laBarGraphWidget_SetTicksLabelsStringID(laBarGraphWidget* graph, 
                                                 uint32_t stringID)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->ticksLabelsStringID == stringID)
        return LA_SUCCESS;
        
    graph->ticksLabelsStringID = stringID;
    
    laWidget_Invalidate((laWidget*) graph);
        
    return LA_SUCCESS;
}

laBool laBarGraphWidget_GetCategoryAxisLabelsVisible(laBarGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->categAxisLabelsVisible;
}

laResult laBarGraphWidget_SetCategoryAxisLabelsVisible(laBarGraphWidget* graph, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->categAxisLabelsVisible == visible)
        return LA_SUCCESS;
        
    graph->categAxisLabelsVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBarGraphTickPosition laBarGraphWidget_GetValueAxisTicksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisTicksPosition;
}
laResult laBarGraphWidget_SetValueAxisTicksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBarGraphTickPosition position)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisTicksPosition == position)
        return LA_SUCCESS;
        
    graph->valueAxisTicksPosition = position;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS; 
}

laBarGraphTickPosition laBarGraphWidget_GetValueAxisSubticksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisSubticksPosition;
}

laResult laBarGraphWidget_SetValueAxisSubticksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBarGraphTickPosition position)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisSubticksPosition == position)
        return LA_SUCCESS;
        
    graph->valueAxisSubticksPosition = position;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;     
}

laBarGraphTickPosition laBarGraphWidget_GetCategoryAxisTicksPosition(laBarGraphWidget* graph)

{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->categAxisTicksPosition;
}

laResult laBarGraphWidget_SetCategoryAxisTicksPosition(laBarGraphWidget* graph, laBarGraphTickPosition position)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->categAxisTicksPosition == position)
        return LA_SUCCESS;
        
    graph->categAxisTicksPosition = position;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;      
}




#endif // LA_BAR_GRAPH_WIDGET_ENABLED
