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
    libaria_widget_rectangle.h

  Summary:
    

  Description:
    This module implements rectangle drawing widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_RECTANGLE_H
#define LIBARIA_RECTANGLE_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_RECTANGLE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

typedef struct laRectangleWidget_t laRectangleWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laRectangleWidget_t

  Summary:
    Implementation of a rectangle widget struct

  Description:
    A rectangle widget draws a basic rectangle of a given thickness using the
    widget's bounding box as the dimensions.

  Remarks:
    None.
*/
typedef struct laRectangleWidget_t
{
    laWidget widget; // widget base class

    int32_t thickness; // rectangle border thickness
} laRectangleWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laRectangleWidget* laRectangleWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:
    

  Parameters:
    
  Returns:
    laRectangleWidget*
    
  Remarks:
    
*/
LIB_EXPORT laRectangleWidget* laRectangleWidget_New();

// *****************************************************************************
/* Function:
    int32_t laRectangleWidget_GetThickness(laRectangleWidget* rect)

  Summary:
    Gets the rectangle border thickness setting

  Description:
    

  Parameters:
    laRectangleWidget* rect - the widget
    
  Returns:
    int32_t - the border thickness setting
    
  Remarks:
    
*/
LIB_EXPORT int32_t laRectangleWidget_GetThickness(laRectangleWidget* rect);

// *****************************************************************************
/* Function:
    laResult laRectangleWidget_SetThickness(laRectangleWidget* rect,
                                            int32_t thk)

  Summary:
    Sets the rectangle border thickness setting

  Description:
    

  Parameters:
    laRectangleWidget* rect - the widget
    int32_t thk - the thickness setting
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/
LIB_EXPORT laResult laRectangleWidget_SetThickness(laRectangleWidget* rect,
                                                   int32_t thk);

// DOM-IGNORE-BEGIN
// internal use only
void _laRectangleWidget_InvalidateRect(laRectangleWidget* rct);
// DOM-IGNORE-END

#endif // LA_RECTANGLE_WIDGET_ENABLED
#endif /* LIBARIA_RECTANGLE_H */