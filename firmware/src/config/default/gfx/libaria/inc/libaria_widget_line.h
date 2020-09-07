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
    libaria_widget_line.h

  Summary:


  Description:
    This module implements line draw widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_LINE_H
#define LIBARIA_LINE_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_LINE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

typedef struct laLineWidget_t laLineWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    laLineWidget_t

  Summary:
    Defines the implementation of a line widget struct

  Description:
    A line widget draws a simple line shape within the confines of its
    bounding rectangle.  All coordinates are expressed in local widget space.

    The color of the line is determined by the widget scheme's 'foreground'
    color.

  Remarks:
    None.
*/
typedef struct laLineWidget_t
{
    laWidget widget; // widget base class

    int32_t x1; // point 1 x
    int32_t y1; // point 1 y
    int32_t x2; // point 2 x
    int32_t y2; // point 2 y
} laLineWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laLineWidget_Constructor(laLineWidget* line);
void _laLineWidget_Destructor(laLineWidget* line);

void _laLineWidget_Paint(laLineWidget* line);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laLineWidget* laLineWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:

  Returns:
    laLineWidget*

  Remarks:

*/
LIB_EXPORT laLineWidget* laLineWidget_New();

// *****************************************************************************
/* Function:
    laResult laLineWidget_GetStartPoint(laLineWidget* line, int32_t* x, int32_t* y)

  Summary:
    Gets the coordinates for the first point of the line.

  Description:


  Parameters:
    laLineWidget* line - the widget
    int32_t* x - pointer to an int to store the x coordinate
    int32_t* y - pointer to tan int to store the y coordinate

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineWidget_GetStartPoint(laLineWidget* line, int32_t* x, int32_t* y);

// *****************************************************************************
/* Function:
    laResult laLineWidget_SetStartPoint(laLineWidget* line, int32_t x, int32_t y)

  Summary:
    Sets the cooridnate for the first point of the line

  Description:


  Parameters:
    laLineWidget* line - the widget
    int32_t x - the x coordinate value
    int32_t y - the y coordinate value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineWidget_SetStartPoint(laLineWidget* line, int32_t x, int32_t y);

// *****************************************************************************
/* Function:
    laResult laLineWidget_GetEndPoint(laLineWidget* line, int32_t* x, int32_t* y)

  Summary:
    Gets the coordinates for the second point of the line.

  Description:


  Parameters:
    laLineWidget* line - the widget
    int32_t* x - pointer to an int to store the x coordinate
    int32_t* y - pointer to tan int to store the y coordinate

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineWidget_GetEndPoint(laLineWidget* line, int32_t* x, int32_t* y);

// *****************************************************************************
/* Function:
    laResult laLineWidget_SetEndPoint(laLineWidget* line, int32_t x, int32_t y)

  Summary:
    Sets the cooridnate for the second point of the line

  Description:


  Parameters:
    laLineWidget* line - the widget
    int32_t x - the x coordinate value
    int32_t y - the y coordinate value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLineWidget_SetEndPoint(laLineWidget* line, int32_t x, int32_t y);

#endif // LA_LINE_WIDGET_ENABLED
#endif /* LIBARIA_LINE_H */