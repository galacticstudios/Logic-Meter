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
    libaria_widget_circle.h

  Summary:


  Description:
    This module implements circle drawing widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_CIRCLE_H
#define LIBARIA_CIRCLE_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_CIRCLE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    laCircleWidget_t

  Summary:
    Implementation of a circle widget.

  Description:
    A circle widget draws a circle of the specified origin and radius inside
    the widget bounds.  All coordinates are expressed in local widget space.

    The color of the circle is determined by the widget scheme's 'foreground'
    color.

  Remarks:
    None.
*/
typedef struct laCircleWidget_t
{
    laWidget widget; // base widget header

    int32_t x; // the origin x cooridnate
    int32_t y; // the origin y coordinate
    int32_t radius; // the radius of the circle
    int32_t thickness; // the thickness of the circle outline
    laBool filled; // fills the circle area
} laCircleWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laCircleWidget_Constructor(laCircleWidget* cir);
void _laCircleWidget_Destructor(laCircleWidget* cir);

void _laCircleWidget_Paint(laCircleWidget* cir);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laCircleWidget* laCircleWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:
    void

  Returns:
    laCircleWidget*

  Remarks:

*/
LIB_EXPORT laCircleWidget* laCircleWidget_New();

// *****************************************************************************
/* Function:
    laResult laCircleWidget_GetOrigin(laCircleWidget* cir, int32_t* x, int32_t* y)

  Summary:
    Gets the origin coordiates of a circle widget

  Description:


  Parameters:
    laCircleWidget* cir - the widget
    int32_t* x - pointer to an integer pointer to store x
    int32_t* y - pointer to an integer pointer to store y

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircleWidget_GetOrigin(laCircleWidget* cir, int32_t* x, int32_t* y);

// *****************************************************************************
/* Function:
    laResult laCircleWidget_SetOrigin(laCircleWidget* cir, int32_t x, int32_t y)

  Summary:
    Sets the origin coordiates of a circle widget

  Description:


  Parameters:
    laCircleWidget* cir - the widget
    int32_t x - the desired x origin coordinate
    int32_t y - the desired y origin coordinate

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircleWidget_SetOrigin(laCircleWidget* cir, int32_t x, int32_t y);

// *****************************************************************************
/* Function:
    uint32_t laCircleWidget_GetRadius(laCircleWidget* cir)

  Summary:
    Gets the radius of a circle widget

  Description:


  Parameters:
    laCircleWidget* cir - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircleWidget_GetRadius(laCircleWidget* cir);

// *****************************************************************************
/* Function:
    laResult laCircleWidget_SetRadius(laCircleWidget* cir, uint32_t rad)

  Summary:
    Sets the radius of a circle widget

  Description:


  Parameters:
    laCircleWidget* cir - the widget
    uint32_t red - the desired radius value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircleWidget_SetRadius(laCircleWidget* cir, uint32_t rad);

// *****************************************************************************
/* Function:
    uint32_t laCircleWidget_GetThickness(laCircleWidget* cir)

  Summary:
    Gets the thickness of a circle widget

  Description:


  Parameters:
    laCircleWidget* cir - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircleWidget_GetThickness(laCircleWidget* cir);

// *****************************************************************************
/* Function:
    laResult laCircleWidget_SetThickness(laCircleWidget* cir, uint32_t thickness)

  Summary:
    Sets the thickness of a circle widget

  Description:


  Parameters:
    laCircleWidget* cir - the widget
    uint32_t thickness - the desired thickness value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircleWidget_SetThickness(laCircleWidget* cir, uint32_t thickness);

// *****************************************************************************
/* Function:
    laBool laCircleWidget_GetFilled(laCircleWidget* cir)

  Summary:
    Gets the filled state of a circle widget

  Description:


  Parameters:
    laCircleWidget* cir - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircleWidget_GetFilled(laCircleWidget* cir);

// *****************************************************************************
/* Function:
    laResult laCircleWidget_SetFilled(laCircleWidget* cir, laBool filled)

  Summary:
    Sets the filled state of a circle widget

  Description:


  Parameters:
    laCircleWidget* cir - the widget
    laBool thickness - filled or not

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircleWidget_SetFilled(laCircleWidget* cir, laBool filled);

#endif // LA_CIRCLE_WIDGET_ENABLED
#endif /* LIBARIA_CIRCLE_H */