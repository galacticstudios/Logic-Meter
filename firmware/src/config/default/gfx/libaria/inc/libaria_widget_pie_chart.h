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
    libaria_widget_pie_chart.h

  Summary:


  Description:
    This module implements pie chart drawing widget functions.
*******************************************************************************/


#ifndef LIBARIA_WIDGET_PIE_CHART_H
#define LIBARIA_WIDGET_PIE_CHART_H

#include "gfx/libaria/inc/libaria_common.h"

#if LA_PIE_CHART_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

typedef struct laPieChartPie_t
{
    uint32_t value;
    uint32_t radius;
    uint32_t offset;
    laScheme* scheme;
} laPieChartPie;

typedef struct laPieChartWidget_t laPieChartWidget;

// *****************************************************************************
/* Function Pointer:
    laPieChartWidget_PressedEvent

  Summary:
    Chart pressed event function callback type
*/
typedef void (*laPieChartWidget_PressedEvent)(laPieChartWidget *, uint32_t);


// *****************************************************************************
/* Structure:
    laPieChartWidget_t

  Summary:
    Implementation of a pie chart widget.

  Description:
    A chart widget draws a chart of the specified origin and radius inside
    the widget bounds.  All coordinates are expressed in local widget space.

    The color of the chart is determined by the widget scheme's 'foreground'
    color.

  Remarks:
    None.
*/
typedef struct laPieChartWidget_t
{
    laWidget widget; // base widget header

    uint32_t startAngle;                    //the start angle of the chart
    int32_t centerAngle;                    //the center angle of the chart

    laArray pieArray;                       //list of pie/data

    //Label properties
    laBool labelsVisible;                 // are labels visible
    uint32_t labelsOffset;                   //offset of labels from center of pie
    GFXU_StringTableAsset * stringTable;    //string table
    uint32_t labelsStringID;                //ID of Superset string containing numbers

    laPieChartWidget_PressedEvent pressedCallback;
} laPieChartWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laPieChartWidget_Constructor(laPieChartWidget* chart);
void _laPieChartWidget_Destructor(laPieChartWidget* chart);

void _laPieChartWidget_Paint(laPieChartWidget* chart);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laPieChartWidget* laPieChartWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the management of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:
    void

  Returns:
    laPieChartWidget*

  Remarks:

*/
LIB_EXPORT laPieChartWidget* laPieChartWidget_New();

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_GetOrigin(laPieChartWidget* chart, int32_t* x, int32_t* y)

  Summary:
    Gets the origin coordinates of a chart widget

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    int32_t* x - pointer to an integer pointer to store x
    int32_t* y - pointer to an integer pointer to store y

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_GetOrigin(laPieChartWidget* chart, int32_t* x, int32_t* y);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetOrigin(laPieChartWidget* chart, int32_t x, int32_t y)

  Summary:
    Sets the origin coordiates of a chart widget

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    int32_t x - the desired x origin coordinate
    int32_t y - the desired y origin coordinate

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetOrigin(laPieChartWidget* chart, int32_t x, int32_t y);

// *****************************************************************************
/* Function:
    int32_t laPieChartWidget_GetStartAngle(laPieChartWidget* chart)

  Summary:
    Returns the start angle of a chart widget

  Description:


  Parameters:
    laPieChartWidget* chart - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT int32_t laPieChartWidget_GetStartAngle(laPieChartWidget* chart);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetStartAngle(laPieChartWidget* chart, int32_t angle)

  Summary:
    Sets the start angle of a chart widget

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    int32_t angle - the desired start angle value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetStartAngle(laPieChartWidget* chart, int32_t angle);

// *****************************************************************************
/* Function:
    int32_t laPieChartWidget_GetCenterAngle(laPieChartWidget* chart)

  Summary:
    Sets the center angle of the chart widget

  Description:


  Parameters:
    laPieChartWidget* chart - the widget

  Returns:
    int32_t

  Remarks:

*/
LIB_EXPORT int32_t laPieChartWidget_GetCenterAngle(laPieChartWidget* chart);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetCenterAngle(laPieChartWidget* chart, int32_t angle)

  Summary:
    Sets the center angle of the chart widget

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    int32_t angle - the desired center angle value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetCenterAngle(laPieChartWidget* chart, int32_t angle);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_AddEntry(laPieChartWidget* chart, int32_t * index)

  Summary:
    Adds an entry to the pie chart

  Description:
    The entry is always added to the end of the set. The index of the new entry
    is returned thru the index parameter.

  Parameters:
    laPieChartWidget* chart - the widget
    int32_t * index - returns the index of the entry

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_AddEntry(laPieChartWidget* chart, int32_t * index);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_DeleteEntries(laPieChartWidget* chart)

  Summary:
    Deletes ALL the data in the pie chart

  Description:

  Parameters:
    laPieChartWidget* chart - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_DeleteEntries(laPieChartWidget* chart);

// *****************************************************************************
/* Function:
    uint32_t laPieChartWidget_GetEntryValue(laPieChartWidget* chart, uint32_t index)

  Summary:
    Returns the value of the entry at the specified index

  Description:

  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t index - the entry index

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laPieChartWidget_GetEntryValue(laPieChartWidget* chart, uint32_t index);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetEntryValue(laPieChartWidget* chart, uint32_t index, uint32_t value)

  Summary:
    Sets the value of an entry at index

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t index - entry index
    uint32_t value - entry value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetEntryValue(laPieChartWidget* chart, uint32_t index, uint32_t value);

// *****************************************************************************
/* Function:
    uint32_t laPieChartWidget_GetEntryRadius(laPieChartWidget* chart, uint32_t index)

  Summary:
    Returns the radius of the entry at the specified index

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t index - the entry index

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laPieChartWidget_GetEntryRadius(laPieChartWidget* chart, uint32_t index);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetEntryRadius(laPieChartWidget* chart, uint32_t index, uint32_t rad)

  Summary:
    Sets the radius of an entry at index

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t index - entry index
    uint32_t rad - entry radius

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetEntryRadius(laPieChartWidget* chart, uint32_t index, uint32_t rad);

// *****************************************************************************
/* Function:
    uint32_t laPieChartWidget_GetEntryOffset(laPieChartWidget* chart, uint32_t index)

  Summary:
    Returns the offset of the entry at the specified index

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t index - the entry index

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laPieChartWidget_GetEntryOffset(laPieChartWidget* chart, uint32_t index);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetEntryOffset(laPieChartWidget* chart, uint32_t index, uint32_t offset)

  Summary:
    Sets the offset of an entry at index

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t offset - entry offset
    uint32_t rad - entry radius

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetEntryOffset(laPieChartWidget* chart, uint32_t index, uint32_t offset);

// *****************************************************************************
/* Function:
    laScheme * laPieChartWidget_GetEntryScheme(laPieChartWidget* chart, uint32_t index)

  Summary:
    Returns the scheme of the entry at the specified index

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t index - the entry index

  Returns:
    laScheme *

  Remarks:

*/
LIB_EXPORT laScheme * laPieChartWidget_GetEntryScheme(laPieChartWidget* chart, uint32_t index);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetEntryScheme(laPieChartWidget* chart, uint32_t index, uint32_t scheme)

  Summary:
    Sets the scheme of an entry at index

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t scheme - entry scheme
    uint32_t rad - entry radius

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetEntryScheme(laPieChartWidget* chart, uint32_t index, laScheme * scheme);

// *****************************************************************************
/* Function:
    aResult laPieChartWidget_SetPressedEventCallback(laPieChartWidget* chart, laPieChartWidget_PressedEvent cb)

  Summary:
    Sets the function called when the chart is pressed/touched

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    laPieChartWidget_PressedEvent cb - callback function

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetPressedEventCallback(laPieChartWidget* chart, laPieChartWidget_PressedEvent cb);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetStringTable(laPieChartWidget* chart, GFXU_StringTableAsset * stringTable)

  Summary:
    Sets the string table for the labels

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    GFXU_StringTableAsset * stringTable - the string table to use

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetStringTable(laPieChartWidget * chart, GFXU_StringTableAsset * stringTable);

// *****************************************************************************
/* Function:
    laPieChartWidget_SetLabelsStringID(laPieChartWidget* chart, uint32_t stringID)

  Summary:
    Sets the string asset for the labels

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t stringID - the ID of the string asset to use for labels

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetLabelsStringID(laPieChartWidget* chart, uint32_t stringID);

// *****************************************************************************
/* Function:
    laBool laPieChartWidget_GetLabelsVisible(laPieChartWidget* chart)

  Summary:
    Returns GFX_TRUE if the labels are shown, GFX_FALSE if hidden

  Description:


  Parameters:
    laPieChartWidget* chart - the widget

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laPieChartWidget_GetLabelsVisible(laPieChartWidget* chart);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetLabelsVisible(laPieChartWidget* chart, laBool visible)

  Summary:
    Shows/hides the data entry labels

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    laBool visible

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetLabelsVisible(laPieChartWidget* chart, laBool visible);

// *****************************************************************************
/* Function:
    uint32_t laPieChartWidget_GetLabelsOffset(laPieChartWidget* chart)

  Summary:
    Gets the offsets of the labels from the center

  Description:


  Parameters:
    laPieChartWidget* chart - the widget

  Returns:
    uint32_t - the offset

  Remarks:

*/
LIB_EXPORT uint32_t laPieChartWidget_GetLabelsOffset(laPieChartWidget* chart);

// *****************************************************************************
/* Function:
    laResult laPieChartWidget_SetLabelsOffset(laPieChartWidget* chart, uint32_t offset)

  Summary:
    Sets the offsets of the labels from the center

  Description:


  Parameters:
    laPieChartWidget* chart - the widget
    uint32_t offset

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laPieChartWidget_SetLabelsOffset(laPieChartWidget* chart, uint32_t offset);

#endif // LA_PIE_CHART_WIDGET_ENABLED
#endif /* LIBARIA_WIDGET_PIE_CHART_H */