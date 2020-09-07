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
    libaria_widget_scrollbar.h

  Summary:


  Description:
    This module implements scroll bar widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_SCROLLBAR_H
#define LIBARIA_SCROLLBAR_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_SCROLLBAR_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_string.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laScrollBarState_t

  Summary:
    Defines the various scroll bar state values

  Description:


  Remarks:
    None.
*/
typedef enum laScrollBarState_t
{
    LA_SCROLLBAR_STATE_NONE,
    LA_SCROLLBAR_STATE_TOP_PRESSED,
    LA_SCROLLBAR_STATE_TOP_INSIDE,
    LA_SCROLLBAR_STATE_BOTTOM_PRESSED,
    LA_SCROLLBAR_STATE_BOTTOM_INSIDE,
    LA_SCROLLBAR_STATE_HANDLE_DOWN
} laScrollBarState;

// *****************************************************************************
/* Enumeration:
    laScrollBarOrientation_t

  Summary:
    Defines the scroll bar direction values

  Description:


  Remarks:
    None.
*/
typedef enum laScrollBarOrientation_t
{
    LA_SCROLLBAR_ORIENT_VERTICAL,
    LA_SCROLLBAR_ORIENT_HORIZONTAL
} laScrollBarOrientation;

typedef struct laScrollBarWidget_t laScrollBarWidget;

// *****************************************************************************
/* Function Pointer:
    laScrollBarWidget_ValueChangedEvent

  Summary:
    Value changed event function callback type
*/
typedef void (*laScrollBarWidget_ValueChangedEvent)(laScrollBarWidget*);

// *****************************************************************************
/* Structure:
    laScrollBarWidget_t

  Summary:
    Implementation of a scroll bar widget.

  Description:
    A scroll bar is a widget that is capable of displaying a range and a scroll
    handle.  The handle can grow and shrink in size depending on the scroll
    range and visible scroll space and can be interacted with to scroll through
    the available space.

  Remarks:
    None.
*/
typedef struct laScrollBarWidget_t
{
    laWidget widget; // widget base class

    laScrollBarState state; // scrollbar input state
    laScrollBarOrientation alignment; // scroll bar direction

    uint32_t max; // maximum scroll value
    uint32_t extent; // visible space/handle size
    uint32_t value; // current scroll value
    uint32_t step; // discreet scroll stepping value

    laScrollBarWidget_ValueChangedEvent valueChangedEvent; // value changed callback

    GFX_Point handleDownOffset;
} laScrollBarWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laScrollBarWidget_Constructor(laScrollBarWidget* bar);
void _laScrollBarWidget_Destructor(laScrollBarWidget* bar);

void _laScrollBarWidget_Update(laScrollBarWidget* bar);
void _laScrollBarWidget_Paint(laScrollBarWidget* bar);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laScrollBarWidget* laScrollBarWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:


  Parameters:

  Returns:
    laScrollBarWidget*

  Remarks:

*/
LIB_EXPORT laScrollBarWidget* laScrollBarWidget_New();

// *****************************************************************************
/* Function:
    laScrollBarOrientation laScrollBarWidget_GetOrientation(laScrollBarWidget* bar)

  Summary:
    Gets the orientation value for the scroll bar

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    laScrollBarOrientation - the orientation value

  Remarks:

*/
LIB_EXPORT laScrollBarOrientation laScrollBarWidget_GetOrientation(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_SetOrientation(laScrollBarWidget* bar,
                                              laScrollBarOrientation align)

  Summary:
    Sets the orientation value of the scroll bar

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget
    laScrollBarOrientation - the desired orientation value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_SetOrientation(laScrollBarWidget* bar,
                                                     laScrollBarOrientation align,
                                                     laBool swapDimensions);

// *****************************************************************************
/* Function:
    uint32_t laScrollBarWidget_GetMaxinumValue(laScrollBarWidget* bar)

  Summary:
    Gets the maximum scroll value

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    uint32_t - the maximum scroll value

  Remarks:

*/
LIB_EXPORT uint32_t laScrollBarWidget_GetMaxinumValue(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_SetMaximumValue(laScrollBarWidget* bar,
                                               uint32_t val)
  Summary:
    Sets the maximum scroll value

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget
    uint32_t val - the desired maximum scroll value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_SetMaximumValue(laScrollBarWidget* bar,
                                                      uint32_t val);

// *****************************************************************************
/* Function:
    uint32_t laScrollBarWidget_GetExtentValue(laScrollBarWidget* bar)

  Summary:
    Gets the current scroll bar extent value

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    uint32_t - the extent value

  Remarks:

*/
LIB_EXPORT uint32_t laScrollBarWidget_GetExtentValue(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_SetExtentValue(laScrollBarWidget* bar,
                                              uint32_t val)
  Summary:
    Sets the scroll bar extent value

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget
    uint32_t val - the extent value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_SetExtentValue(laScrollBarWidget* bar,
                                                     uint32_t val);

// *****************************************************************************
/* Function:
    uint32_t laScrollBarWidget_GetScrollValue(laScrollBarWidget* bar)

  Summary:
    Gets the current scroll value

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    uint32_t - the scroll value

  Remarks:

*/
LIB_EXPORT uint32_t laScrollBarWidget_GetScrollValue(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_SetScrollValue(laScrollBarWidget* bar,
                                                     uint32_t val)
  Summary:
    Sets the current scroll value

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget
    uint32_t - the desired scroll value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_SetScrollValue(laScrollBarWidget* bar,
                                                     uint32_t val);

// *****************************************************************************
/* Function:
    uint32_t laScrollBarWidget_GetScrollPercentage(laScrollBarWidget* bar)

  Summary:
    Gets the current scroll value as a percentage

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    uint32_t - the scroll percentage

  Remarks:

*/
LIB_EXPORT uint32_t laScrollBarWidget_GetScrollPercentage(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_SetScrollPercentage(laScrollBarWidget* bar,
                                                   uint32_t val)
  Summary:
    Sets the current scroll value using a percentage.  Percentage should be a
    value from 0 - 100

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget
    uint32_t val - a value from 0 - 100

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_SetScrollPercentage(laScrollBarWidget* bar,
                                                          uint32_t val);

// *****************************************************************************
/* Function:
    uint32_t laScrollBarWidget_GetStepSize(laScrollBarWidget* bar)

  Summary:
    Gets the current discreet step size

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    uint32_t - the current step size

  Remarks:

*/
LIB_EXPORT uint32_t laScrollBarWidget_GetStepSize(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_SetStepSize(laScrollBarWidget* bar,
                                           uint32_t val)

  Summary:
    Sets the current step size

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget
    uint32_t val - the desired step size

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_SetStepSize(laScrollBarWidget* bar,
                                                  uint32_t val);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_StepBackward(laScrollBarWidget* bar)

  Summary:
    Moves the scroll value back by the current step size

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_StepBackward(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_StepForward(laScrollBarWidget* bar)

  Summary:
    Moves the scroll value forward by the current step size

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_StepForward(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laScrollBarWidget_ValueChangedEvent laScrollBarWidget_GetValueChangedEventCallback(laScrollBarWidget* bar)

  Summary:
    Gets the current value changed callback function pointer

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget

  Returns:
    laScrollBarWidget_ValueChangedEvent - a valid pointer or NULL

  Remarks:

*/
LIB_EXPORT laScrollBarWidget_ValueChangedEvent laScrollBarWidget_GetValueChangedEventCallback(laScrollBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laScrollBarWidget_SetValueChangedEventCallback(laScrollBarWidget* bar,
                                                            laScrollBarWidget_ValueChangedEvent cb)
  Summary:
    Sets the value changed event callback pointer

  Description:


  Parameters:
    laScrollBarWidget* bar - the widget
    laScrollBarWidget_ValueChangedEvent - a valid pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laScrollBarWidget_SetValueChangedEventCallback(laScrollBarWidget* bar,
                                                                   laScrollBarWidget_ValueChangedEvent cb);

// DOM-IGNORE-BEGIN
// internal use only
void _laScrollBarWidget_TouchDownEvent(laScrollBarWidget* bar, laInput_TouchDownEvent* evt);
void _laScrollBarWidget_TouchUpEvent(laScrollBarWidget* bar, laInput_TouchUpEvent* evt);
void _laScrollBarWidget_TouchMovedEvent(laScrollBarWidget* bar, laInput_TouchMovedEvent* evt);

void _laScrollBar_GetUpLeftButtonRect(laScrollBarWidget* bar, GFX_Rect* rect);
void _laScrollBar_GetDownRightButtonRect(laScrollBarWidget* bar, GFX_Rect* rect);
void _laScrollBar_GetScrollAreaRect(laScrollBarWidget* bar, GFX_Rect* rect);
void _laScrollBar_GetHandleRect(laScrollBarWidget* bar, GFX_Rect* rect);

uint32_t _laScrollWidget_GetExtent(laScrollBarWidget* bar);
uint32_t _laScrollBar_GetPercentFromPoint(laScrollBarWidget* bar, GFX_Point* pnt);
uint32_t _laScrollBar_GetValueFromPercent(laScrollBarWidget* bar, uint32_t per);

void _laScrollBarWidget_InvalidateBorderAreas(laScrollBarWidget* bar);
// DOM-IGNORE-END

#endif // LA_SCROLLBAR_WIDGET_ENABLED
#endif /* LIBARIA_SCROLLBAR_H */
