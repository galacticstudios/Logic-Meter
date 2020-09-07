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

#define DEFAULT_POINT_SIZE      10

void _laLineGraphWidget_Constructor(laLineGraphWidget* graph)
{
    _laWidget_Constructor((laWidget*)graph);

    graph->widget.type = LA_WIDGET_ARC;
    
    graph->widget.destructor = (laWidget_Destructor_FnPtr)&_laLineGraphWidget_Destructor;

    // override base class methods
    graph->widget.paint = (laWidget_Paint_FnPtr)&_laLineGraphWidget_Paint;

    graph->widget.rect.width = DEFAULT_WIDTH;
    graph->widget.rect.height = DEFAULT_HEIGHT;

    graph->widget.borderType = LA_WIDGET_BORDER_NONE;
    graph->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;
    
    graph->tickLength = DEFAULT_TICK_LENGTH;
    graph->fillGraphArea = LA_TRUE;
    graph->fillValueArea = LA_TRUE;
    graph->stacked = LA_FALSE;
    
    graph->maxValue = DEFAULT_MAX_VALUE;
    graph->minValue = DEFAULT_MIN_VALUE;
    graph->tickInterval = DEFAULT_TICK_INTERVAL;
    graph->subtickInterval = DEFAULT_SUBTICK_INTERVAL;
    
    graph->valueGridlinesVisible = LA_TRUE;
    graph->valueAxisLabelsVisible = LA_TRUE;
    graph->valueAxisTicksVisible = LA_TRUE;
    graph->valueAxisSubticksVisible = LA_TRUE;
    graph->valueAxisTicksPosition = LINE_GRAPH_TICK_CENTER;
    graph->valueAxisSubticksPosition = LINE_GRAPH_TICK_CENTER;
    
    graph->categAxisLabelsVisible = LA_TRUE;
    graph->categAxisTicksVisible = LA_TRUE;
    graph->categAxisTicksPosition = LINE_GRAPH_TICK_CENTER;
    
    laArray_Create(&graph->dataSeries);
    laArray_Create(&graph->categories);
}

void _laLineGraphWidget_Destructor(laLineGraphWidget* graph)
{
    laLineGraphWidget_DestroyAll(graph);
    
    _laWidget_Destructor((laWidget*)graph);
}

laLineGraphWidget* laLineGraphWidget_New()
{
    laLineGraphWidget* graph = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    graph = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laLineGraphWidget));

    if(graph == NULL)
        return NULL;
    
    _laLineGraphWidget_Constructor(graph);

    return graph;
}

GFX_Rect _laLineGraphWidget_GetValueDamagedRect(laLineGraphWidget* graph,
                                      uint32_t seriesID,
                                      uint32_t categoryID)
{
    
    GFX_Rect damagedRect;
    GFX_Rect graphRect;
    
    _laLineGraphWidget_GetGraphRect(graph, &graphRect);
    
    if (graph->stacked == LA_TRUE)
    {
        GFX_Rect widgetRect;
    
        widgetRect = laUtils_WidgetLayerRect((laWidget *) graph);
        
        damagedRect.height = widgetRect.height;
        damagedRect.y = widgetRect.y;
    
        if (graph->fillValueArea == LA_TRUE &&
            graph->categories.size > 1)
        {
            damagedRect.x = graphRect.x + categoryID * graphRect.width / (graph->categories.size - 1);
            damagedRect.x -= graphRect.width / (graph->categories.size - 1);
            damagedRect.width = 2 * graphRect.width / (graph->categories.size - 1);
        }
        else
        {
            damagedRect.x = graphRect.x + (categoryID + 1) * graphRect.width / (graph->categories.size + 1);
            damagedRect.x -= graphRect.width / (graph->categories.size + 1);
            damagedRect.width = 2 * graphRect.width / (graph->categories.size + 1);
        }
    }
    else
    {
        //Get the rectangle for the point and adjacent point/s and combine them
        GFX_Point valuePoint;
        GFX_Point originPoint;
        GFX_Rect valueRect;
        laLineGraphDataSeries * series;
        
        originPoint = _laLineGraphWidget_GetOriginPoint(graph);
        
        valuePoint = _laLineGraphWidget_GetValuePoint(graph, seriesID, categoryID, originPoint);
        series = laArray_Get(&graph->dataSeries, seriesID);
        
        valueRect.x = valuePoint.x - (series->pointSize/2 + 2);
        valueRect.y = valuePoint.y - (series->pointSize/2 + 2);
        valueRect.height = valueRect.width = series->pointSize + 4;
        
        damagedRect = valueRect;
        
        if (series->drawLines == LA_TRUE || graph->fillValueArea == LA_TRUE)
        {
            //Get point from previous category
            if (categoryID > 0)
            {
                valuePoint = _laLineGraphWidget_GetValuePoint(graph, seriesID, categoryID - 1, originPoint);
                valueRect.x = valuePoint.x - (series->pointSize/2 + 2);
                valueRect.y = valuePoint.y - (series->pointSize/2 + 2);
                valueRect.height = valueRect.width = series->pointSize + 4;
                    
                damagedRect = GFX_RectCombine(&damagedRect, &valueRect);
            }
            
            //Get point from next category
            if (categoryID + 1 < graph->categories.size)
            {
                valuePoint = _laLineGraphWidget_GetValuePoint(graph, seriesID, categoryID + 1, originPoint);
                valueRect.x = valuePoint.x - series->pointSize/2 + 2;
                valueRect.y = valuePoint.y - series->pointSize/2 + 2;
                valueRect.height = valueRect.width = series->pointSize + 4;

                damagedRect = GFX_RectCombine(&damagedRect, &valueRect);
            }
        }
    }

    return damagedRect;
}

uint32_t laLineGraphWidget_GetTickLength(laLineGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->tickLength;
}

laResult laLineGraphWidget_SetTickLength(laLineGraphWidget* graph, uint32_t length)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->tickLength == length)
        return LA_SUCCESS;
        
    graph->tickLength = length;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laLineGraphWidget_GetStacked(laLineGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->stacked;
}

laResult laLineGraphWidget_SetStacked(laLineGraphWidget* graph, laBool stacked)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->stacked == stacked)
        return LA_SUCCESS;
        
    graph->stacked = stacked;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

uint32_t laLineGraphWidget_GetMaxValue(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->maxValue;
}

laResult laLineGraphWidget_SetMaxValue(laLineGraphWidget* graph, laLineGraphValueAxis axis, int32_t value)
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

uint32_t laLineGraphWidget_GetMinValue(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->minValue;
}

laResult laLineGraphWidget_SetMinValue(laLineGraphWidget* graph, laLineGraphValueAxis axis, int32_t value)
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

laBool laLineGraphWidget_GetValueAxisLabelsVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisLabelsVisible;
}

laResult laLineGraphWidget_SetValueAxisLabelsVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisLabelsVisible == visible)
        return LA_SUCCESS;
        
    graph->valueAxisLabelsVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laLineGraphWidget_GetFillGraphArea(laLineGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->fillGraphArea;
}

laResult laLineGraphWidget_SetFillGraphArea(laLineGraphWidget* graph, laBool fill)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->fillGraphArea == fill)
        return LA_SUCCESS;
        
    graph->fillGraphArea = fill;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laLineGraphWidget_GetFillSeriesArea(laLineGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->fillValueArea;
}

laResult laLineGraphWidget_SetFillSeriesArea(laLineGraphWidget* graph, laBool fill)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->fillValueArea == fill)
        return LA_SUCCESS;
        
    graph->fillValueArea = fill;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}


laBool laLineGraphWidget_GetGridlinesVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueGridlinesVisible;
}

laResult laLineGraphWidget_SetGridlinesVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueGridlinesVisible == visible)
        return LA_SUCCESS;
        
    graph->valueGridlinesVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laLineGraphWidget_GetValueAxisTicksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisTicksVisible;
}

laResult laLineGraphWidget_SetValueAxisTicksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisTicksVisible == visible)
        return LA_SUCCESS;
        
    graph->valueAxisTicksVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

uint32_t laLineGraphWidget_GetValueAxisTickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->tickInterval;
}

laResult laLineGraphWidget_SetValueAxisTickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis, uint32_t interval)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->tickInterval == interval)
        return LA_SUCCESS;
        
    graph->tickInterval = interval;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

uint32_t laLineGraphWidget_GetValueAxisSubtickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->subtickInterval;
}

laResult laLineGraphWidget_SetValueAxisSubtickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis, uint32_t interval)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->subtickInterval == interval)
        return LA_SUCCESS;
        
    graph->subtickInterval = interval;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laLineGraphWidget_GetValueAxisSubticksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisSubticksVisible;
}

laResult laLineGraphWidget_SetValueAxisSubticksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisSubticksVisible == visible)
        return LA_SUCCESS;
        
    graph->valueAxisSubticksVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laLineGraphWidget_GetCategoryAxisTicksVisible(laLineGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->categAxisTicksVisible;
}

laResult laLineGraphWidget_SetCategoryAxisTicksVisible(laLineGraphWidget* graph, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->categAxisTicksVisible == visible)
        return LA_SUCCESS;
        
    graph->categAxisTicksVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laResult laLineGraphWidget_AddCategory(laLineGraphWidget* graph, uint32_t * id)
{
    laLineGraphCategory * category;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    category = laContext_GetActive()->memIntf.heap.malloc(sizeof(laLineGraphCategory));
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

laResult laLineGraphWidget_GetCategoryText(laLineGraphWidget* graph, uint32_t categoryID, laString * str)
{
    laLineGraphCategory * category;
    
    if(graph == NULL)
        return LA_FAILURE;
    
    if (categoryID >= graph->categories.size)
        return LA_FAILURE;
    
    category = laArray_Get(&graph->categories, categoryID);
    if (category == NULL)
        return LA_FAILURE;
    
    return laString_Copy(str, &category->text);
}

laResult laLineGraphWidget_SetCategoryText(laLineGraphWidget* graph, int32_t categoryID, laString str)
{
    laLineGraphCategory * category;
    
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

laResult laLineGraphWidget_AddSeries(laLineGraphWidget* graph, uint32_t * seriesID)
{
    laLineGraphDataSeries * series;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    series = laContext_GetActive()->memIntf.heap.malloc(sizeof(laLineGraphDataSeries));
    if (series == NULL)
        return LA_FAILURE;

    laArray_Create(&series->data);
    series->axis = LINE_GRAPH_AXIS_0;
    series->scheme = graph->widget.scheme;
    series->drawLines = LA_TRUE;
    series->pointType = LINE_GRAPH_DATA_POINT_CIRCLE;
    series->fillPoints = LA_TRUE;
    series->pointSize = DEFAULT_POINT_SIZE;
    
    laArray_PushBack(&graph->dataSeries, series);
    
    if (seriesID != NULL)
    {
        *seriesID = graph->dataSeries.size;
    }
    
    laWidget_Invalidate((laWidget*)graph);
    
    return LA_SUCCESS;
}

laResult laLineGraphWidget_AddDataToSeries(laLineGraphWidget* graph, uint32_t seriesID, int32_t value, uint32_t * index)
{
    laLineGraphDataSeries * series;
    int32_t * data;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    if (seriesID >= graph->dataSeries.size)
        return LA_FAILURE;
    
    data = laContext_GetActive()->memIntf.heap.malloc(sizeof(data));
    
    *data = value;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    
    laArray_PushBack(&series->data, data);

    if (index != NULL)
    {
       *index = series->data.size - 1;
    }
    
    laWidget_Invalidate((laWidget*)graph);
    
    return LA_SUCCESS;
}

GFX_Rect prevDamagedRect, damagedRect;

laResult laLineGraphWidget_SetDataInSeries(laLineGraphWidget* graph, 
                                          uint32_t seriesID,
                                          uint32_t index,
                                          int32_t value)
{

    laLineGraphDataSeries * series;
    int32_t * data;
    
    if (graph == NULL)
        return LA_FAILURE;
    
    if (seriesID >= graph->dataSeries.size)
        return LA_FAILURE;
    
    series = laArray_Get(&graph->dataSeries, seriesID);

    if (index >= series->data.size)
        return LA_FAILURE;
    
    data = laArray_Get(&series->data, index);
    
    prevDamagedRect = _laLineGraphWidget_GetValueDamagedRect(graph,
                                     seriesID,
                                     index);
    
    *data = value;
    
    damagedRect = _laLineGraphWidget_GetValueDamagedRect(graph,
                                     seriesID,
                                     index);
    
    damagedRect = GFX_RectCombine(&prevDamagedRect, &damagedRect);

    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)graph),
                          &damagedRect,
                          LA_FALSE);
    
//    laWidget_Invalidate((laWidget*)graph);
    
    return LA_SUCCESS;
}

laScheme * laLineGraphWidget_GetSeriesScheme(laLineGraphWidget* graph, uint32_t seriesID)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return NULL;
    
    if (seriesID >= graph->dataSeries.size)
        return NULL;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return NULL;
    
    return series->scheme;
}

laResult laLineGraphWidget_SetSeriesScheme(laLineGraphWidget* graph, int32_t seriesID, laScheme * scheme)
{
    laLineGraphDataSeries * series;
    
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

laBool laLineGraphWidget_GetSeriesFillPoints(laLineGraphWidget* graph, uint32_t seriesID)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return LA_FALSE;
    
    if (seriesID >= graph->dataSeries.size)
        return LA_FALSE;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FALSE;
    
    return series->fillPoints;
}

laResult laLineGraphWidget_SetSeriesFillPoints(laLineGraphWidget* graph, int32_t seriesID, laBool fill)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return LA_FAILURE;
    
    if (seriesID >= (int32_t) graph->dataSeries.size)
        return LA_FAILURE;
    
    if (seriesID < 0)
        seriesID = graph->dataSeries.size - 1;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FAILURE;
    
    series->fillPoints = fill;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laBool laLineGraphWidget_GetSeriesLinesVisible(laLineGraphWidget* graph, uint32_t seriesID)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return LA_FALSE;
    
    if (seriesID >= graph->dataSeries.size)
        return LA_FALSE;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FALSE;
    
    return series->drawLines;
}

laResult laLineGraphWidget_SetSeriesLinesVisible(laLineGraphWidget* graph, int32_t seriesID, laBool visible)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return LA_FAILURE;
    
    if (seriesID >= (int32_t) graph->dataSeries.size)
        return LA_FAILURE;
    
    if (seriesID < 0)
        seriesID = graph->dataSeries.size - 1;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FAILURE;
    
    series->drawLines = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laLineGraphDataPointType laLineGraphWidget_GetSeriesPointType(laLineGraphWidget* graph, uint32_t seriesID)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return LINE_GRAPH_DATA_POINT_NONE;
    
    if (seriesID >= graph->dataSeries.size)
        return LINE_GRAPH_DATA_POINT_NONE;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LINE_GRAPH_DATA_POINT_NONE;
    
    return series->pointType;
}

laResult laLineGraphWidget_SetSeriesPointType(laLineGraphWidget* graph, int32_t seriesID, laLineGraphDataPointType type)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return LA_FAILURE;
    
    if (seriesID >= (int32_t) graph->dataSeries.size)
        return LA_FAILURE;
    
    if (seriesID < 0)
        seriesID = graph->dataSeries.size - 1;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FAILURE;
    
    series->pointType = type;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

uint32_t laLineGraphWidget_GetSeriesPointSize(laLineGraphWidget* graph, uint32_t seriesID)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return 0;
    
    if (seriesID >= graph->dataSeries.size)
        return 0;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return 0;
    
    return series->pointSize;
}

laResult laLineGraphWidget_SetSeriesPointSize(laLineGraphWidget* graph, int32_t seriesID, uint32_t size)
{
    laLineGraphDataSeries * series;
    
    if(graph == NULL)
        return LA_FAILURE;
    
    if (seriesID >= (int32_t) graph->dataSeries.size)
        return LA_FAILURE;
    
    if (seriesID < 0)
        seriesID = graph->dataSeries.size - 1;
    
    series = laArray_Get(&graph->dataSeries, seriesID);
    if (series == NULL)
        return LA_FAILURE;
    
    series->pointSize = size;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laResult laLineGraphWidget_DestroyAll(laLineGraphWidget* graph)
{
    laLineGraphCategory * category;
    laLineGraphDataSeries * series;
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

laResult laLineGraphWidget_SetStringTable(laLineGraphWidget* graph, 
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

laResult laLineGraphWidget_SetTicksLabelsStringID(laLineGraphWidget* graph, 
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

laBool laLineGraphWidget_GetCategoryAxisLabelsVisible(laLineGraphWidget* graph)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->categAxisLabelsVisible;
}

laResult laLineGraphWidget_SetCategoryAxisLabelsVisible(laLineGraphWidget* graph, laBool visible)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->categAxisLabelsVisible == visible)
        return LA_SUCCESS;
        
    graph->categAxisLabelsVisible = visible;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;
}

laLineGraphTickPosition laLineGraphWidget_GetValueAxisTicksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisTicksPosition;
}
laResult laLineGraphWidget_SetValueAxisTicksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis, laLineGraphTickPosition position)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisTicksPosition == position)
        return LA_SUCCESS;
        
    graph->valueAxisTicksPosition = position;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS; 
}

laLineGraphTickPosition laLineGraphWidget_GetValueAxisSubticksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->valueAxisSubticksPosition;
}

laResult laLineGraphWidget_SetValueAxisSubticksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis, laLineGraphTickPosition position)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->valueAxisSubticksPosition == position)
        return LA_SUCCESS;
        
    graph->valueAxisSubticksPosition = position;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;     
}

laLineGraphTickPosition laLineGraphWidget_GetCategoryAxisTicksPosition(laLineGraphWidget* graph)

{
    if(graph == NULL)
        return LA_FAILURE;
        
    return graph->categAxisTicksPosition;
}

laResult laLineGraphWidget_SetCategoryAxisTicksPosition(laLineGraphWidget* graph, laLineGraphTickPosition position)
{
    if(graph == NULL)
        return LA_FAILURE;
        
    if(graph->categAxisTicksPosition == position)
        return LA_SUCCESS;
        
    graph->categAxisTicksPosition = position;
    
    laWidget_Invalidate((laWidget*)graph);
        
    return LA_SUCCESS;      
}

GFX_Point _laLineGraphWidget_GetOriginPoint(laLineGraphWidget* graph)
{
    GFX_Point originPoint = {0};
    GFX_Rect graphRect = {0};
    float pixelsPerUnit;
    
    _laLineGraphWidget_GetGraphRect(graph, &graphRect);
    
    pixelsPerUnit = (float) graphRect.height / ((float) graph->maxValue - (float) graph->minValue);
    
    originPoint.x = graphRect.x;
    if (graph->minValue < 0 && graph->maxValue > 0)
    {
        originPoint.y = graphRect.y + (int32_t) ((float) graph->maxValue * pixelsPerUnit);
    }
    else if (graph->minValue >= 0)
    {
        originPoint.y = graphRect.y + graphRect.height;
    }
    else if (graph->maxValue <= 0)
    {
        originPoint.y = graphRect.y;
    }
    
    return originPoint;
}

GFX_Point _laLineGraphWidget_GetValuePoint(laLineGraphWidget* graph,
                                     uint32_t seriesID,
                                     uint32_t categoryIndex,
                                     GFX_Point originPoint)
{
    GFX_Point valuePoint = {0};
    GFX_Rect graphRect = {0};
    int32_t value;
    int32_t * valuePtr = NULL;
    laLineGraphDataSeries * series;
    int32_t originValue = 0;
    float pixelsPerUnit;
    
    _laLineGraphWidget_GetGraphRect(graph, &graphRect);
    
    pixelsPerUnit = (float) graphRect.height / ((float) graph->maxValue - (float) graph->minValue);    
    
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
    
    value = originValue;
    
    if (graph->stacked == LA_TRUE)
    {
        unsigned int i = 0;
        int32_t stackedPosValue = 0, stackedNegValue = 0;
        for (i = 0; i < seriesID; i++)
        {
            series = laArray_Get(&graph->dataSeries, seriesID);
            if (series == NULL)
                break;
            
            valuePtr = laArray_Get(&series->data, categoryIndex);
            if (valuePtr != NULL)
            {
                if (*valuePtr > 0)
                {
                    stackedPosValue += *valuePtr;
                    value = stackedPosValue;
                }
                else
                {
                    stackedNegValue += *valuePtr;
                    value = stackedNegValue;
                }
            }
        }
    }
    else
    {
        series = laArray_Get(&graph->dataSeries, seriesID);
        if (series == NULL)
            return originPoint;
            
        valuePtr = laArray_Get(&series->data, categoryIndex);
        if (valuePtr != NULL)
            value = *valuePtr;
    }
    
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
    
    if (value >= originValue)
    {
        valuePoint.y = originPoint.y - (int32_t) ((float)(value - originValue) * pixelsPerUnit);
    }
    //draw the points below the origin value
    else if (value < originValue)
    {
        valuePoint.y = originPoint.y + (int32_t) ((float)(originValue - value) * pixelsPerUnit);
    }
    
    return valuePoint;
}




#endif // LA_LINE_GRAPH_WIDGET_ENABLED
