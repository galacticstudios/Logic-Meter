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


  Description:
    This module implements button widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_CHECKBOX_H
#define LIBARIA_CHECKBOX_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_CHECKBOX_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laCheckBoxWidget_t laCheckBoxWidget;

// *****************************************************************************
/* Function Pointer:
    laCheckBoxWidget_CheckedEvent

  Summary:
    Checkbox checked event function callback type
*/
typedef void (*laCheckBoxWidget_CheckedEvent)(laCheckBoxWidget*);

// *****************************************************************************
/* Function Pointer:
    laCheckBoxWidget_UncheckedEvent

  Summary:
    Checkbox unchecked event function callback type
*/
typedef void (*laCheckBoxWidget_UncheckedEvent)(laCheckBoxWidget*);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    laCheckBoxWidget_t

  Summary:
    Implementation of a checkbox widget.

  Description:
    A check box widget contains an interactive two-state box indicating on or
    off.  The check box may also contain descriptive text.  Custom images for
    the check box may be used in place of the default box graphic.

  Remarks:
    None.
*/
typedef struct laCheckBoxWidget_t
{
    laWidget widget; // base class properties

    laBool checked; // the state of the box

    laString text; // the text of the box

    laHAlignment halign; // the horizontal alignment of the box contents
    laVAlignment valign; // the vertical alignment of the box contents

    GFXU_ImageAsset* checkedImage; // pointer to a custom image to use for the
                                   // checked image
    GFXU_ImageAsset* uncheckedImage; // pointer to a custom image to use for the
                                     // unchecked image

    laRelativePosition imagePosition; // postition of the image relative to the text
                                      // of the box
    uint32_t imageMargin; // the distance between the image and the text

    GFXU_ExternalAssetReader* reader; // an external asset reader pointer

    laCheckBoxWidget_CheckedEvent checkedEvent; // callback for checked events
    laCheckBoxWidget_CheckedEvent uncheckedEvent; // callback for unchecked events
} laCheckBoxWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laCheckBoxWidget_Constructor(laCheckBoxWidget* cbox);
void _laCheckBoxWidget_Destructor(laCheckBoxWidget* cbox);

void _laCheckBoxWidget_Paint(laCheckBoxWidget* cbox);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laCheckBoxWidget* laCheckBoxWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:
    void

  Returns:
    laCheckBoxWidget*

  Remarks:

*/
LIB_EXPORT laCheckBoxWidget* laCheckBoxWidget_New();

// *****************************************************************************
/* Function:
    laBool laCheckBoxWidget_GetChecked(laCheckBoxWidget* cbox)

  Summary:
    Gets the checked state of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    laBool - the checked flag value

  Remarks:

*/
LIB_EXPORT laBool laCheckBoxWidget_GetChecked(laCheckBoxWidget* cbox);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetChecked(laCheckBoxWidget* cbox,
                                         laBool checked)

  Summary:
    Sets the checked state of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    laBool checked - the desired checked value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetChecked(laCheckBoxWidget* cbox,
                                                laBool checked);

// *****************************************************************************
/* Function:
    laHAlignment laCheckBoxWidget_GetHAlignment(laCheckBoxWidget* cbox)

  Summary:
    Gets the horizontal alignment of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    laHAlignment - the current halign value

  Remarks:

*/
LIB_EXPORT laHAlignment laCheckBoxWidget_GetHAlignment(laCheckBoxWidget* cbox);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetHAlignment(laCheckBoxWidget* cbox,
                                            laHAlignment align)

  Summary:
    Sets the horizontal alignment of the check box.

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    laHAlignment align - the desired halign value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetHAlignment(laCheckBoxWidget* cbox,
                                                   laHAlignment align);

// *****************************************************************************
/* Function:
    laVAlignment laCheckBoxWidget_GetVAlignment(laCheckBoxWidget* cbox)

  Summary:
    Gets the vertical alignment of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    laVAlignment

  Remarks:

*/
LIB_EXPORT laVAlignment laCheckBoxWidget_GetVAlignment(laCheckBoxWidget* cbox);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetVAlignment(laCheckBoxWidget* cbox,
                                            laVAlignment align)

  Summary:
    Sets the vertical alignment of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    laVAlignment align - the valign value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetVAlignment(laCheckBoxWidget* cbox,
                                                   laVAlignment align);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laCheckBoxWidget_GetCheckedImage(laCheckBoxWidget* btn)

  Summary:
    Gets the checked image of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    GFXU_ImageAsset* - the current checked image asset pointer

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laCheckBoxWidget_GetCheckedImage(laCheckBoxWidget* btn);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetCheckedImage(laCheckBoxWidget* btn,
                                              GFXU_ImageAsset* img)

  Summary:
    Sets the checked image of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    GFXU_ImageAsset* img - the desired checked image asset pointer

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetCheckedImage(laCheckBoxWidget* btn,
                                                     GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laCheckBoxWidget_GetUncheckedImage(laCheckBoxWidget* btn)

  Summary:
    Gets the unchecked image of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    GFXU_ImageAsset* - the current unchecked image asset pointer

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laCheckBoxWidget_GetUncheckedImage(laCheckBoxWidget* btn);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetUncheckedImage(laCheckBoxWidget* btn,
                                                GFXU_ImageAsset* img)

  Summary:
    Sets the unchecked image of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    GFXU_ImageAsset* img - the desired unchecked image asset pointer

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetUncheckedImage(laCheckBoxWidget* btn,
                                                       GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    laRelativePosition laCheckBoxWidget_GetImagePosition(laCheckBoxWidget* btn)

  Summary:
    Gets the image position of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    laRelativePosition - the current image position value

  Remarks:

*/
LIB_EXPORT laRelativePosition laCheckBoxWidget_GetImagePosition(laCheckBoxWidget* btn);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetImagePosition(laCheckBoxWidget* btn,
                                               laRelativePosition pos)

  Summary:
    Sets the image position of the check box

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    laRelativePosition pos - the desired image position value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetImagePosition(laCheckBoxWidget* btn,
                                                      laRelativePosition pos);

// *****************************************************************************
/* Function:
    uint32_t laCheckBoxWidget_GetImageMargin(laCheckBoxWidget* btn)

  Summary:
    Gets the distance between the image and the text

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    uint32_t - the current image margin value

  Remarks:

*/
LIB_EXPORT uint32_t laCheckBoxWidget_GetImageMargin(laCheckBoxWidget* btn);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetImageMargin(laCheckBoxWidget* btn, uint32_t mg)

  Summary:
    Sets the distance between the image and the text

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    uint32_t mg - the desired image margin value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetImageMargin(laCheckBoxWidget* btn,
                                                    uint32_t mg);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_GetText(laCheckBoxWidget* cbox, laString* str)

  Summary:
    Gets a copy of the checkbox text.  If the text has local data the data will
    be duplicated.  The caller is responsible for managing the memory
    as appropriate.

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    str - pointer to an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_GetText(laCheckBoxWidget* cbox, laString* str);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetText(laCheckBoxWidget* cbox, laString str)

  Summary:
    Sets the checkbox text to the input string.  If the string has local data
    the data will be duplicated and copied to the checkboxes internal string.

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetText(laCheckBoxWidget* cbox,
                                             laString str);

// *****************************************************************************
/* Function:
    laCheckBoxWidget_CheckedEvent laCheckBoxWidget_GetCheckedEventCallback(laCheckBoxWidget* cbox)

  Summary:
    Gets the checked event callback

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    laCheckBoxWidget_CheckedEvent - a valid callback pointer or NULL

  Remarks:

*/
LIB_EXPORT laCheckBoxWidget_CheckedEvent laCheckBoxWidget_GetCheckedEventCallback(laCheckBoxWidget* cbox);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetCheckedEventCallback(laCheckBoxWidget* cbox,
                                                      laCheckBoxWidget_CheckedEvent cb)

  Summary:
    Sets the checked event callback

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    laCheckBoxWidget_CheckedEvent cb - a valid callback pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetCheckedEventCallback(laCheckBoxWidget* cbox,
                                                             laCheckBoxWidget_CheckedEvent cb);

// *****************************************************************************
/* Function:
    laCheckBoxWidget_UncheckedEvent laCheckBoxWidget_GetUncheckedEventCallback(laCheckBoxWidget* cbox)

  Summary:
    Gets the unchecked event callback

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget

  Returns:
    laCheckBoxWidget_UncheckedEvent - a valid callback pointer or NULL

  Remarks:

*/
LIB_EXPORT laCheckBoxWidget_UncheckedEvent laCheckBoxWidget_GetUncheckedEventCallback(laCheckBoxWidget* cbox);

// *****************************************************************************
/* Function:
    laResult laCheckBoxWidget_SetUncheckedEventCallback(laCheckBoxWidget* cbox,
                                                        laCheckBoxWidget_UncheckedEvent cb)

  Summary:
    Sets the unchecked event callback

  Description:


  Parameters:
    laCheckBoxWidget* cbox - the widget
    laCheckBoxWidget_UncheckedEvent cb - a valid callback pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCheckBoxWidget_SetUncheckedEventCallback(laCheckBoxWidget* cbox,
                                                               laCheckBoxWidget_UncheckedEvent cb);

// DOM-IGNORE-BEGIN
// internal use only
void _laCheckBoxWidget_GetImageRect(laCheckBoxWidget* widget,
                                    GFX_Rect* imgRect,
                                    GFX_Rect* imgSrcRect);

void _laCheckBoxWidget_GetTextRect(laCheckBoxWidget* widget,
                                   GFX_Rect* textRect,
                                   GFX_Rect* drawRect);

void _laCheckBoxWidget_TouchDownEvent(laCheckBoxWidget* widget, laInput_TouchDownEvent* evt);
void _laCheckBoxWidget_TouchUpEvent(laCheckBoxWidget* widget, laInput_TouchUpEvent* evt);
void _laCheckBoxWidget_TouchMovedEvent(laCheckBoxWidget* widget, laInput_TouchMovedEvent* evt);
// DOM-IGNORE-END

#endif // LA_CHECKBOX_WIDGET_ENABLED
#endif /* LIBARIA_BUTTON_H */
