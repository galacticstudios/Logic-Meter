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
    libaria_widget_circular_slider.h

  Summary:


  Description:
    This module implements slider drawing widget functions.
*******************************************************************************/


#ifndef LIBARIA_WIDGET_CIRCULAR_SLIDER_H
#define LIBARIA_WIDGET_CIRCULAR_SLIDER_H

#include "gfx/libaria/inc/libaria_common.h"

#if LA_CIRCULAR_SLIDER_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    laCircularSliderWidgetDir_t

  Summary:
    Direction of the slider

  Description:
    Direction of the slider

  Remarks:
    None.
*/
typedef enum laCircularSliderWidgetDir_t
{
    CIRCULAR_SLIDER_DIR_COUNTER_CLOCKWISE,
    CIRCULAR_SLIDER_DIR_CLOCKWISE,
} laCircularSliderWidgetDir;

// *****************************************************************************
/* Structure:
    laCircularSliderButtonState_t

  Summary:
    State of the slider button

  Description:
    State of the slider button

  Remarks:
    None.
*/
typedef enum laCircularSliderButtonState_t
{
    LA_CIRCULAR_SLIDER_STATE_UP,
    LA_CIRCULAR_SLIDER_STATE_DOWN,
} laCircularSliderButtonState;

// *****************************************************************************
/* Enumeration:
    laCircularSliderWidgetArcType_t

  Summary:
    The arcs that compose the circular slider

  Description:
    The arcs that compose the circular slider

  Remarks:
    None.
*/
typedef enum laCircularSliderWidgetArcType_t
{
    OUTSIDE_CIRCLE_BORDER,
    INSIDE_CIRCLE_BORDER,
    ACTIVE_AREA,
    INACTIVE_AREA,
    CIRCLE_BUTTON,
} laCircularSliderWidgetArcType;

// *****************************************************************************
/* Structure:
    laCircularSliderArc_t

  Summary:
    Internal structure for the arcs in the circular slider widget

  Description:
    Describs the arc instances in the circular gauge widget

  Remarks:
    None.
*/
typedef struct laCircularSliderArc_t
{
    laBool visible;
    int32_t startAngle;
    int32_t centerAngle;
    uint32_t radius;
    uint32_t thickness;
    laScheme* scheme;
} laCircularSliderArc;

typedef struct laCircularSliderWidget_t laCircularSliderWidget;

// *****************************************************************************
/* Function Pointer:
    laCircularSliderWidget_PressedEvent

  Summary:
    Button pressed event function callback type
*/
typedef void (*laCircularSliderWidget_PressedEvent)(laCircularSliderWidget *, int32_t);

// *****************************************************************************
/* Function Pointer:
    laCircularSliderWidget_ValueChangedEvent

  Summary:
    Value changed event function callback type
*/
typedef void (*laCircularSliderWidget_ValueChangedEvent)(laCircularSliderWidget *, int32_t);

// *****************************************************************************
/* Function Pointer:
    laCircularSliderWidget_ReleasedEvent

  Summary:
    Button released event function callback type
*/
typedef void (*laCircularSliderWidget_ReleasedEvent)(laCircularSliderWidget *, int32_t);

// *****************************************************************************
/* Structure:
    laCircularSliderWidget_t

  Summary:
    Implementation of a slider widget.

  Description:
    A slider widget draws a slider of the specified origin and radius inside
    the widget bounds.  All coordinates are expressed in local widget space.

    The color of the slider is determined by the widget scheme's 'foreground'
    color.

  Remarks:
    None.
*/
typedef struct laCircularSliderWidget_t
{
    laWidget widget; // base widget header

    uint32_t radius; // the radius of the slider

    uint32_t startAngle; //the start angle of the slider

    uint32_t value;      //the value of the slider
    uint32_t startValue; //the start value of the slider
    uint32_t endValue;   //the end value of the slider
    float degPerUnit;   //degrees per unit in the slider

    laBool roundEdges; //round edges
    laBool sticky; //snaps to start value before wrapping around
    laBool buttonTouch; //only button is active to touch

    laCircularSliderWidgetDir direction;    //the direction of the slider

    laCircularSliderArc activeArc;            //the arc for the slider value
    laCircularSliderArc inActiveArc;          //the arc for the slider remainder
    laCircularSliderArc outsideBorderArc;     //the arc for the outside border
    laCircularSliderArc insideBorderArc;      //the arc for the inside border
    laCircularSliderArc circleButtonArc;      //the arc for the circle button

    laCircularSliderButtonState btnState;   //the state of the circular slider button

    laCircularSliderWidget_PressedEvent pressedCallback;
    laCircularSliderWidget_ValueChangedEvent valueChangedCallback;
    laCircularSliderWidget_ReleasedEvent releasedCallback;
} laCircularSliderWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laCircularSliderWidget_Constructor(laCircularSliderWidget* slider);
void _laCircularSliderWidget_Destructor(laCircularSliderWidget* slider);

void _laCircularSliderWidget_Paint(laCircularSliderWidget* slider);
GFX_Point _laCircularSliderWidget_GetCircleCenterPointAtValue(laCircularSliderWidget* slider, int32_t value);

// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laCircularSliderWidget* laCircularSliderWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the management of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:
    void

  Returns:
    laCircularSliderWidget*

  Remarks:

*/
LIB_EXPORT laCircularSliderWidget* laCircularSliderWidget_New();

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_GetOrigin(laCircularSliderWidget* slider, int32_t* x, int32_t* y)

  Summary:
    Gets the origin coordinates of a slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    int32_t* x - pointer to an integer pointer to store x
    int32_t* y - pointer to an integer pointer to store y

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_GetOrigin(laCircularSliderWidget* slider, int32_t* x, int32_t* y);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetOrigin(laCircularSliderWidget* slider, int32_t x, int32_t y)

  Summary:
    Sets the origin coordiates of a slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    int32_t x - the desired x origin coordinate
    int32_t y - the desired y origin coordinate

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetOrigin(laCircularSliderWidget* slider, int32_t x, int32_t y);

// *****************************************************************************
/* Function:
    uint32_t laCircularSliderWidget_GetRadius(laCircularSliderWidget* slider)

  Summary:
    Gets the radius of a slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularSliderWidget_GetRadius(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetRadius(laCircularSliderWidget* slider, uint32_t rad)

  Summary:
    Sets the radius of a slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    uint32_t red - the desired radius value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetRadius(laCircularSliderWidget* slider, uint32_t rad);

// *****************************************************************************
/* Function:
    int32_t laCircularSliderWidget_GetStartAngle(laCircularSliderWidget* slider)

  Summary:
    Returns the start angle of a slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularSliderWidget_GetStartAngle(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetStartAngle(laCircularSliderWidget* slider, int32_t angle)

  Summary:
    Sets the start angle of a slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    int32_t angle - the desired start angle value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetStartAngle(laCircularSliderWidget* slider, uint32_t angle);

// *****************************************************************************
/* Function:
    uint32_t laCircularSliderWidget_GetArcThickness(laCircularSliderWidget* slider, laCircularSliderWidgetArcType type)

  Summary:
    Returns the thickness of an arc in the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidgetArcType type - the type of arc

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularSliderWidget_GetArcThickness(laCircularSliderWidget* slider,
                                                           laCircularSliderWidgetArcType type);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetArcThickness(laCircularSliderWidget* slider,
                                                    laCircularSliderWidgetArcType type,
                                                    uint32_t thickness)

  Summary:
    Sets the thickness of an arc in the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidgetArcType type - the type of arc
    uint32_t thickness - the thickness of the arc

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetArcThickness(laCircularSliderWidget* slider,
                                                           laCircularSliderWidgetArcType type,
                                                           uint32_t thickness);

// *****************************************************************************
/* Function:
    uint32_t laCircularSliderWidget_GetArcRadius(laCircularSliderWidget* slider, laCircularSliderWidgetArcType type)

  Summary:
    Returns the radius of an arc in the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidgetArcType type - the type of arc

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularSliderWidget_GetArcRadius(laCircularSliderWidget* slider,
                                                        laCircularSliderWidgetArcType type);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetStartValue(laCircularSliderWidget* slider, uint32_t value)

  Summary:
    Sets the start value of the slider widget

  Description:

  Parameters:
    laCircularSliderWidget* slider - the widget
    uint32_t value

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetArcRadius(laCircularSliderWidget* slider,
                                                        laCircularSliderWidgetArcType type,
                                                        uint32_t radius);

// *****************************************************************************
/* Function:
    laScheme * laCircularSliderWidget_GetArcScheme(laCircularSliderWidget* slider, laCircularSliderWidgetArcType type)

  Summary:
    Returns the scheme of an arc in the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidgetArcType type - the type of arc

  Returns:
    laScheme *

  Remarks:

*/
LIB_EXPORT laScheme * laCircularSliderWidget_GetArcScheme(laCircularSliderWidget* slider,
                                                          laCircularSliderWidgetArcType type);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetArcScheme(laCircularSliderWidget* slider,
                                                        laCircularSliderWidgetArcType type,
                                                        laScheme * scheme)

  Summary:
    Sets the scheme of the specified arc

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidgetArcType type - the type of arc
    laScheme * scheme - scheme

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetArcScheme(laCircularSliderWidget* slider,
                                                        laCircularSliderWidgetArcType type,
                                                        laScheme * scheme);

// *****************************************************************************
/* Function:
    laBool laCircularSliderWidget_GetArcVisible(laCircularSliderWidget* slider, laCircularSliderWidgetArcType type)

  Summary:
    Returns true if the specified arc is visible

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidgetArcType type - the type of arc

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laCircularSliderWidget_GetArcVisible(laCircularSliderWidget* slider,
                                                         laCircularSliderWidgetArcType type);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetArcVisible(laCircularSliderWidget* slider,
                                                         laCircularSliderWidgetArcType type,
                                                         laBool visible)

  Summary:
    Shows/Hides the specified arc visible

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidgetArcType type - the type of arc
    laBool visible - show/hide

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetArcVisible(laCircularSliderWidget* slider,
                                                         laCircularSliderWidgetArcType type,
                                                         laBool visible);

// *****************************************************************************
/* Function:
    uint32_t laCircularSliderWidget_GetStartValue(laCircularSliderWidget* slider)

  Summary:
    Gets the start value of the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularSliderWidget_GetStartValue(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetStartValue(laCircularSliderWidget* slider, uint32_t value)

  Summary:
    Sets the start value of the slider widget

  Description:

  Parameters:
    laCircularSliderWidget* slider - the widget
    uint32_t value

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetStartValue(laCircularSliderWidget* slider, uint32_t value);

// *****************************************************************************
/* Function:
    uint32_t laCircularSliderWidget_GetEndValue(laCircularSliderWidget* slider)

  Summary:
    Gets the end value of the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularSliderWidget_GetEndValue(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetEndValue(laCircularSliderWidget* slider, uint32_t value)

  Summary:
    Sets the end value of the slider widget

  Description:

  Parameters:
    laCircularSliderWidget* slider - the widget
    uint32_t value

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetEndValue(laCircularSliderWidget* slider, uint32_t value);

// *****************************************************************************
/* Function:
    uint32_t laCircularSliderWidget_GetValue(laCircularSliderWidget* slider)

  Summary:
    Gets the value of the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularSliderWidget_GetValue(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetValue(laCircularSliderWidget* slider, uint32_t value)

  Summary:
    Sets the value of the slider widget

  Description:

  Parameters:
    laCircularSliderWidget* slider - the widget
    uint32_t value

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetValue(laCircularSliderWidget* slider, uint32_t value);

// *****************************************************************************
/* Function:
    laBool laCircularSliderWidget_GetRoundEdges(laCircularSliderWidget* slider)

  Summary:
    Returns true if the slider has rounded edges

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laCircularSliderWidget_GetRoundEdges(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetRoundEdges(laCircularSliderWidget* slider, laBool round)

  Summary:
    If round = true, the slider active area edges are round

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laBool round

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetRoundEdges(laCircularSliderWidget* slider, laBool round);

// *****************************************************************************
/* Function:
    laBool laCircularSliderWidget_GetStickyButton(laCircularSliderWidget* slider)

  Summary:
    Returns true if the slider button sticks to the start/end value

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laCircularSliderWidget_GetStickyButton(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetStickyButton(laCircularSliderWidget* slider, laBool c)

  Summary:
    If snap = true, the slider button sticks to the start/end value of the slider

  Description:
    When sticky is enabled, the slider button will not immediately wrap past the start or end value
    if the button is dragged past the start angle. If the button is dragged past the threshold of 90 degrees
    beyond the start angle, only then will the slider value wrap around.


  Parameters:
    laCircularSliderWidget* slider - the widget
    laBool sticky - set button as sticky

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetStickyButton(laCircularSliderWidget* slider, laBool sticky);


// *****************************************************************************
/* Function:
    laCircularSliderWidgetDir laCircularSliderWidget_GetDirection(laCircularSliderWidget* slider)

  Summary:
    Returns direction of the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    laCircularSliderWidgetDir

  Remarks:

*/

// *****************************************************************************
/* Function:
    laBool laCircularSliderWidget_GetTouchOnButtonOnly(laCircularSliderWidget* slider)

  Summary:
    Returns true if the slider slider only responds to touch inside the button area

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laCircularSliderWidget_GetTouchOnButtonOnly(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetTouchOnButtonOnly(laCircularSliderWidget* slider, laBool buttonOnly)

  Summary:
    If buttonOnly = true, the slider will only respond to touches inside the button area

  Description:

  Parameters:
    laCircularSliderWidget* slider - the widget
    laBool buttonOnly - set touch to button only

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetTouchOnButtonOnly(laCircularSliderWidget* slider, laBool buttonOnly);


// *****************************************************************************
/* Function:
    laCircularSliderWidgetDir laCircularSliderWidget_GetDirection(laCircularSliderWidget* slider)

  Summary:
    Returns direction of the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget

  Returns:
    laCircularSliderWidgetDir

  Remarks:

*/

LIB_EXPORT laCircularSliderWidgetDir laCircularSliderWidget_GetDirection(laCircularSliderWidget* slider);

// *****************************************************************************
/* Function:
    laResult laCircularSliderWidget_SetDirection(laCircularSliderWidget* slider, laCircularSliderWidgetDir dir)

  Summary:
    Sets the direction of the slider widget

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidgetDir dir - direction

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetDirection(laCircularSliderWidget* slider, laCircularSliderWidgetDir dir);


// *****************************************************************************
/* Function:
    laCircularSliderWidget_SetPressedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_ValueChangedEvent cb)

  Summary:
    Sets the function that gets called when the slider button is pressed

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidget_PressedEvent cb - the callback function

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetPressedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_PressedEvent cb);

// *****************************************************************************
/* Function:
    laCircularSliderWidget_SetValueChangedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_ValueChangedEvent cb)

  Summary:
    Sets the function that gets called when the slider value changes

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidget_ValueChangedEvent cb - the callback function

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetValueChangedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_ValueChangedEvent cb);

// *****************************************************************************
/* Function:
    laCircularSliderWidget_SetReleasedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_ReleasedEvent cb)

  Summary:
    Sets the function that gets called when the slider button is released

  Description:


  Parameters:
    laCircularSliderWidget* slider - the widget
    laCircularSliderWidget_ReleasedEvent cb - the callback function

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularSliderWidget_SetReleasedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_ReleasedEvent cb);

#endif // LA_CIRCULAR_SLIDER_WIDGET_ENABLED
#endif /* LIBARIA_WIDGET_CIRCULAR_SLIDER_H */
