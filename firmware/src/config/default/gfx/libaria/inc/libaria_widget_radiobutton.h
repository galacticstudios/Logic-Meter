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
    libaria_widget_radiobutton.h

  Summary:
    

  Description:
    This module implements radio button widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_RADIOBUTTON_H
#define LIBARIA_RADIOBUTTON_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_RADIOBUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_radiobutton_group.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laRadioButtonWidget_t laRadioButtonWidget;
typedef struct laRadioButtonGroup_t laRadioButtonGroup;

// *****************************************************************************
/* Function Pointer:
    laRadioButtonWidget_SelectedEvent

  Summary:
    Radio button selected function callback type
*/
typedef void (*laRadioButtonWidget_SelectedEvent)(laRadioButtonWidget*);

// *****************************************************************************
/* Function Pointer:
    laRadioButtonWidget_DeselectedEvent

  Summary:
    Radio button deselected function callback type
*/
typedef void (*laRadioButtonWidget_DeselectedEvent)(laRadioButtonWidget*);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laRadioButtonWidget_t

  Summary:
    Implementation of a radio button widget struct

  Description:
    A radio button is similar to a checkbox widget in that it has an on and
    off state.  It is further capable of being added to a radio button group.
    This group provides a mutually exclusive selection capability so that
    only one radio button may be selected at any one time.

  Remarks:
    None.
*/
typedef struct laRadioButtonWidget_t
{
    laWidget widget; // widget base class

    laBool selected; // indicates if the radio button is selected

    laString text; // radio button text

    laHAlignment halign; // horizontal alignment
    laVAlignment valign; // vertical alignment

    GFXU_ImageAsset* selectedImage; // button custom selected image
    GFXU_ImageAsset* unselectedImage; // buton custom unselected image
    
    laRelativePosition imagePosition; // image icon relative position
    uint32_t imageMargin; // image margin
    uint32_t circleButtonSize; // size of radio circle button in pixels

    laRadioButtonWidget_SelectedEvent selectedEvent; // button selected event callback
    laRadioButtonWidget_DeselectedEvent deselectedEvent; // button deselected event callback

    struct
    {
        uint8_t enabled;
    } paintData;
    
    GFXU_ExternalAssetReader* reader; // asset reader

    laRadioButtonGroup* group; // radio button group
} laRadioButtonWidget;


// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laRadioButtonWidget* laRadioButtonWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:
    

  Parameters:
    
  Returns:
    laProgressBarWidget*
    
  Remarks:
    
*/
LIB_EXPORT laRadioButtonWidget* laRadioButtonWidget_New();

// *****************************************************************************
/* Function:
    laRadioButtonGroup* laRadioButtonWidget_GetGroup(laRadioButtonWidget* btn)

  Summary:
    Returns the pointer to the currently set radio button group.

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
    
  Returns:
    laRadioButtonGroup* - the currently assigned radio button group
    
  Remarks:
    
*/
LIB_EXPORT laRadioButtonGroup* laRadioButtonWidget_GetGroup(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laBool laRadioButtonWidget_GetSelected(laRadioButtonWidget* btn)

  Summary:
    Returns true if this radio button is currently selected

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
    
  Returns:
    laBool - true if this button is currently selected
    
  Remarks:
    
*/
LIB_EXPORT laBool laRadioButtonWidget_GetSelected(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetSelected(laRadioButtonWidget* btn)

  Summary:
    Sets this button as selected.

  Description:
    If this button belongs to a radio button group then this function will
    potentially unselect another button and become selected.

  Parameters:
    laRadioButtonWidget* btn - the widget
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/
LIB_EXPORT laResult laRadioButtonWidget_SetSelected(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_GetText(laRadioButtonWidget* btn, 
                                         laString* str)

  Summary:
    Gets the text value for the button.

  Description:
    This function allocates memory and initializes the input string pointer.  The
    caller is responsible for managing the memory once this function returns.

  Parameters:
    laRadioButtonWidget* btn - the widget
    laString* str - a pointer to an laString object
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/
LIB_EXPORT laResult laRadioButtonWidget_GetText(laRadioButtonWidget* btn, 
                                                laString* str);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetText(laRadioButtonWidget* btn,
                                         laString str)

  Summary:
    Sets the text value for the button.

  Description:
    This function copies the contents of the input string into its internal
    string buffer.  The input string can then be freed or altered without 
    affecting the label's internal string value.

  Parameters:
    laRadioButtonWidget* btn - the widget
    laString str - an laString object
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/                                             
LIB_EXPORT laResult laRadioButtonWidget_SetText(laRadioButtonWidget* btn,
                                                laString str);

// *****************************************************************************
/* Function:
    laHAlignment laRadioButtonWidget_GetHAlignment(laRadioButtonWidget* btn)

  Summary:
    Gets the horizontal alignment setting for a button

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    laHAlignment - the horizontal alignment value
    
  Remarks:
    
*/    
LIB_EXPORT laHAlignment laRadioButtonWidget_GetHAlignment(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetHAlignment(laRadioButtonWidget* btn,
                                               laHAlignment align)

  Summary:
    Sets the horizontal alignment value for a button

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
    laHAlignment align - the desired alignment value
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadioButtonWidget_SetHAlignment(laRadioButtonWidget* btn,
                                                      laHAlignment align);

// *****************************************************************************
/* Function:
    laVAlignment laRadioButtonWidget_GetVAlignment(laRadioButtonWidget* btn)

  Summary:
    Sets the vertical alignment for a button

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    laVAlignment align - the desired vertical alignment setting
    
  Remarks:
    
*/    
LIB_EXPORT laVAlignment laRadioButtonWidget_GetVAlignment(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetVAlignment(laRadioButtonWidget* btn,
                                               laVAlignment align)

  Summary:
    Sets the vertical alignment for a button

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
    laVAlignment align - the desired vertical alignment setting
    
  Returns:
    laResult - the operation result
        
  Remarks:
    
*/    
LIB_EXPORT laResult laRadioButtonWidget_SetVAlignment(laRadioButtonWidget* btn,
                                                      laVAlignment align);                                                    
// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laRadioButtonWidget_GetSelectedImage(laRadioButtonWidget* btn)

  Summary:
    Gets the selected image asset pointer for a button

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    GFXU_ImageAsset* - the selected asset pointer
    
  Remarks:
    
*/    
LIB_EXPORT GFXU_ImageAsset* laRadioButtonWidget_GetSelectedImage(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetSelectedImage(laRadioButtonWidget* btn,
                                                  GFXU_ImageAsset* img)

  Summary:
    Sets the image to be used as a selected icon

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
    GFXU_ImageAsset* img - the desired image asset pointer or NULL
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadioButtonWidget_SetSelectedImage(laRadioButtonWidget* btn,
                                                         GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laRadioButtonWidget_GetUnselectedImage(laRadioButtonWidget* btn)

  Summary:
    Gets the image asset pointer currently used as the unselected icon

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    GFXU_ImageAsset* - the selected asset pointer
    
  Remarks:
    
*/    
LIB_EXPORT GFXU_ImageAsset* laRadioButtonWidget_GetUnselectedImage(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetUnselectedImage(laRadioButtonWidget* btn,
                                                    GFXU_ImageAsset* img)

  Summary:
    Sets the asset pointer for the radio button's unselected image icon

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
    GFXU_ImageAsset* img - the desired image asset pointer or NULL
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadioButtonWidget_SetUnselectedImage(laRadioButtonWidget* btn,
                                                           GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    laRelativePosition laRadioButtonWidget_GetImagePosition(laRadioButtonWidget* btn)

  Summary:
    Gets the current image position setting for the radio button

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    laRelativePosition - the current image relative position
    
  Remarks:
    
*/                                                           
LIB_EXPORT laRelativePosition laRadioButtonWidget_GetImagePosition(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetImagePosition(laRadioButtonWidget* btn,
                                                  laRelativePosition pos)

  Summary:
    Sets the image relative position setting for the radio button

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
    laRelativePosition pos - the desired image position
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadioButtonWidget_SetImagePosition(laRadioButtonWidget* btn,
                                                         laRelativePosition pos);

// *****************************************************************************
/* Function:
    uint16_t laRadioButtonWidget_GetImageMargin(laRadioButtonWidget* btn)

  Summary:
    Gets the distance between the icon and the text

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    uint32_t - the distance value
    
  Remarks:
    
*/    
LIB_EXPORT uint32_t laRadioButtonWidget_GetImageMargin(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetImageMargin(laRadioButtonWidget* btn,
                                                uint32_t mg)

  Summary:
    Sets the distance between the icon and text

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
    uint32_t mg - the distance value
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadioButtonWidget_SetImageMargin(laRadioButtonWidget* btn,
                                                       uint32_t mg);

// *****************************************************************************
/* Function:
    laRadioButtonWidget_SelectedEvent laRadioButtonWidget_GetSelectedEventCallback(laRadioButtonWidget* btn)

  Summary:
    Gets the current radio button selected event callback

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    laRadioButtonWidget_SelectedEvent - a valid callback pointer or NULL
    
  Remarks:
    
*/    
LIB_EXPORT laRadioButtonWidget_SelectedEvent laRadioButtonWidget_GetSelectedEventCallback(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetSelectedEventCallback(laRadioButtonWidget* btn,
                                                          laRadioButtonWidget_SelectedEvent cb)

  Summary:
    Sets the radio button selected event callback

  Description:
    This callback is called when the radio button becomes selected

  Parameters:
    laRadioButtonWidget* btn - the widget
    laRadioButtonWidget_SelectedEvent - a valid callback pointer or NULL
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadioButtonWidget_SetSelectedEventCallback(laRadioButtonWidget* btn,
                                                                laRadioButtonWidget_SelectedEvent cb);

// *****************************************************************************
/* Function:
    laRadioButtonWidget_DeselectedEvent laRadioButtonWidget_GetDeselectedEventCallback(laRadioButtonWidget* btn)

  Summary:
    Gets the current radio button deselected event callback

  Description:
    

  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    laRadioButtonWidget_DeselectedEvent - a valid callback pointer or NULL
    
  Remarks:
    
*/    
LIB_EXPORT laRadioButtonWidget_DeselectedEvent laRadioButtonWidget_GetDeselectedEventCallback(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetDeselectedEventCallback(laRadioButtonWidget* btn,
                                                            laRadioButtonWidget_DeselectedEvent cb)

  Summary:
    Sets the deselected callback pointer

  Description:
    This callback is called when this radio button is deselected

  Parameters:
    laRadioButtonWidget* btn - the widget
    laRadioButtonWidget_DeselectedEvent - a valid callback pointer or NULL
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadioButtonWidget_SetDeselectedEventCallback(laRadioButtonWidget* btn,
                                                                   laRadioButtonWidget_DeselectedEvent cb);

// *****************************************************************************
/* Function:
    LIB_EXPORT uint32_t laRadioButtonWidget_GetCircleButtonSize(laRadioButtonWidget* btn)

  Summary:
    Gets the diameter/size of the default circle button

  Description:
    
  Parameters:
    laRadioButtonWidget* btn - the widget
        
  Returns:
    uint32_t size - the size of the default circle button
    
  Remarks:
    
*/
LIB_EXPORT uint32_t laRadioButtonWidget_GetCircleButtonSize(laRadioButtonWidget* btn);

// *****************************************************************************
/* Function:
    laResult laRadioButtonWidget_SetCircleButtonSize(laRadioButtonWidget* btn,
                                                     uint32_t size)

  Summary:
    Sets the size of the default circle button

  Description:

  Parameters:
    laRadioButtonWidget* btn - the widget
    uint32_t size - the diameter of the circle, in pixels
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/  
LIB_EXPORT laResult laRadioButtonWidget_SetCircleButtonSize(laRadioButtonWidget* btn,
                                                            uint32_t size);

// DOM-IGNORE-BEGIN
// internal use only
void _laRadioButtonWidget_Select(laRadioButtonWidget* btn);
void _laRadioButtonWidget_Deselect(laRadioButtonWidget* btn);

// internal use only
void _laRadioButtonWidget_GetImageRect(laRadioButtonWidget* btn,
									   GFX_Rect* imgRect,
									   GFX_Rect* imgSrcRect);

void _laRadioButtonWidget_GetTextRect(laRadioButtonWidget* btn,
								      GFX_Rect* textRect,
								      GFX_Rect* drawRect);
// DOM-IGNORE-END

#endif // LA_RADIOBUTTON_WIDGET_ENABLED
#endif /* LIBARIA_RADIOBUTTON_H */
