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
    libaria_widget_bar_graph.h

  Summary:


  Description:
    This module implements bar graph drawing widget functions.
*******************************************************************************/


#ifndef LIBARIA_WIDGET_BAR_GRAPH_H
#define LIBARIA_WIDGET_BAR_GRAPH_H

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_string.h"

#if LA_BAR_GRAPH_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************
#define MAX_TICK_LABEL_DIGITS 10
#define MAX_TICK_LABEL_VALUE 999999999
#define LABEL_OFFSET_MIN_PIX 5

// *****************************************************************************
/* Enumeration:
    laBarGraphValueAxis_t

  Summary:
    The value axis index value
*/
typedef enum laBarGraphValueAxis_t
{
    BAR_GRAPH_AXIS_0 = 0,
//  BAR_GRAPH_AXIS_1, //unsupported
} laBarGraphValueAxis;

// *****************************************************************************
/* Enumeration:
    laBarGraphTickPosition_t

  Summary:
    The tick position relative to axis
*/
typedef enum laBarGraphTickPosition_t
{
    BAR_GRAPH_TICK_IN,
    BAR_GRAPH_TICK_OUT,
    BAR_GRAPH_TICK_CENTER,
} laBarGraphTickPosition;

// *****************************************************************************
/* Structure:
    laBarGraphDataSeries_t

  Summary:
    The data series object that contains the series properties and data

  Description:


  Remarks:
    None.
*/
typedef struct laBarGraphDataSeries_t
{
    laScheme * scheme;
    laArray data;
    laBarGraphValueAxis axis;
} laBarGraphDataSeries;

// *****************************************************************************
/* Structure:
    laBarGraphCategory_t

  Summary:
    Contains the Category properties

  Description:


  Remarks:
    None.
*/
typedef struct laBarGraphCategory_t
{
    laString text;
} laBarGraphCategory;

// *****************************************************************************
/* Structure:
    laBarGraphWidget_t

  Summary:
    Implementation of a bar graph widget.

  Description:
    A bar graph widget draws a bar graph. All coordinates are expressed in local widget space.

    The color of the graph is determined by the widget scheme's 'foreground'
    color.

  Remarks:
    None.
*/
typedef struct laBarGraphWidget_t
{
    laWidget widget; // base widget header

    uint32_t tickLength;
    laBool fillGraphArea;

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
    laBarGraphTickPosition valueAxisTicksPosition;
    laBarGraphTickPosition valueAxisSubticksPosition;

    //Category axis properties
    laBool categAxisLabelsVisible;
    laBool categAxisTicksVisible;
    laBarGraphTickPosition categAxisTicksPosition;
    laArray categories;

    GFXU_ExternalAssetReader* reader; // asset reader
} laBarGraphWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laBarGraphWidget_Constructor(laBarGraphWidget* graph);
void _laBarGraphWidget_Destructor(laBarGraphWidget* graph);

void _laBarGraphWidget_Paint(laBarGraphWidget* graph);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laBarGraphWidget* laBarGraphWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the management of this memory until the
    widget is added to a widget tree.

  Description:

  Parameters:
    void

  Returns:
    laBarGraphWidget*

  Remarks:

*/
LIB_EXPORT laBarGraphWidget* laBarGraphWidget_New();

// *****************************************************************************
/* Function:
    uint32_t laBarGraphWidget_GetTickLength(laBarGraphWidget* graph)

  Summary:
    Returns the length of the ticks

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    uint32_t - tick length

  Remarks:

*/
LIB_EXPORT uint32_t laBarGraphWidget_GetTickLength(laBarGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetTickLength(laBarGraphWidget* graph, uint32_t length)

  Summary:
    Sets the length of the ticks

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    uint32_t length - length in pixels

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetTickLength(laBarGraphWidget* graph, uint32_t length);

// *****************************************************************************
/* Function:
    uint32_t laBarGraphWidget_GetMaxValue(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns the max value of the axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    uint32_t - max value

  Remarks:

*/
LIB_EXPORT uint32_t laBarGraphWidget_GetMaxValue(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetMaxValue(laBarGraphWidget* graph, laBarGraphValueAxis axis, int32_t value)

  Summary:
    Sets the max value of the axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    int32_t value - max value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetMaxValue(laBarGraphWidget* graph, laBarGraphValueAxis axis, int32_t value);

// *****************************************************************************
/* Function:
    uint32_t laBarGraphWidget_GetMinValue(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns the min value of the axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    uint32_t - min value

  Remarks:

*/
LIB_EXPORT uint32_t laBarGraphWidget_GetMinValue(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetMinValue(laBarGraphWidget* graph, laBarGraphValueAxis axis, int32_t value)

  Summary:
    Sets the min value of the axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    int32_t value - min value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetMinValue(laBarGraphWidget* graph, laBarGraphValueAxis axis, int32_t value);

// *****************************************************************************
/* Function:
    laBool laBarGraphWidget_GetValueAxisLabelsVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns GFX_TRUE if the value axis labels are visible

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the value axis labels are visible

  Remarks:

*/
LIB_EXPORT laBool laBarGraphWidget_GetValueAxisLabelsVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetValueAxisLabelsVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible)

  Summary:
    Shows/Hides the labels in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    laBool visible - shows the labels if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetValueAxisLabelsVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible);

// *****************************************************************************
/* Function:
    laBool laBarGraphWidget_GetValueAxisTicksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns GFX_TRUE if the value axis ticks are visible

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the value axis ticks are visible

  Remarks:

*/
LIB_EXPORT laBool laBarGraphWidget_GetValueAxisTicksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetValueAxisTicksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible)

  Summary:
    Shows/Hides the ticks in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    laBool visible - shows the ticks if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetValueAxisTicksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible);

// *****************************************************************************
/* Function:
    laBool laBarGraphWidget_GetValueAxisSubticksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns GFX_TRUE if the value axis subticks are visible

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the value axis subticks are visible

  Remarks:

*/
LIB_EXPORT laBool laBarGraphWidget_GetValueAxisSubticksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetValueAxisSubticksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible)

  Summary:
    Shows/Hides the subticks in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    laBool visible - shows the subticks if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetValueAxisSubticksVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible);

// *****************************************************************************
/* Function:
    uint32_t laBarGraphWidget_GetValueAxisTickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns the interval between major ticks in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index

  Returns:
    uint32_t - ticks in pixels

  Remarks:

*/
LIB_EXPORT uint32_t laBarGraphWidget_GetValueAxisTickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetValueAxisTickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis, uint32_t interval)

  Summary:
    Sets the tick interval in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    uint32_t interval - tick interval in pixels

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetValueAxisTickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis, uint32_t interval);

// *****************************************************************************
/* Function:
    uint32_t laBarGraphWidget_GetValueAxisSubtickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns the interval between minor ticks in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index

  Returns:
    uint32_t - ticks in pixels

  Remarks:

*/
LIB_EXPORT uint32_t laBarGraphWidget_GetValueAxisSubtickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetValueAxisSubtickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis, uint32_t interval)

  Summary:
    Sets the minor tick interval in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    uint32_t interval - tick interval in pixels

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetValueAxisSubtickInterval(laBarGraphWidget* graph, laBarGraphValueAxis axis, uint32_t interval);



// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_AddSeries(laBarGraphWidget* graph, uint32_t * seriesID)

  Summary:
    Adds a series to the graph

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    uint32_t * seriesID - destination of the returned series ID

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_AddSeries(laBarGraphWidget* graph, uint32_t * seriesID);

// *****************************************************************************
/* Function:
    laScheme * laBarGraphWidget_GetSeriesScheme(laBarGraphWidget* graph, uint32_t seriesID)

  Summary:
    Returns scheme of the specified series

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laScheme * - scheme of the specified series

  Remarks:

*/
LIB_EXPORT laScheme * laBarGraphWidget_GetSeriesScheme(laBarGraphWidget* graph, uint32_t seriesID);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetSeriesScheme(laBarGraphWidget* graph, uint32_t seriesID, laScheme * scheme)

  Summary:
    Sets the color scheme of the series

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    int32_t seriesID - the series ID, if negative the last series is referenced
    laScheme * scheme - the color scheme

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetSeriesScheme(laBarGraphWidget* graph, int32_t seriesID, laScheme * scheme);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_AddDataToSeries(laBarGraphWidget* graph, uint32_t seriesID, uint32_t categoryID, int32_t value)

  Summary:
    Adds a data (value) to the specified series at categoryID index

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    uint32_t seriesID - the series ID
    int32_t value - the value
    uint32_t * index - the destination to return the index of the added data

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_AddDataToSeries(laBarGraphWidget* graph, uint32_t seriesID, int32_t value, uint32_t * index);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetDataInSeries(laBarGraphWidget* graph,
                                              uint32_t seriesID,
                                              uint32_t index,
                                              int32_t value);
  Summary:
    Sets the value of the entry in the series index. The entry should have
    been previously

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    uint32_t seriesID - the series ID
    uint32_t index - the index of the data
    int32_t value - the value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetDataInSeries(laBarGraphWidget* graph,
                                          uint32_t seriesID,
                                          uint32_t index,
                                          int32_t value);


// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_DestroyAll(laBarGraphWidget* graph)

  Summary:
    Destroys data, series and categories and frees the memory allocated

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_DestroyAll(laBarGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetStringTable(laBarGraphWidget* graph, GFXU_StringTableAsset * stringTable)

  Summary:
    Sets the string table used for the generated axis labels

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    GFXU_StringTableAsset * stringTable - the string table

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetStringTable(laBarGraphWidget* graph, GFXU_StringTableAsset * stringTable);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetTicksLabelsStringID(laBarGraphWidget* graph, uint32_t stringID)

  Summary:
    Sets the ID of the superset string used for the value axis tick labels

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    uint32_t stringID - the string ID

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetTicksLabelsStringID(laBarGraphWidget* graph, uint32_t stringID);

// *****************************************************************************
/* Function:
    laBool laBarGraphWidget_GetGridlinesVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns GFX_TRUE if the axis gridlines are visible

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index

  Returns:
    laBool - GFX_TRUE if the axis gridlines are visible

  Remarks:

*/
LIB_EXPORT laBool laBarGraphWidget_GetGridlinesVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetGridlinesVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible)

  Summary:
    Shows/Hides the gridlines

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - category ID
    laBool visible - shows the gridlines if GFX_TRUE

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetGridlinesVisible(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBool visible);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_AddCategory(laBarGraphWidget* graph, uint32_t * id)

  Summary:
    Adds a category to the graph

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    uint32_t * id - destination of the ID of the new category

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_AddCategory(laBarGraphWidget* graph, uint32_t * id);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_GetCategoryText(laBarGraphWidget* graph, uint32_t categoryID, laString * str)

  Summary:
    Gets a copy of the string used to label the category

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    uint32_t categoryID - category ID
    laString * str - destination of the copied string

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_GetCategoryText(laBarGraphWidget* graph, uint32_t categoryID, laString * str);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetCategoryText(laBarGraphWidget* graph, uint32_t categoryID, laString str)

  Summary:
    Sets the string used to label the category

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    int32_t categoryID - category ID, if -1 the last category is assigned
    laString str - the string to use

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetCategoryText(laBarGraphWidget* graph, int32_t categoryID, laString str);

// *****************************************************************************
/* Function:
    laBool laBarGraphWidget_GetStacked(laBarGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the bars are stacked

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laBool laBarGraphWidget_GetStacked(laBarGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetStacked(laBarGraphWidget* graph, laBool stacked)

  Summary:
    Stacks the bar graph

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBool stacked - if GFX_TRUE, the bars are stacked

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetStacked(laBarGraphWidget* graph, laBool stacked);

// *****************************************************************************
/* Function:
    laBool laBarGraphWidget_GetCategoryAxisTicksVisible(laBarGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the category axis ticks are visible

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the category axis ticks are visible

  Remarks:

*/
LIB_EXPORT laBool laBarGraphWidget_GetCategoryAxisTicksVisible(laBarGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetCategoryAxisTicksVisible(laBarGraphWidget* graph, laBool visible)

  Summary:
    Shows/Hides the category axis ticks

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBool visible - if GFX_TRUE, the axis ticks are shown

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetCategoryAxisTicksVisible(laBarGraphWidget* graph, laBool visible);

// *****************************************************************************
/* Function:
    laBool laBarGraphWidget_GetCategoryAxisLabelsVisible(laBarGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the category axis labels are visible

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the category axis labels are visible

  Remarks:

*/
LIB_EXPORT laBool laBarGraphWidget_GetCategoryAxisLabelsVisible(laBarGraphWidget* graph);

// *****************************************************************************
/* Function:
    LIB_EXPORT laResult laBarGraphWidget_SetCategoryAxisLabelsVisible(laBarGraphWidget* graph, laBool visible)

  Summary:
    Shows/Hides the category axis labels

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBool visible - if GFX_TRUE, the axis labels are shown

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetCategoryAxisLabelsVisible(laBarGraphWidget* graph, laBool visible);

// *****************************************************************************
/* Function:
    laBool laBarGraphWidget_GetFillGraphArea(laBarGraphWidget* graph)

  Summary:
    Returns GFX_TRUE if the category axis labels are visible

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laBool - GFX_TRUE if the category axis labels are visible

  Remarks:

*/
LIB_EXPORT laBool laBarGraphWidget_GetFillGraphArea(laBarGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetFillGraphArea(laBarGraphWidget* graph, laBool fill)

  Summary:
    Sets the graph area filled or not

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBool fill - if GFX_TRUE, fills the graph area

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetFillGraphArea(laBarGraphWidget* graph, laBool fill);

// *****************************************************************************
/* Function:
    laBarGraphTickPosition laBarGraphWidget_GetValueAxisTicksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns the position of the ticks in the axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the index of the value axis

  Returns:
    laBarGraphTickPosition - the tick position

  Remarks:

*/
LIB_EXPORT laBarGraphTickPosition laBarGraphWidget_GetValueAxisTicksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetValueAxisTicksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBarGraphTickPosition position)

  Summary:
    Sets the position of the ticks in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    laBarGraphTickPosition position - the tick position

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetValueAxisTicksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBarGraphTickPosition position);

// *****************************************************************************
/* Function:
    laBarGraphTickPosition laBarGraphWidget_GetValueAxisSubticksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis)

  Summary:
    Returns the position of the subticks in the axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the index of the value axis

  Returns:
    laBarGraphTickPosition - the subtick position

  Remarks:

*/
LIB_EXPORT laBarGraphTickPosition laBarGraphWidget_GetValueAxisSubticksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetValueAxisSubticksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBarGraphTickPosition position)

  Summary:
    Sets the position of the subticks in the value axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphValueAxis axis - the value axis index
    laBarGraphTickPosition position  - position of the subticks

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetValueAxisSubticksPosition(laBarGraphWidget* graph, laBarGraphValueAxis axis, laBarGraphTickPosition position);

// *****************************************************************************
/* Function:
    laBarGraphTickPosition laBarGraphWidget_GetCategoryAxisTicksPosition(laBarGraphWidget* graph)

  Summary:
    Returns the position of the ticks in the category axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget

  Returns:
    laBarGraphTickPosition - position of the ticks in the category axis

  Remarks:

*/
LIB_EXPORT laBarGraphTickPosition laBarGraphWidget_GetCategoryAxisTicksPosition(laBarGraphWidget* graph);

// *****************************************************************************
/* Function:
    laResult laBarGraphWidget_SetCategoryAxisTicksPosition(laBarGraphWidget* graph, laBarGraphTickPosition position)

  Summary:
    Sets the position of the ticks in the category axis

  Description:

  Parameters:
    laBarGraphWidget* graph - the widget
    laBarGraphTickPosition position  - position of the ticks

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laBarGraphWidget_SetCategoryAxisTicksPosition(laBarGraphWidget* graph, laBarGraphTickPosition position);

// DOM-IGNORE-BEGIN
//Internal use only
void _laBarGraphWidget_GetGraphRect(laBarGraphWidget* graph,
                                           GFX_Rect * graphRect);
// DOM-IGNORE-END

#endif // LA_BAR_GRAPH_WIDGET_ENABLED
#endif /* LIBARIA_WIDGET_BAR_GRAPH_H */
