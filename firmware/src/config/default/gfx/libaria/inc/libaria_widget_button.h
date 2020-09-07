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
    libaria_widget_button.h

  Summary:
    Defines a button widget
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_BUTTON_H
#define LIBARIA_BUTTON_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_BUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_string.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laButtonState_t

  Summary:
    Controls the button pressed state
*/

typedef enum laButtonState_t
{
    LA_BUTTON_STATE_UP,
    LA_BUTTON_STATE_DOWN,
    LA_BUTTON_STATE_TOGGLED
} laButtonState;

typedef struct laButtonWidget_t laButtonWidget;

// *****************************************************************************
/* Function Pointer:
    laButtonWidget_PressedEvent

  Summary:
    Button pressed event function callback type
*/
typedef void (*laButtonWidget_PressedEvent)(laButtonWidget*);

// *****************************************************************************
/* Function Pointer:
    laButtonWidget_ReleasedEvent

  Summary:
    Button released event function callback type
*/
typedef void (*laButtonWidget_ReleasedEvent)(laButtonWidget*);

// *****************************************************************************
/* Structure:
    laButtonWidget_t

  Summary:
    Implementation of a button widget.  A button is an interactive element
    that simulates a typical button with a pressed an released state.

  Description:


  Remarks:
    None.
*/
typedef struct laButtonWidget_t
{
    laWidget widget;  // base widget header

    laButtonState state; // button state
    uint8_t toggleable; // indicates if the button is toggleable

    laString text; // the string that holds the button text
    int32_t textLineSpace; //the space between lines of text (pixels)

    laHAlignment halign; // horizontal alignment of the button
    laVAlignment valign; // vertical alignment of the button

    GFXU_ImageAsset* pressedImage; // button pressed icon image
    GFXU_ImageAsset* releasedImage; // button released icon image

    laRelativePosition imagePosition; // icon position in relation to text
    uint32_t imageMargin; // distance between text and icon
    int32_t pressedOffset; // pressed text offset distance

    laButtonWidget_PressedEvent pressedEvent; // pressed event callback
    laButtonWidget_ReleasedEvent releasedEvent; // released event callback

    GFXU_ExternalAssetReader* reader; // external asset reader state

    //stores string draw parameters, primarily for multi-line text
    struct
    {
        GFX_Rect textRect;
        GFX_Rect clipRect;
        GFX_Rect drawRect;
        uint32_t newoffset;
        uint32_t end;
        GFX_Rect bounds;
        uint32_t numlines;
        uint32_t line;
        uint32_t lineY;
        uint32_t maxLines;
        GFX_Rect * lineRect;
        uint32_t * offset;
    } drawObj;    
    
} laButtonWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// DOM-IGNORE-BEGIN
// internal use only
void _laButtonWidget_Constructor(laButtonWidget* btn);
void _laButtonWidget_Destructor(laButtonWidget* btn);

void _laButtonWidget_Paint(laButtonWidget* btn);
// DOM-IGNORE-END

// *****************************************************************************
/* Function:
    laButtonWidget* laButtonWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:
    Creates a new button widget instance.  Invokes the button constructor

  Returns:
    laButtonWidget* - pointer to a new button widget instance

  Remarks:
    Caller is responsible for managing the memory allocated by this function
    until the widget is added to a valid widget tree.
*/
LIB_EXPORT laButtonWidget* laButtonWidget_New();

// *****************************************************************************
/* Function:
    laBool laButtonWidget_GetToggleable(laButtonWidget* btn)

  Summary:
    Gets the value of this button's toggle flag

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference

  Returns:
    laBool - the value of the toggle flag

*/
LIB_EXPORT laBool laButtonWidget_GetToggleable(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetToggleable(laButtonWidget* btn,
                                      laBool toggleable)

  Summary:
    Enables the toggle mode for a button.  When pressed, toggle buttons will
    stay down until pressed again.

  Description:

  Parameters:
    laButtonWidget* btn - the button to modify
    laBool toggleable - the desired togglestate

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetToggleable(laButtonWidget* btn,
                                                 laBool toggleable);

// *****************************************************************************
/* Function:
    laBool laButtonWidget_GetPressed(laButtonWidget* btn)

  Summary:
    Gets the pressed state of a button

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference

  Returns:
    laBool - the button pressed state

  Remarks:

*/
LIB_EXPORT laBool laButtonWidget_GetPressed(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetPressed(laButtonWidget* btn, laBool pressed)

  Summary:
    Sets the pressed state for a button.

  Description:

  Parameters:
    laButtonWidget* btn - the button to modify
    laBool pressed - the pressed state

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetPressed(laButtonWidget* btn,
                                              laBool pressed);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_GetText(laButtonWidget* btn, laString* str)

  Summary:
    Gets the text for a button.  If the button's string has local data then
    a duplicate of the string will be allocated.  The caller is responsible
    for managing the memory for the duplicated string.  If the button string
    is a string table reference then only the reference ID is copied.

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference
    laString* str - pointer to a string to copy the button string into

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_GetText(laButtonWidget* btn, laString* str);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetText(laButtonWidget* btn, laString str)

  Summary:
    Sets the text for a button.  If the input string has local data then
    the data will be copied into the button's local string, causing a memory
    allocation.  If the input string is a string table reference then only the
    reference will be copied.  The input string can be safely modified and
    the button string will not be affected.

  Description:

  Parameters:
    laButtonWidget* btn - the button to modify
    laString str - the string to set to the button

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetText(laButtonWidget* btn, laString str);

// *****************************************************************************
/* Function:
    int32_t laButtonWidget_GetTextLineSpace(laButtonWidget* btn)

  Summary:
    Returns the line spacing in pixels for the button text. If < 0, the
    ascent value of the string's font is used.

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference

  Returns:
    int32_t - the line spacing in pixels

  Remarks:

*/
LIB_EXPORT int32_t laButtonWidget_GetTextLineSpace(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetTextLineSpace(laButtonWidget* btn, int32_t pixels)

  Summary:
    Sets the line space in pixels of the text in the button widget. A value < 0
    sets the spacing to the ascent value of the string's font.

  Description:

  Parameters:
    laButtonWidget* btn - the button to modify
    int32_t pixels - the line space, in pixels

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetTextLineSpace(laButtonWidget* btn, int32_t pixels);

// *****************************************************************************
/* Function:
    laHAlignment laButtonWidget_GetHAlignment(laButtonWidget* btn)

  Summary:
    Gets the horizontal alignment setting for a button

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference

  Returns:
    laHAlignment - the horizontal alignment value

  Remarks:

*/
LIB_EXPORT laHAlignment laButtonWidget_GetHAlignment(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetHAlignment(laButtonWidget* btn,
                                             laHAlignment align)

  Summary:
    Sets the horizontal alignment value for a button

  Description:

  Parameters:
    laButtonWidget* btn - the button to modify
    laHAlignment align - the desired alignment value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetHAlignment(laButtonWidget* btn,
                                                 laHAlignment align);

// *****************************************************************************
/* Function:
    laVAlignment laButtonWidget_GetVAlignment(laButtonWidget* btn)

  Summary:
    Gets the vertical alignment setting for a button

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference

  Returns:
    laVAlignment - the vertical alignment setting for the button

  Remarks:

*/
LIB_EXPORT laVAlignment laButtonWidget_GetVAlignment(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetVAlignment(laButtonWidget* btn,
                                      laVAlignment align)

  Summary:
    Sets the vertical alignment for a button

  Description:

  Parameters:
    laButtonWidget* btn - the btn to modify
    laVAlignment align - the desired vertical alignment setting

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetVAlignment(laButtonWidget* btn,
                                                 laVAlignment align);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laButtonWidget_GetPressedImage(laButtonWidget* btn)

  Summary:
    Gets the pressed image asset pointer for a button

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference

  Returns:
    GFXU_ImageAsset* - the pressed asset pointer

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laButtonWidget_GetPressedImage(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetPressedImage(laButtonWidget* btn,
                                        GFXU_ImageAsset* img)

  Summary:
    Sets the image to be used as a pressed icon

  Description:

  Parameters:
    laButtonWidget* btn - the widget
    GFXU_ImageAsset* img - pointer to an image asset

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetPressedImage(laButtonWidget* btn,
                                                   GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laButtonWidget_GetReleasedImage(laButtonWidget* btn)

  Summary:
    Gets the currently used released icon

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference

  Returns:
    GFXU_ImageAsset* - the released asset pointer

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laButtonWidget_GetReleasedImage(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetReleasedImage(laButtonWidget* btn,
                                             GFXU_ImageAsset* img)

  Summary:
    Sets the image to be used as the released icon

  Description:

  Parameters:
    laButtonWidget* btn - the widget
    GFXU_ImageAsset* img - the image asset to be used

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetReleasedImage(laButtonWidget* btn,
                                                    GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    laRelativePosition laButtonWidget_GetImagePosition(laButtonWidget* btn)

  Summary:
    Gets the position of the button icon

  Description:

  Parameters:
    laButtonWidget* btn - the button to reference

  Returns:
    laRelativePosition

  Remarks:

*/
LIB_EXPORT laRelativePosition laButtonWidget_GetImagePosition(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetImagePosition(laButtonWidget* btn,
                                             laRelativePosition pos)

  Summary:
    Sets the position of the button icon

  Description:

  Parameters:
    laButtonWidget* btn - the widget
    laRelativePosition pos - the desired image position

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetImagePosition(laButtonWidget* btn,
                                                    laRelativePosition pos);

// *****************************************************************************
/* Function:
    uint32_t laButtonWidget_GetImageMargin(laButtonWidget* btn)

  Summary:
    Gets the distance between the icon and the text

  Description:

  Parameters:
    laButtonWidget* btn - the widget

  Returns:
    uint32_t - the distance value

  Remarks:

*/
LIB_EXPORT uint32_t laButtonWidget_GetImageMargin(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetImageMargin(laButtonWidget* btn,
                                           uint32_t mg)

  Summary:
    Sets the distance between the icon and text

  Description:

  Parameters:
    laButtonWidget* btn - the widget
    uint32_t - the distance value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetImageMargin(laButtonWidget* btn,
                                                  uint32_t mg);

// *****************************************************************************
/* Function:
    int32_t laButtonWidget_GetPressedOffset(laButtonWidget* btn)

  Summary:
    Gets the offset of the button internals when pressed

  Description:


  Parameters:
    laButtonWidget* btn - the widget

  Returns:
    int32_t - the distance value

  Remarks:

*/
LIB_EXPORT int32_t laButtonWidget_GetPressedOffset(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetPressedOffset(laButtonWidget* btn, int32_t offs)

  Summary:
    Sets the offset of the button internals when pressed

  Description:
    This value will be applied to all of the contents of the button
    when it is pressed.  This helps to visualize the button being
    pressed.

  Parameters:
    laButtonWidget* btn - the widget
    int32_t - the distance value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetPressedOffset(laButtonWidget* btn,
                                                    int32_t offs);

// *****************************************************************************
/* Function:
    laButtonWidget_PressedEvent laButtonWidget_GetPressedEventCallback(laButtonWidget* btn)

  Summary:
    Gets the callback associated with the button pressed event

  Description:

  Parameters:
    laButtonWidget* btn - the widget

  Returns:
    laButtonWidget_PressedEvent

  Remarks:

*/
LIB_EXPORT laButtonWidget_PressedEvent laButtonWidget_GetPressedEventCallback(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetPressedEventCallback(laButtonWidget* btn,
                                                    laButtonWidget_PressedEvent cb)

  Summary:
    Sets the pressed event callback for the button

  Description:

  Parameters:
    laButtonWidget* btn - the widget
    laButtonWidget_PressedEvent cb - a valid callback pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetPressedEventCallback(laButtonWidget* btn,
                                                           laButtonWidget_PressedEvent cb);
// *****************************************************************************
/* Function:
    laButtonWidget_ReleasedEvent laButtonWidget_GetReleasedEventCallback(laButtonWidget* btn)

  Summary:
    Gets the callback for the button released event

  Description:

  Parameters:
    laButtonWidget* btn - the widget

  Returns:
    laButtonWidget_ReleasedEvent

  Remarks:

*/
LIB_EXPORT laButtonWidget_ReleasedEvent laButtonWidget_GetReleasedEventCallback(laButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laButtonWidget_SetReleasedEventCallback(laButtonWidget* btn,
                                                     laButtonWidget_ReleasedEvent cb)

  Summary:
    Sets the callback for the button released event

  Description:

  Parameters:
    laButtonWidget* btn - the widget
    laButtonWidget_ReleasedEvent cb - a valid callback pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laButtonWidget_SetReleasedEventCallback(laButtonWidget* btn,
                                                            laButtonWidget_ReleasedEvent cb);

// DOM-IGNORE-BEGIN
// internal use only
void _laButtonWidget_GetImageRect(laButtonWidget* btn,
                                  GFX_Rect* imgRect,
                                  GFX_Rect* imgSrcRect);

void _laButtonWidget_GetTextRect(laButtonWidget* btn,
                                 GFX_Rect* textRect,
                                 GFX_Rect* drawRect);

void _laButtonWidget_InvalidateBorderAreas(laButtonWidget* btn);
// DOM-IGNORE-END

#endif /* LA_BUTTON_WIDGET_ENABLED */
#endif /* LIBARIA_BUTTON_H */

