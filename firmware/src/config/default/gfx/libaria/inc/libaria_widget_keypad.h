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
    libaria_widget_keypad.h

  Summary:


  Description:
    This module implements keypad widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_KEYPAD_H
#define LIBARIA_KEYPAD_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_KEYPAD_WIDGET_ENABLED && LA_BUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laKeyPadWidget_t laKeyPadWidget;
typedef struct laButtonWidget_t laButtonWidget;

// *****************************************************************************
/* Function Pointer:
    laKeyPadWidget_KeyClickEvent

  Summary:
    Key click event function callback type
*/
typedef void (*laKeyPadWidget_KeyClickEvent)(laKeyPadWidget*,
                                             laButtonWidget*,
                                             uint32_t,
                                             uint32_t);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    laKeyPadCellAction_t

  Summary:
    Defines an assigned action to a key pad cell

  Description:


  Remarks:
    None.
*/
typedef enum laKeyPadCellAction_t
{
    LA_KEYPAD_CELL_ACTION_NONE,
    LA_KEYPAD_CELL_ACTION_APPEND,
    LA_KEYPAD_CELL_ACTION_SET,
    LA_KEYPAD_CELL_ACTION_BACKSPACE,
    LA_KEYPAD_CELL_ACTION_CLEAR,
    LA_KEYPAD_CELL_ACTION_ACCEPT
} laKeyPadCellAction;

// *****************************************************************************
/* Structure:
    laKeyPadActionTrigger_t

  Summary:
    Defines the trigger for keypad action and events

  Description:


  Remarks:
    None.
*/
typedef enum laKeyPadActionTrigger_t
{
    LA_KEYPAD_TRIGGER_KEYRELEASED,
    LA_KEYPAD_TRIGGER_KEYPRESSED,
} laKeyPadActionTrigger;

// *****************************************************************************
/* Structure:
    laKeyPadCell_t

  Summary:
    Defines a key pad cell struct

  Description:
    A key pad is made up of an array of key pad cells.  Each cell is individually
    an laButtonWidget, an action, a value, and a few other options.

  Remarks:
    None.
*/
typedef struct laKeyPadCell_t
{
    laBool enabled; // indicates if the cell should be drawn
    laButtonWidget* button; // the button that handles the cell input events
                            // and rendering
    laKeyPadCellAction action; // the action that occurs when the cell is
                               // activated
    laString value; // the value that is passed to the edit event system
} laKeyPadCell;

// *****************************************************************************
/* Structure:
    laKeyPadCell_t

  Summary:
    Defines a key pad widget struct

  Description:
    A key pad is a widget that is comprised of an array of laButtonWidgets.
    This widget serves to issue edit events based on application or input
    interaction.  Receptor edit widgets can then receive these edit events
    and react accordingly.

  Remarks:
    None.
*/
typedef struct laKeyPadWidget_t
{
    laWidget widget; // widget base class

    uint32_t rows; // number of button rows
    uint32_t cols; // number of button columns

    laKeyPadActionTrigger trigger; //trigger for action and events

    laKeyPadCell* cells; // key cell array

    laKeyPadWidget_KeyClickEvent clickEvt; // key click callback event

    GFXU_ExternalAssetReader* reader; // asset reader
} laKeyPadWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laKeyPadWidget_Constructor(laKeyPadWidget* pad, uint32_t rows, uint32_t cols);
void _laKeyPadWidget_Destructor(laKeyPadWidget* pad);

void _laKeyPadWidget_Paint(laKeyPadWidget* pad);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laKeyPadWidget* laKeyPadWidget_New(uint32_t rows, uint32_t cols)

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:


  Parameters:
    uint32_t - number of rows to create
    uint32_t - number of columns to create

  Returns:
    laKeyPadWidget*

  Remarks:

*/
LIB_EXPORT laKeyPadWidget* laKeyPadWidget_New(uint32_t rows, uint32_t cols);


// *****************************************************************************
/* Function:
    laKeyPadActionTrigger laKeyPadWidget_SetKeyPadActionTrigger(laKeyPadWidget* pad)

  Summary:
    Gets the current trigger for keypad edit action and events

  Description:

  Parameters:
    laKeyPadWidget* pad - the widget

  Returns:
    laKeyPadActionTrigger - the trigger

  Remarks:

*/
LIB_EXPORT laKeyPadActionTrigger laKeyPadWidget_GetKeyPadActionTrigger(laKeyPadWidget* pad);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyPadActionTrigger(laKeyPadWidget* pad,
                                                   laKeyPadActionTrigger trigger)

  Summary:
    Sets the current trigger for keypad edit action and events

  Description:
    Keypad actions and events callback will be called based on the trigger

  Parameters:
    laKeyPadWidget* pad - the widget
    laKeyPadActionTrigger trigger - the trigger

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyPadActionTrigger(laKeyPadWidget* pad,
                                                          laKeyPadActionTrigger trigger);


// *****************************************************************************
/* Function:
    laKeyPadWidget_KeyClickEvent laKeyPadWidget_GetKeyClickEventCallback(laKeyPadWidget* pad)

  Summary:
    Gets the current key click event callback pointer

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget

  Returns:
    laKeyPadWidget_KeyClickEvent - the callback pointer

  Remarks:

*/
LIB_EXPORT laKeyPadWidget_KeyClickEvent laKeyPadWidget_GetKeyClickEventCallback(laKeyPadWidget* pad);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyClickEventCallback(laKeyPadWidget* pad,
                                                     laKeyPadWidget_KeyClickEvent cb)

  Summary:
    Sets the current key click event callback pointer

  Description:
    The key click event callback pointer is issued any time a button is
    interacted with.

  Parameters:
    laKeyPadWidget* pad - the widget
    laKeyPadWidget_KeyClickEvent cb - the callback pointer

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyClickEventCallback(laKeyPadWidget* pad,
                                                            laKeyPadWidget_KeyClickEvent cb);

// *****************************************************************************
/* Function:
    laBool laKeyPadWidget_GetKeyEnabled(laKeyPadWidget* pad,
                                        uint32_t row,
                                        uint32_t col)

  Summary:
    Gets the enabled flag for a cell at a given row/column

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column

  Returns:
    laBool - the flag value

  Remarks:

*/
LIB_EXPORT laBool laKeyPadWidget_GetKeyEnabled(laKeyPadWidget* pad,
                                               uint32_t row,
                                               uint32_t col);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyEnabled(laKeyPadWidget* pad,
                                          uint32_t row,
                                          uint32_t col,
                                          laBool enabled)

  Summary:
    Sets the enabled flag for a cell at the given row/column

  Description:
    The enabled flag controls the visibility and interactibility of a key pad
    cell.  This enables the key pad to be configured to match such examples as
    a phone dialer key pad with twelve buttons total but the buttons to the left
    and right of the zero button not being drawn.

  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    laBool enabled - the flag value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyEnabled(laKeyPadWidget* pad,
                                                 uint32_t row,
                                                 uint32_t col,
                                                 laBool enabled);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_GetKeyText(laKeyPadWidget* pad,
                                       uint32_t row,
                                       uint32_t col,
                                       laString* str)

  Summary:
    Returns a copy of the display text for a given cell at row/column

  Description:
    This function allocates memory for the input string argument.  The application
    becomes responsible for the management of the memory after function
    completion.

    The input string does not need to be initialized in any fashion before
    calling this function.

  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    laString* str - a pointer to an laString object

  Returns:
    laResult - the result of the operation

  Remarks:
*/
LIB_EXPORT laResult laKeyPadWidget_GetKeyText(laKeyPadWidget* pad,
                                              uint32_t row,
                                              uint32_t col,
                                              laString* str);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyText(laKeyPadWidget* pad,
                                       uint32_t row,
                                       uint32_t col,
                                       laString str)

  Summary:
    Sets the display text for a given cell at row/column

  Description:
    Sets the display text for a given cell at row/column

  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    laString str - an laString object

  Returns:
    laResult - the result of the operation

  Remarks:
*/

LIB_EXPORT laResult laKeyPadWidget_SetKeyText(laKeyPadWidget* pad,
                                              uint32_t row,
                                              uint32_t col,
                                              laString str);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laKeyPadWidget_GetKeyPressedImage(laKeyPadWidget* pad,
                                                              uint32_t row,
                                                              uint32_t col)

  Summary:
    Gets the pressed icon image asset pointer for the display image for
    a key pad cell

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column

  Returns:
    GFXU_ImageAsset* - pointer to the icon image asset

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laKeyPadWidget_GetKeyPressedImage(laKeyPadWidget* pad,
                                                              uint32_t row,
                                                              uint32_t col);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyPressedImage(laKeyPadWidget* pad,
                                               uint32_t row,
                                               uint32_t col,
                                               GFXU_ImageAsset* img)

  Summary:
    Sets the pressed icon image asset pointer for a key pad cell

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    GFXU_ImageAsset* img - pointer to an image asset

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyPressedImage(laKeyPadWidget* pad,
                                                      uint32_t row,
                                                      uint32_t col,
                                                      GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laKeyPadWidget_GetKeyReleasedImage(laKeyPadWidget* pad,
                                                        uint32_t row,
                                                        uint32_t col)

  Summary:
    Gets the released icon image asset pointer for the display image for
    a key pad cell

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column

  Returns:
    GFXU_ImageAsset* - pointer to the icon image asset

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laKeyPadWidget_GetKeyReleasedImage(laKeyPadWidget* pad,
                                                               uint32_t row,
                                                               uint32_t col);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyReleasedImage(laKeyPadWidget* pad,
                                                uint32_t row,
                                                uint32_t col,
                                                GFXU_ImageAsset* img)

  Summary:
    Sets the released icon image asset pointer for a key pad cell

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    GFXU_ImageAsset* img - pointer to an image asset

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyReleasedImage(laKeyPadWidget* pad,
                                                       uint32_t row,
                                                       uint32_t col,
                                                       GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    laRelativePosition laKeyPadWidget_GetKeyImagePosition(laKeyPadWidget* pad,
                                                          uint32_t row,
                                                          uint32_t col)

  Summary:
    Gets the image position for a key pad cell

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column

  Returns:
    laRelativePosition - the image position

  Remarks:

*/
LIB_EXPORT laRelativePosition laKeyPadWidget_GetKeyImagePosition(laKeyPadWidget* pad,
                                                                 uint32_t row,
                                                                 uint32_t col);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyImagePosition(laKeyPadWidget* pad,
                                                uint32_t row,
                                                uint32_t col,
                                                laRelativePosition pos)

  Summary:


  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    laRelativePosition pos - the desired image position

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyImagePosition(laKeyPadWidget* pad,
                                                       uint32_t row,
                                                       uint32_t col,
                                                       laRelativePosition pos);

// *****************************************************************************
/* Function:
    uint32_t laKeyPadWidget_GetKeyImageMargin(laKeyPadWidget* pad,
                                              uint32_t row,
                                              uint32_t col)

  Summary:
    Gets the key pad cell image margin value

  Description:
    The image margin value is the space between the image and the text

  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column

  Returns:
    uint32_t - the margin value

  Remarks:

*/
LIB_EXPORT uint32_t laKeyPadWidget_GetKeyImageMargin(laKeyPadWidget* pad,
                                                     uint32_t row,
                                                     uint32_t col);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyImageMargin(laKeyPadWidget* pad,
                                              uint32_t row,
                                              uint32_t col,
                                              uint32_t mg)

  Summary:
    Sets the key pad cell image margin value for a given cell at row/column

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    uint32_t mg - the desired margin value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyImageMargin(laKeyPadWidget* pad,
                                                     uint32_t row,
                                                     uint32_t col,
                                                     uint32_t mg);

// *****************************************************************************
/* Function:
    laBackgroundType laKeyPadWidget_GetKeyBackgroundType(laKeyPadWidget* pad,
                                                         uint32_t row,
                                                         uint32_t col)

  Summary:
    Gets the background type for a key pad cell at row/column

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column

  Returns:
    laBackgroundType - the cell background type

  Remarks:

*/
LIB_EXPORT laBackgroundType laKeyPadWidget_GetKeyDrawBackground(laKeyPadWidget* pad,
                                                                uint32_t row,
                                                                uint32_t col);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyBackgroundType(laKeyPadWidget* pad,
                                                 uint32_t row,
                                                 uint32_t col,
                                                 laBackgroundType type)

  Summary:
    Sets the background type for a key pad cell at row/column

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    laBackgroundType type - the desired background type

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyBackgroundType(laKeyPadWidget* pad,
                                                        uint32_t row,
                                                        uint32_t col,
                                                        laBackgroundType type);

// *****************************************************************************
/* Function:
    laKeyPadCellAction laKeyPadWidget_GetKeyAction(laKeyPadWidget* pad,
                                                   uint32_t row,
                                                   uint32_t col)

  Summary:
    Gets the key pad cell action for a cell at row/column

  Description:


  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column

  Returns:
    laKeyPadCellAction - the cell action value

  Remarks:

*/
LIB_EXPORT laKeyPadCellAction laKeyPadWidget_GetKeyAction(laKeyPadWidget* pad,
                                                          uint32_t row,
                                                          uint32_t col);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyAction(laKeyPadWidget* pad,
                                         uint32_t row,
                                         uint32_t col,
                                         laKeyPadCellAction action)

  Summary:
    Sets the cell action type for a key pad cell at row/column

  Description:
    The cell action is the action that is dispatched to the Aria edit event
    system.  This event will then be received by the active edit event
    receptor widget if one exists.

  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    laKeyPadCellAction action - the desired edit action

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyAction(laKeyPadWidget* pad,
                                                uint32_t row,
                                                uint32_t col,
                                                laKeyPadCellAction action);

// *****************************************************************************
/* Function:
    laString* laKeyPadWidget_GetKeyValue(laKeyPadWidget* pad,
                                         uint32_t row,
                                         uint32_t col)

  Summary:
    Gets the edit text value for a given key pad cell.

  Description:
    This function allocates memory and returns a valid laString pointer.  The
    caller is responsible for managing the memory once this function returns.

  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column

  Returns:
    laString* - an initialized string containing a copy of the key pad cell
                edit value text

  Remarks:

*/
LIB_EXPORT laString* laKeyPadWidget_GetKeyValue(laKeyPadWidget* pad,
                                                uint32_t row,
                                                uint32_t col);

// *****************************************************************************
/* Function:
    laResult laKeyPadWidget_SetKeyValue(laKeyPadWidget* pad,
                                        uint32_t row,
                                        uint32_t col,
                                        laString str)

  Summary:
    Sets the edit value for a given key pad cell.

  Description:
    The edit value for a key pad cell is the value that is passed to the Aria
    edit event management system.  This may be different than the displayed text
    of the cell or when the cell is using a picture icon and has no display text.

    An input string that references the string table is a valid use case and the
    edit text will change as the active string table language changes.

  Parameters:
    laKeyPadWidget* pad - the widget
    uint32_t row - the indicated row
    uint32_t col - the indicated column
    laString str - the string to set the key value to

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laKeyPadWidget_SetKeyValue(laKeyPadWidget* pad,
                                               uint32_t row,
                                               uint32_t col,
                                               laString str);

// DOM-IGNORE-BEGIN
// internal use only
void _laKeyPadWidget_InvalidateBorderAreas(laKeyPadWidget* pad);
// DOM-IGNORE-END

#endif // LA_WIDGET_KEYPAD_ENABLED && LA_WIDGET_BUTTON_ENABLED
#endif /* LIBARIA_KEYPAD_H */
