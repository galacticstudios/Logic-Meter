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
    libaria_input.h

  Summary:
    

*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_INPUT_H
#define LIBARIA_INPUT_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_event.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

typedef struct laWidget_t laWidget;

// DOM-IGNORE-BEGIN
#define LA_INPUT_PRIMARY_ID    0
#define LA_MAX_TOUCH_STATES    2
// DOM-IGNORE-END

// *****************************************************************************
/* Enumeration:
    laGestureID

  Summary:
    Placeholder for eventual gesture support.

  Remarks:
    None.
*/
typedef enum laGestureID_t
{
    LA_GESTURE_NONE = 0
} laGestureID;

// *****************************************************************************
/* Enumeration:
    laKey

  Summary:
    All values possible for key entry from the libaria keyboard widget

  Description:
    All values possible for key entry from the libaria keyboard widget

  Remarks:
    None.
*/
typedef enum laKey_t
{
    KEY_NULL = 0,
    KEY_ESCAPE,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_PRINTSCREEN,
    KEY_SCROLLLOCK,
    KEY_PAUSE,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_BACKQUOTE,
    KEY_TAB,
    KEY_CAPSLOCK,
    KEY_BRACKET_LEFT,
    KEY_BRACKET_RIGHT,
    KEY_SLASH,
    KEY_SEMICOLON,
    KEY_QUOTE,
    KEY_BACKSLASH,
    KEY_EQUALS,
    KEY_BACKSPACE,
    KEY_MINUS,
    KEY_COMMA,
    KEY_ENTER,
    KEY_PERIOD,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_SPACE,
    KEY_LCTRL,
    KEY_RCTRL,
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_LALT,
    KEY_RALT,
    KEY_LMETA,
    KEY_RMETA,
    KEY_INSERT,
    KEY_HOME,
    KEY_PAGEUP,
    KEY_END,
    KEY_PAGEDOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_DOWN,
    KEY_UP,
    KEY_NUMLOCK,
    KEY_KP_DIVIDE,
    KEY_KP_MULTIPLY,
    KEY_KP_MINUS,
    KEY_KP_PLUS,
    KEY_KP_ENTER,
    KEY_KP_1,
    KEY_KP_2,
    KEY_KP_3,
    KEY_KP_4,
    KEY_KP_5,
    KEY_KP_6,
    KEY_KP_7,
    KEY_KP_8,
    KEY_KP_9,
    KEY_KP_0,
    KEY_KP_PERIOD,
    KEY_LAST = KEY_KP_PERIOD
} laKey;

// *****************************************************************************
/* Enumeration:
    laMouseButton

  Summary:
    All values possible for mouse key entry from the libaria mouse input

  Description:
    All values possible for mouse key entry from the libaria mouse input

  Remarks:
    None.
*/
typedef enum laMouseButton_t
{
    BUTTON_NONE = 0,
    BUTTON_LEFT,
    BUTTON_MIDDLE,
    BUTTON_RIGHT,
    BUTTON_WHEEL_UP,
    BUTTON_WHEEL_DOWN,
    BUTTON_LAST = BUTTON_WHEEL_DOWN
} laMouseButton;

#define NUM_KEYS    KEY_LAST + 1
#define NUM_BUTTONS BUTTON_LAST + 1

// *****************************************************************************
/* Structure:
    laTouchState

  Summary:
    Manage the touch input state and track the touch coordinate

  Description:
    Manage the touch input state and track the touch coordinate

  Remarks:
    None.
*/
typedef struct laTouchState_t
{
    uint32_t valid;
    int32_t  x;
    int32_t  y;
} laTouchState;

// *****************************************************************************
/* Structure:
    laInputState_t

  Summary:
    Maintain a history of touch states; currently libaria keeps track of the last touch state only.

  Description:
    Maintain a history of touch states; currently libaria keeps track of the last touch state only.

  Remarks:
    None.
*/
typedef struct laInputState_t
{
    laBool enabled;

    laTouchState touch[LA_MAX_TOUCH_STATES];
    //uint8_t keyState[KEY_LAST];  // change to bitset?
    //laMouseState_t mouse;
    
} laInputState;

// *****************************************************************************
/* Structure:
    laInput_TouchDownEvent_t

  Summary:
    Register and handle the touch press detect event

  Description:
    Register and handle the touch press detect event

  Remarks:
    None.
*/
typedef struct laInput_TouchDownEvent_t
{
    laWidgetEvent event;
    
    int32_t touchID;
    int32_t x;
    int32_t y;
} laInput_TouchDownEvent;

// *****************************************************************************
/* Structure:
    laInput_TouchUpEvent_t

  Summary:
    Register and handle the touch release detect event

  Description:
    Register and handle the touch release detect event

  Remarks:
    None.
*/
typedef struct laInput_TouchUpEvent_t
{
    laWidgetEvent event;
    
    int32_t touchID;
    int32_t x;
    int32_t y;
} laInput_TouchUpEvent;

// *****************************************************************************
/* Structure:
    laInput_TouchMovedEvent_t

  Summary:
    Register and handle the touch coordinates changed event

  Description:
    Register and handle the touch coordinates changed event

  Remarks:
    None.
*/
typedef struct laInput_TouchMovedEvent_t
{
    laWidgetEvent event;
    
    int32_t touchID;
    int32_t prevX;
    int32_t prevY;
    int32_t x;
    int32_t y;
} laInput_TouchMovedEvent;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laBool laInput_GetEnabled()

  Summary:
    Returns the input enabled status of the current context

  Description:
    Returns the input enabled status of the current context

  Parameters:
    
    
  Returns:
    laBool
    
  Remarks:
    
*/
LIB_EXPORT laBool   laInput_GetEnabled();

// *****************************************************************************
/* Function:
    laResult laInput_SetEnabled(laBool enable)

  Summary:
    Sets the input status of the current context with the specified input argument 

  Description:
    Sets the input status of the current context with the specified input argument

  Parameters:
    laBool
    
  Returns:
    laResult
    
  Remarks:
    
*/
LIB_EXPORT laResult laInput_SetEnabled(laBool enable);

// *****************************************************************************
/* Function:
    laResult laInput_InjectTouchDown(uint32_t id, int32_t x, int32_t y)

  Summary:
    Register and track the touch down event and queue it for handling by associated widgets 

  Description:
    Register and track the touch down event and queue it for handling by associated widgets

  Parameters:
    uint32_t
    int32_t x
    int32_t y
    
  Returns:
    laResult
    
  Remarks:
    
*/
LIB_EXPORT laResult laInput_InjectTouchDown(uint32_t id, int32_t x, int32_t y);

// *****************************************************************************
/* Function:
    laResult laInput_InjectTouchUp(uint32_t id, int32_t x, int32_t y)

  Summary:
    Register and track the touch up event and queue it for handling by associated widgets

  Description:
    Register and track the touch up event and queue it for handling by associated widgets

  Parameters:
    uint32_t
    int32_t x
    int32_t y
    
  Returns:
    laResult
    
  Remarks:
    
*/
LIB_EXPORT laResult laInput_InjectTouchUp(uint32_t id, int32_t x, int32_t y);

// *****************************************************************************
/* Function:
    laResult laInput_InjectTouchMoved(uint32_t id, int32_t x, int32_t y)

  Summary:
    Register and track the touch moved event and queue it for handling by associated widgets

  Description:
    Register and track the touch moved event and queue it for handling by associated widgets

  Parameters:
    uint32_t
    int32_t x
    int32_t y
    
  Returns:
    laResult
    
  Remarks:
    
*/
LIB_EXPORT laResult laInput_InjectTouchMoved(uint32_t id, int32_t x, int32_t y);

// DOM-IGNORE-BEGIN
// alternative input APIs (not yet implemented)

/*
LIB_EXPORT laResult laInput_InjectGesture(laGesture id, int32_t x, int32_t y);

LIB_EXPORT laResult laInput_InjectKeyDown(laKey key);
LIB_EXPORT laResult laInput_InjectKeyUp(laKey key);

LIB_EXPORT laResult laInput_InjectMouseButtonDown(laMouseButton button);
LIB_EXPORT laResult laInput_InjectMouseButtonUp(laMouseButton button);
LIB_EXPORT laResult laInput_InjectMouseMoved(int32_t x, int32_t y);*/

laEventResult _laInput_HandleInputEvent(laEvent* evt);
// DOM-IGNORE-END
#endif /* LIBARIA_INPUT_H */
