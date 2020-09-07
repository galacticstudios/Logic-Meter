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

/*******************************************************************************
 Module for Microchip Graphics Library - Aria User Interface Library

  Company:
    Microchip Technology Inc.

  File Name:
    libaria_widget_line_graph.h

  Summary:


  Description:
    This module implements line graph drawing widget functions.
*******************************************************************************/


#ifndef LIBARIA_WIDGET_LINE_GRAPH_H
#define LIBARIA_WIDGET_LINE_GRAPH_H

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_string.h"

#if LA_LINE_GRAPH_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laLineGraphValueAxis_t

  Summary:
    The value axis index value
*/
typedef enum laLineGraphValueAxis_t
{
    LINE_GRAPH_AXIS_0 = 0,
//  LINE_GRAPH_AXIS_1, //unsupported
} laLineGraphValueAxis;

// *****************************************************************************
/* Enumeration:
    laLineGraphTickPosition_t

  Summary:
    The tick position relative to axis
*/
typedef enum laLineGraphTickPosition_t
{
    LINE_GRAPH_TICK_IN,
    LINE_GRAPH_TICK_OUT,
    LINE_GRAPH_TICK_CENTER,
} laLineGraphTickPosition;

// *****************************************************************************
/* Enumeration:
    laLineGraphDataPointType_t

  Summary:
    The graph data point type
*/
typedef enum laLineGraphDataPointType_t
{
    LINE_GRAPH_DATA_POINT_NONE,
    LINE_GRAPH_DATA_POINT_CIRCLE,
    LINE_GRAPH_DATA_POINT_SQUARE,
} laLineGraphDataPointType;

// *****************************************************************************
/* Structure:
    laLineGraphDataSeries_t

  Summary:
    The data series object that contains the series properties and data

  Description:


  Remarks:
    None.
*/
typedef struct laLineGraphDataSeries_t
{
    laScheme * scheme;
    laArray data;
    laLineGraphValueAxis axis;
    laLineGraphDataPointType pointType;
    uint32_t pointSize;
    laBool fillPoints;
    laBool drawLines;
} laLineGraphDataSeries;

// *****************************************************************************
/* Structure:
    laLineGraphCategory_t

  Summary:
    Contains the Category properties

  Description:


  Remarks:
    None.
*/
typedef struct laLineGraphCategory_t
{
    laString text;
    int32_t stackValue;
} laLineGraphCategory;

// *****************************************************************************
/* Structure:
    laLineGraphWidget_t

  Summary:
    Implementation of a line graph widget.

  Description:
    A line graph widget draws a line graph. All coordinates are expressed in local widget space.

    The color of the graph is determined by the widget scheme's 'foreground'
    color.

  Remarks:
    None.
*/
typedef struct laLineGraphWidget_t
{
    laWidget widget; // base widget header

    uint32_t tickLength;
    laBool fillGraphArea;
    laBool fillValueArea;

    //Value axis properties
    int32_t maxValue;
    int32_t minValue;
    uint32_t tickInterval;
    uint32_t subtickInterval;
    laBool valueAxisLabelsVisible;
    laBool valueAxisTicksVisible;
    laBool valueAxisSubticksVisible;
    laBool valueGridlinesVisible;
    laBool stacked;
    laArray dataSeries;
    GFXU_StringTableAsset * stringTable; //string table
    uint32_t ticksLabelsStringID; //ID of Superset string containing numbers
    laLineGraphTickPosition valueAxisTicksPosition;
    laLineGraphTickPosition valueAxisSubticksPosition;

    //Category axis properties
    laBool categAxisLabelsVisible;
    laBool categAxisTicksVisible;
    laLineGraphTickPosition categAxisTicksPosition;
    laArray categories;

    GFXU_ExternalAssetReader* reader; // asset reader
} laLineGraphWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************
// DOM-IGNORE-BEGIN
// internal use only
void _laLineGraphWidget_Constructor(laLineGraphWidget* graph);
void _laLineGraphWidget_Destructor(laLineGraphWidget* graph);

void _laLineGraphWidget_Paint(laLineGraphWidget* graph);
// DOM-IGNORE-END

// *****************************************************************************
/* Function:
    laLineGraphWidget* laLineGraphWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the management of this memory until the
    widget is added to a widget tree.

  Description:

  Parameters:
    void

  Returns:
    laLineGraphWidget*

  Remarks:

*/
LIB_EXPORT laLineGraphWidget* laLineGraphWidget_New();

// *****************************************************************************
/* Function:
    uint32_t laLineGraphWidget_GetTickLength(laLineGraphWidget* graph)

  Summary:
    Returns the length of the ticks

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    uint32_t - tick length

  Remarks:

*/
LIB_EXPORT uint32_t laLineGraphWidget_GetTickLength(laLineGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetTickLength(laLineGraphWidget* graph, uint32_t length)

  Summary:
    Sets the length of the ticks

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t length - length in pixels

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetTickLength(laLineGraphWidget* graph, uint32_t length);

// *****************************************************************************
/* Function:
    uint32_t laLineGraphWidget_GetMaxValue(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns the max value of the axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    uint32_t - max value

  Remarks:

*/
LIB_EXPORT uint32_t laLineGraphWidget_GetMaxValue(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetMaxValue(laLineGraphWidget* graph, laLineGraphValueAxis axis, int32_t value)

  Summary:
    Sets the max value of the axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    int32_t value - max value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetMaxValue(laLineGraphWidget* graph, laLineGraphValueAxis axis, int32_t value);

// *****************************************************************************
/* Function:
    uint32_t laLineGraphWidget_GetMinValue(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns the min value of the axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    uint32_t - min value

  Remarks:

*/
LIB_EXPORT uint32_t laLineGraphWidget_GetMinValue(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetMinValue(laLineGraphWidget* graph, laLineGraphValueAxis axis, int32_t value)

  Summary:
    Sets the min value of the axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    int32_t value - min value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetMinValue(laLineGraphWidget* graph, laLineGraphValueAxis axis, int32_t value);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetValueAxisLabelsVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns GFX_TRUE if the value axis labels are visible

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the value axis labels are visible

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetValueAxisLabelsVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetValueAxisLabelsVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible)

  Summary:
    Shows/Hides the labels in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    laBool visible - shows the labels if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetValueAxisLabelsVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetValueAxisTicksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns GFX_TRUE if the value axis ticks are visible

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the value axis ticks are visible

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetValueAxisTicksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetValueAxisTicksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible)

  Summary:
    Shows/Hides the ticks in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    laBool visible - shows the ticks if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetValueAxisTicksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetValueAxisSubticksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns GFX_TRUE if the value axis subticks are visible

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the value axis subticks are visible

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetValueAxisSubticksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetValueAxisSubticksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible)

  Summary:
    Shows/Hides the subticks in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    laBool visible - shows the subticks if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetValueAxisSubticksVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible);

// *****************************************************************************
/* Function:
    uint32_t laLineGraphWidget_GetValueAxisTickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns the interval between major ticks in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index

  Returns:
    uint32_t - ticks in pixels

  Remarks:

*/
LIB_EXPORT uint32_t laLineGraphWidget_GetValueAxisTickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetValueAxisTickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis, uint32_t interval)

  Summary:
    Sets the tick interval in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    uint32_t interval - tick interval in pixels

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetValueAxisTickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis, uint32_t interval);

// *****************************************************************************
/* Function:
    uint32_t laLineGraphWidget_GetValueAxisSubtickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns the interval between minor ticks in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index

  Returns:
    uint32_t - ticks in pixels

  Remarks:

*/
LIB_EXPORT uint32_t laLineGraphWidget_GetValueAxisSubtickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetValueAxisSubtickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis, uint32_t interval)

  Summary:
    Sets the minor tick interval in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    uint32_t interval - tick interval in pixels

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetValueAxisSubtickInterval(laLineGraphWidget* graph, laLineGraphValueAxis axis, uint32_t interval);



// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_AddSeries(laLineGraphWidget* graph, uint32_t * seriesID)

  Summary:
    Adds a series to the graph

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t * seriesID - destination of the returned series ID

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_AddSeries(laLineGraphWidget* graph, uint32_t * seriesID);

// *****************************************************************************
/* Function:
    laScheme * laLineGraphWidget_GetSeriesScheme(laLineGraphWidget* graph, uint32_t seriesID)

  Summary:
    Returns scheme of the specified series

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laScheme * - scheme of the specified series

  Remarks:

*/
LIB_EXPORT laScheme * laLineGraphWidget_GetSeriesScheme(laLineGraphWidget* graph, uint32_t seriesID);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetSeriesScheme(laLineGraphWidget* graph, uint32_t seriesID, laScheme * scheme)

  Summary:
    Sets the color scheme of the series

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    int32_t seriesID - the series ID, if negative the last series is referenced
    laScheme * scheme - the color scheme

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetSeriesScheme(laLineGraphWidget* graph, int32_t seriesID, laScheme * scheme);

// *****************************************************************************
/* Function:
   laBool laLineGraphWidget_GetSeriesFillPoints(laLineGraphWidget* graph, uint32_t seriesID)

  Summary:
    Returns GFX_TRUE if the series points are filled

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the series points are filled

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetSeriesFillPoints(laLineGraphWidget* graph, uint32_t seriesID);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetSeriesFillPoints(laLineGraphWidget* graph, int32_t seriesID, laBool fill)

  Summary:
    Sets the series points filled

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    int32_t seriesID - the series ID, if negative the last series is referenced
    laBool fill - fills the points if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetSeriesFillPoints(laLineGraphWidget* graph, int32_t seriesID, laBool fill);

// *****************************************************************************
/* Function:
   laBool laLineGraphWidget_GetSeriesLinesVisible(laLineGraphWidget* graph, uint32_t seriesID)

  Summary:
    Returns GFX_TRUE if the series lines are visible

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the series lines are visible

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetSeriesLinesVisible(laLineGraphWidget* graph, uint32_t seriesID);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetSeriesLinesVisible(laLineGraphWidget* graph, int32_t seriesID, laBool visible)

  Summary:
    Shows/hides the lines between series points

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    int32_t seriesID - the series ID, if negative the last series is referenced
    laBool fill - Shows the lines between series data points if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetSeriesLinesVisible(laLineGraphWidget* graph, int32_t seriesID, laBool visible);

// *****************************************************************************
/* Function:
   laLineGraphDataPointType laLineGraphWidget_GetSeriesPointType(laLineGraphWidget* graph, uint32_t seriesID)

  Summary:
    Returns the type of point drawn for the series data points

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t seriesID - the series ID,

  Returns:
    laLineGraphDataPointType - the point type

  Remarks:

*/
LIB_EXPORT laLineGraphDataPointType laLineGraphWidget_GetSeriesPointType(laLineGraphWidget* graph, uint32_t seriesID);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetSeriesPointType(laLineGraphWidget* graph, int32_t seriesID, laLineGraphDataPointType type)

  Summary:
    Sets the type of point drawn for the series data points

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    int32_t seriesID - the series ID, if negative the last series is referenced
    laLineGraphDataPointType type - point type

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetSeriesPointType(laLineGraphWidget* graph, int32_t seriesID, laLineGraphDataPointType type);

// *****************************************************************************
/* Function:
    uint32_t laLineGraphWidget_GetSeriesPointSize(laLineGraphWidget* graph, uint32_t seriesID)

  Summary:
    Returns the size of the drawn point

  Description:
    For circular points, this value is the radius
    For square points, the length of each side is twice the value

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t seriesID - the series ID

  Returns:
    uint32_t - the point size

  Remarks:

*/
LIB_EXPORT uint32_t laLineGraphWidget_GetSeriesPointSize(laLineGraphWidget* graph, uint32_t seriesID);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetSeriesPointSize(laLineGraphWidget* graph, int32_t seriesID, uint32_t size)

  Summary:
    Sets the size of the point drawn for the series data

  Description:
    For circular points, this value sets the radius
    For square points, the length of each side is twice the value

  Parameters:
    laLineGraphWidget* graph - the widget
    int32_t seriesID - the series ID, if negative the last series is referenced
    uint32_t size - size in pixels

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetSeriesPointSize(laLineGraphWidget* graph, int32_t seriesID, uint32_t size);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_AddDataToSeries(laLineGraphWidget* graph, uint32_t seriesID, uint32_t categoryID, int32_t value)

  Summary:
    Adds a data (value) to the specified series at categoryID index

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t seriesID - the series ID
    int32_t value - the value
    uint32_t * index - the destination to return the index of the added data

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_AddDataToSeries(laLineGraphWidget* graph, uint32_t seriesID, int32_t value, uint32_t * index);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetDataInSeries(laLineGraphWidget* graph,
                                              uint32_t seriesID,
                                              uint32_t index,
                                              int32_t value);
  Summary:
    Sets the value of the entry in the series index. The entry should have
    been previously

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t seriesID - the series ID
    uint32_t index - the index of the data
    int32_t value - the value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetDataInSeries(laLineGraphWidget* graph,
                                          uint32_t seriesID,
                                          uint32_t index,
                                          int32_t value);


// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_DestroyAll(laLineGraphWidget* graph)

  Summary:
    Destroys data, series and categories and frees the memory allocated

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_DestroyAll(laLineGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetStringTable(laLineGraphWidget* graph, GFXU_StringTableAsset * stringTable)

  Summary:
    Sets the string table used for the generated axis labels

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    GFXU_StringTableAsset * stringTable - the string table

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetStringTable(laLineGraphWidget* graph, GFXU_StringTableAsset * stringTable);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetTicksLabelsStringID(laLineGraphWidget* graph, uint32_t stringID)

  Summary:
    Sets the ID of the superset string used for the value axis tick labels

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t stringID - the string ID

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetTicksLabelsStringID(laLineGraphWidget* graph, uint32_t stringID);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetGridlinesVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns GFX_TRUE if the axis gridlines are visible

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index

  Returns:
    laBool - GFX_TRUE if the axis gridlines are visible

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetGridlinesVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetGridlinesVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible)

  Summary:
    Shows/Hides the gridlines

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - category ID
    laBool visible - shows the gridlines if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetGridlinesVisible(laLineGraphWidget* graph, laLineGraphValueAxis axis, laBool visible);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_AddCategory(laLineGraphWidget* graph, uint32_t * id)

  Summary:
    Adds a category to the graph

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t * id - destination of the ID of the new category

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_AddCategory(laLineGraphWidget* graph, uint32_t * id);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_GetCategoryText(laLineGraphWidget* graph, uint32_t categoryID, laString * str)

  Summary:
    Gets a copy of the string used to label the category

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    uint32_t categoryID - category ID
    laString * str - destination of the copied string

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_GetCategoryText(laLineGraphWidget* graph, uint32_t categoryID, laString * str);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetCategoryText(laLineGraphWidget* graph, uint32_t categoryID, laString str)

  Summary:
    Sets the string used to label the category

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    int32_t categoryID - category ID, if -1 the last category is assigned
    laString str - the string to use

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetCategoryText(laLineGraphWidget* graph, int32_t categoryID, laString str);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetStacked(laLineGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the bars are stacked

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetStacked(laLineGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetStacked(laLineGraphWidget* graph, laBool stacked)

  Summary:
    Stacks the line graph

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laBool stacked - if GFX_TRUE, the bars are stacked

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetStacked(laLineGraphWidget* graph, laBool stacked);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetCategoryAxisTicksVisible(laLineGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the category axis ticks are visible

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the category axis ticks are visible

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetCategoryAxisTicksVisible(laLineGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetCategoryAxisTicksVisible(laLineGraphWidget* graph, laBool visible)

  Summary:
    Shows/Hides the category axis ticks

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laBool visible - if GFX_TRUE, the axis ticks are shown

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetCategoryAxisTicksVisible(laLineGraphWidget* graph, laBool visible);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetCategoryAxisLabelsVisible(laLineGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the category axis labels are visible

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the category axis labels are visible

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetCategoryAxisLabelsVisible(laLineGraphWidget* graph);

// *****************************************************************************
/* Function:
    LIB_EXPORT laResult laLineGraphWidget_SetCategoryAxisLabelsVisible(laLineGraphWidget* graph, laBool visible)

  Summary:
    Shows/Hides the category axis labels

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laBool visible - if GFX_TRUE, the axis labels are shown

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetCategoryAxisLabelsVisible(laLineGraphWidget* graph, laBool visible);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetFillGraphArea(laLineGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the graph area is filled

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the graph area is filled

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetFillGraphArea(laLineGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetFillGraphArea(laLineGraphWidget* graph, laBool fill)

  Summary:
    Sets the graph area filled or not

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laBool fill - if GFX_TRUE, fills the graph area

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetFillGraphArea(laLineGraphWidget* graph, laBool fill);

// *****************************************************************************
/* Function:
    laBool laLineGraphWidget_GetFillSeriesArea(laLineGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the series area are filled

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the series area is filled

  Remarks:

*/
LIB_EXPORT laBool laLineGraphWidget_GetFillSeriesArea(laLineGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetFillGraphArea(laLineGraphWidget* graph, laBool fill)

  Summary:
    Sets the series area filled or not

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laBool fill - if GFX_TRUE, fills the series area

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetFillSeriesArea(laLineGraphWidget* graph, laBool fill);

// *****************************************************************************
/* Function:
    laLineGraphTickPosition laLineGraphWidget_GetValueAxisTicksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns the position of the ticks in the axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the index of the value axis

  Returns:
    laLineGraphTickPosition - the tick position

  Remarks:

*/
LIB_EXPORT laLineGraphTickPosition laLineGraphWidget_GetValueAxisTicksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetValueAxisTicksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis, laLineGraphTickPosition position)

  Summary:
    Sets the position of the ticks in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    laLineGraphTickPosition position - the tick position

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetValueAxisTicksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis, laLineGraphTickPosition position);

// *****************************************************************************
/* Function:
    laLineGraphTickPosition laLineGraphWidget_GetValueAxisSubticksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis)

  Summary:
    Returns the position of the subticks in the axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the index of the value axis

  Returns:
    laLineGraphTickPosition - the subtick position

  Remarks:

*/
LIB_EXPORT laLineGraphTickPosition laLineGraphWidget_GetValueAxisSubticksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetValueAxisSubticksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis, laLineGraphTickPosition position)

  Summary:
    Sets the position of the subticks in the value axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphValueAxis axis - the value axis index
    laLineGraphTickPosition position  - position of the subticks

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetValueAxisSubticksPosition(laLineGraphWidget* graph, laLineGraphValueAxis axis, laLineGraphTickPosition position);

// *****************************************************************************
/* Function:
    laLineGraphTickPosition laLineGraphWidget_GetCategoryAxisTicksPosition(laLineGraphWidget* graph)

  Summary:
    Returns the position of the ticks in the category axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget

  Returns:
    laLineGraphTickPosition - position of the ticks in the category axis

  Remarks:

*/
LIB_EXPORT laLineGraphTickPosition laLineGraphWidget_GetCategoryAxisTicksPosition(laLineGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laLineGraphWidget_SetCategoryAxisTicksPosition(laLineGraphWidget* graph, laLineGraphTickPosition position)

  Summary:
    Sets the position of the ticks in the category axis

  Description:

  Parameters:
    laLineGraphWidget* graph - the widget
    laLineGraphTickPosition position  - position of the ticks

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineGraphWidget_SetCategoryAxisTicksPosition(laLineGraphWidget* graph, laLineGraphTickPosition position);

// DOM-IGNORE-BEGIN
// internal use only
void _laLineGraphWidget_GetGraphRect(laLineGraphWidget* graph,
                                           GFX_Rect * graphRect);

GFX_Point _laLineGraphWidget_GetValuePoint(laLineGraphWidget* graph,
                                     uint32_t seriesID,
                                     uint32_t categoryIndex,
                                     GFX_Point originPoint);

GFX_Point _laLineGraphWidget_GetOriginPoint(laLineGraphWidget* graph);
// DOM-IGNORE-END

#endif // LA_LINE_GRAPH_WIDGET_ENABLED
#endif /* LIBARIA_WIDGET_LINE_GRAPH_H */
