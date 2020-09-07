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
    libaria_widget_textfield.h

  Summary:


  Description:
    This module implements text field widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_TEXTFIELD_H
#define LIBARIA_TEXTFIELD_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_TEXTFIELD_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_editwidget.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laTextFieldWidget_t laTextFieldWidget;

// *****************************************************************************
/* Function Pointer:
    laTextFieldWidget_TextChangedCallback

  Summary:
    Text changed event function callback type
*/
typedef void (*laTextFieldWidget_TextChangedCallback)(laTextFieldWidget*);

// *****************************************************************************
/* Function Pointer:
    laTextFieldWidget_FocusChangedCallback

  Summary:
    Focus changed event function callback type
*/
typedef void (*laTextFieldWidget_FocusChangedCallback)(laTextFieldWidget*, laBool);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laTextFieldWidget_t

  Summary:
    Implementation of a text field widget.

  Description:
    A text field widget is a widget that is capable of displaying a single line
    of editable text.  This widget is capable of receiving edit events from the
    Aria edit event system.  It can also display a blinking cursor.

  Remarks:
    None.
*/
typedef struct laTextFieldWidget_t
{
    laEditWidget editWidget; // edit widget base class

    laString text; // the text to edit

    laHAlignment halign; // horizontal alignment

    uint32_t cursorPos; // current cursor position
    uint32_t cursorDelay; // cursor blink delay
    uint32_t cursorTime; // current cursor tick counter
    laBool cursorEnable; // cursor enabled flag
    laBool cursorVisible; // cursor visbility flag
    laBool clearOnFirstEdit; // needs clear on first edit

    laTextFieldWidget_TextChangedCallback textChangedEvent; // text changed event
    laTextFieldWidget_FocusChangedCallback focusChangedEvent; // focus changed event

    GFXU_ExternalAssetReader* reader; // asset reader
} laTextFieldWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laTextFieldWidget_Constructor(laTextFieldWidget* txt);
void _laTextFieldWidget_Destructor(laTextFieldWidget* txt);

laWidgetUpdateState _laTextFieldWidget_Update(laTextFieldWidget* img, uint32_t dt);
void _laTextFieldWidget_Paint(laTextFieldWidget* txt);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laTextFieldWidget* laTextFieldWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:


  Parameters:

  Returns:
    laTextFieldWidget*

  Remarks:

*/
LIB_EXPORT laTextFieldWidget* laTextFieldWidget_New();

// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_GetText(laTextFieldWidget* txt, laString* str)

  Summary:
    Gets the text value for the box.

  Description:
    This function allocates memory and initializes the input string pointer.  The
    caller is responsible for managing the memory once this function returns.

  Parameters:
    laTextFieldWidget* txt - the widget
    laString* str - a pointer to an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_GetText(laTextFieldWidget* txt, laString* str);

// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_SetText(laTextFieldWidget* txt, laString str)

  Summary:
    Sets the text value for the box.

  Description:
    This function copies the contents of the input string into its internal
    string buffer.  The input string can then be freed or altered without
    affecting the label's internal string value.

  Parameters:
    laTextFieldWidget* txt - the widget
    laString str - an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_SetText(laTextFieldWidget* txt, laString str);

// *****************************************************************************
/* Function:
    laHAlignment laTextFieldWidget_GetAlignment(laTextFieldWidget* txt)

  Summary:
    Gets the text horizontal alignment value.

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget

  Returns:
    laHAlignment - the horizontal alignment value

  Remarks:

*/
LIB_EXPORT laHAlignment laTextFieldWidget_GetAlignment(laTextFieldWidget* txt);

// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_SetAlignment(laTextFieldWidget* txt,
                                            laHAlignment align)

  Summary:
    Sets the text horizontal alignment value

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget
    laHAlignment - the horizontal alignment value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_SetAlignment(laTextFieldWidget* txt,
                                                   laHAlignment align);

// *****************************************************************************
/* Function:
    uint32_t laTextFieldWidget_GetCursorDelay(laTextFieldWidget* txt)

  Summary:
    Gets the current cursor delay.

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget

  Returns:
    uint32_t - the current delay value

  Remarks:

*/
LIB_EXPORT uint32_t laTextFieldWidget_GetCursorDelay(laTextFieldWidget* txt);

// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_SetCursorDelay(laTextFieldWidget* txt,
                                          uint32_t dt)

  Summary:
    Sets the cursor delay value

  Description:
    This value is typically expressed in milliseconds

  Parameters:
    laTextFieldWidget* txt - the widget
    uint32_t dt - the cursor delay value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_SetCursorDelay(laTextFieldWidget* txt,
                                                     uint32_t dt);

// *****************************************************************************
/* Function:
    laBool laTextFieldWidget_GetCursorEnabled(laTextFieldWidget* txt)

  Summary:
    Gets the cursor enabled value

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget

  Returns:
    laBool - the cursor enabled flag value

  Remarks:

*/
LIB_EXPORT laBool laTextFieldWidget_GetCursorEnabled(laTextFieldWidget* txt);

// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_SetCursorEnabled(laTextFieldWidget* txt,
                                                laBool en)

  Summary:
    Sets the cursor enabled value flag

  Description:
    The cursor enabled flag controls whether the cursor will display or not

  Parameters:
    laTextFieldWidget* txt - the widget
    laBool en - the desired flag state

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_SetCursorEnabled(laTextFieldWidget* txt,
                                                       laBool en);

// *****************************************************************************
/* Function:
    uint32_t laTextFieldWidget_GetCursorPosition(laTextFieldWidget* txt)

  Summary:
    Gets the current edit cursor position

  Description:
    This cursor will appear to the left of the character at index of the string

  Parameters:
    laTextFieldWidget* txt - the widget

  Returns:
    uint32_t - the index of the cursor

  Remarks:

*/
LIB_EXPORT uint32_t laTextFieldWidget_GetCursorPosition(laTextFieldWidget* txt);

// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_SetCursorPosition(laTextFieldWidget* txt,
                                                 uint32_t pos)

  Summary:
    Sets the position of the cursor

  Description:
    The cursor will appear to the left of the character at pos

  Parameters:
    laTextFieldWidget* txt - the widget
    uint32_t pos - the position of the cursor in character indices

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_SetCursorPosition(laTextFieldWidget* txt,
                                                        uint32_t pos);

// *****************************************************************************
/* Function:
    laTextFieldWidget_TextChangedCallback laTextFieldWidget_GetTextChangedEventCallback(laTextFieldWidget* txt)

  Summary:
    Gets the current text changed event callback pointer

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget

  Returns:
    laTextFieldWidget_TextChangedCallback - a valid pointer or NULL

  Remarks:

*/
LIB_EXPORT laTextFieldWidget_TextChangedCallback laTextFieldWidget_GetTextChangedEventCallback(laTextFieldWidget* txt);

// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_SetTextChangedEventCallback(laTextFieldWidget* txt,
                                                           laTextFieldWidget_TextChangedCallback cb)

  Summary:
    Sets the text changed event callback pointer

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget
    laTextFieldWidget_TextChangedCallback - a valid pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_SetTextChangedEventCallback(laTextFieldWidget* txt,
                                                                  laTextFieldWidget_TextChangedCallback cb);
// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_SetClearOnFirstEdit(laTextFieldWidget* txt,
                                                   laBool clear)

  Summary:
    Sets the flag to indicate that the text field will be cleared on first edit.

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget
    laBool clear - the desired flag state

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_SetClearOnFirstEdit(laTextFieldWidget* txt, laBool clear);

// *****************************************************************************
/* Function:
    laTextFieldWidget_FocusChangedCallback laTextFieldWidget_GetFocusChangedEventCallback(laTextFieldWidget* txt)

  Summary:
    Gets the current focus changed event callback pointer

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget

  Returns:
    laTextFieldWidget_FocusChangedCallback - a valid pointer or NULL

  Remarks:

*/
LIB_EXPORT laTextFieldWidget_FocusChangedCallback laTextFieldWidget_GetFocusChangedEventCallback(laTextFieldWidget* txt);

// *****************************************************************************
/* Function:
    laResult laTextFieldWidget_SetFocusChangedEventCallback(laTextFieldWidget* txt,
                                                           laTextFieldWidget_FocusChangedCallback cb)

  Summary:
    Sets the focus changed event callback pointer

  Description:


  Parameters:
    laTextFieldWidget* txt - the widget
    laTextFieldWidget_FocusChangedCallback - a valid pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTextFieldWidget_SetFocusChangedEventCallback(laTextFieldWidget* txt,
                                                       laTextFieldWidget_FocusChangedCallback cb);

// DOM-IGNORE-BEGIN
// internal use only
void _laTextFieldWidget_TouchDownEvent(laTextFieldWidget* txt, laInput_TouchDownEvent* evt);
void _laTextFieldWidget_TouchUpEvent(laTextFieldWidget* txt, laInput_TouchUpEvent* evt);
void _laTextFieldWidget_TouchMovedEvent(laTextFieldWidget* txt, laInput_TouchMovedEvent* evt);

void _laTextFieldWidget_FocusGained(laWidget* widget);
void _laTextFieldWidget_FocusLost(laWidget* widget);

void _laTextFieldWidget_GetTextRect(laTextFieldWidget* txt,
                                    GFX_Rect* textRect,
                                    GFX_Rect* drawRect);

void _laTextFieldWidget_GetCursorRect(laTextFieldWidget* txt, GFX_Rect* cursorRect);
// DOM-IGNORE-END

#endif // LA_TEXTFIELD_WIDGET_ENABLED
#endif /* LIBARIA_TEXTFIELD_H */
