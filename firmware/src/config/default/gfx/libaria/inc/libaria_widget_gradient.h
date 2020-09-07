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
    libaria_widget_gradient.h

  Summary:


  Description:
    This module implements gradient drawing widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_GRADIENT_H
#define LIBARIA_GRADIENT_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_GRADIENT_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laGradientWidgetDirection_t

  Summary:
    Implementation of a gradient widget.

  Description:
    A gradient widget is similar to a panel widget with the exception that it
    can draw a gradient color for its background.  This operation can be more
    costly than drawing a solid color and should be used sparingly.

    Gradient uses 'foreground' and 'foreground inactive' as its interpolated
    background draw colors.

  Remarks:
    None.
*/
typedef enum laGradientWidgetDirection_t
{
    LA_GRADIENT_DIRECTION_RIGHT,
    LA_GRADIENT_DIRECTION_DOWN,
    LA_GRADIENT_DIRECTION_LEFT,
    LA_GRADIENT_DIRECTION_UP
} laGradientWidgetDirection;

// *****************************************************************************
/* Enumeration:
    laGradientWidget_t

  Summary:
    Gradient widget struct definition.

  Description:


  Remarks:
    None.
*/
typedef struct laGradientWidget_t
{
    laWidget widget; // widget base class

    laGradientWidgetDirection dir; // gradient direction
} laGradientWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laGradientWidget_Constructor(laGradientWidget* grad);
void _laGradientWidget_Destructor(laGradientWidget* grad);

void _laGradientWidget_Paint(laGradientWidget* grad);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laGradientWidget* laGradientWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:
    void

  Returns:
    laGradientWidget*

  Remarks:

*/
LIB_EXPORT laGradientWidget* laGradientWidget_New();

// *****************************************************************************
/* Function:
    laGradientWidgetDirection laGradientWidget_GetDirection(laGradientWidget* grad)

  Summary:
    Gets the gradient direction value for this widget.

  Description:


  Parameters:
    laGradientWidget* grad - the widget

  Returns:
    laGradientWidgetDirection - the current gradient direction

  Remarks:

*/
LIB_EXPORT laGradientWidgetDirection laGradientWidget_GetDirection(laGradientWidget* grad);

// *****************************************************************************
/* Function:
    laResult laGradientWidget_SetDirection(laGradientWidget* grad, laGradientWidgetDirection dir)

  Summary:
    Sets the gradient direction value for this widget.

  Description:


  Parameters:
    laGradientWidget* grad - the widget
    laGradientWidgetDirection dir - the desired gradient direction

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laGradientWidget_SetDirection(laGradientWidget* grad, laGradientWidgetDirection dir);

#endif // LA_GRADIENT_WIDGET_ENABLED
#endif /* LIBARIA_RECTANGLE_H */