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
    libaria_widget_window.h

  Summary:
    Window Widget

  Description:
    This module implements window container widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_WINDOW_H
#define LIBARIA_WINDOW_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_WINDOW_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laWindowWidget_t laWindowWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    laWindowWidget_t

  Summary:
    Implementation of a window widget struct

  Description:
    A window widget is an extension of a basic panel.  It adds a title bar
    with text and an icon.

  Remarks:
    None.
*/
typedef struct laWindowWidget_t
{
    laWidget widget; // base widget class

    laString title; // title text

    GFXU_ImageAsset* icon; // title icon
    uint32_t iconMargin; // title icon margin

    struct
    {
        GFX_Rect barRect;
    } paintData;

    GFXU_ExternalAssetReader* reader; // asset reader
} laWindowWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laWindowWidget_Constructor(laWindowWidget* win);
void _laWindowWidget_Destructor(laWindowWidget* win);

void _laWindowWidget_Paint(laWindowWidget* win);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laWindowWidget* laWindowWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:


  Parameters:

  Returns:
    laWindowWidget*

  Remarks:

*/
LIB_EXPORT laWindowWidget* laWindowWidget_New();

// *****************************************************************************
/* Function:
    laResult laWindowWidget_GetTitle(laWindowWidget* win, laString* str)

  Summary:
    Gets the title text for this window.

  Description:
    This function allocates memory and initializes the input string pointer.  The
    caller is responsible for managing the memory once this function returns.

  Parameters:
    laWindowWidget* win - the widget
    laString* str - a pointer to an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWindowWidget_GetTitle(laWindowWidget* win, laString* str);

// *****************************************************************************
/* Function:
    laResult laWindowWidget_SetTitle(laWindowWidget* win, laString str)

  Summary:
    Sets the title text for the window.

  Description:
    This function copies the contents of the input string into its internal
    string buffer.  The input string can then be freed or altered without
    affecting the label's internal string value.

  Parameters:
    laWindowWidget* win - the widget
    laString str - an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWindowWidget_SetTitle(laWindowWidget* win, laString str);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laWindowWidget_GetIcon(laWindowWidget* win)

  Summary:
    Gets the currently used window icon

  Description:


  Parameters:
    laWindowWidget* win - the widget

  Returns:
    GFXU_ImageAsset*

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laWindowWidget_GetIcon(laWindowWidget* win);

// *****************************************************************************
/* Function:
    laResult laWindowWidget_SetIcon(laWindowWidget* win,
                                    GFXU_ImageAsset* img)

  Summary:
    Sets the image to be used as a window icon

  Description:


  Parameters:
    laWindowWidget* win - the widget
    GFXU_ImageAsset* - pointer to an image asset

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWindowWidget_SetIcon(laWindowWidget* win,
                                           GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    uint32_t laWindowWidget_GetIconMargin(laWindowWidget* win)

  Summary:
    Gets the current image icon margin

  Description:


  Parameters:
    laWindowWidget* win - the widget

  Returns:
    uint32_t - the icon margin

  Remarks:

*/
LIB_EXPORT uint32_t laWindowWidget_GetIconMargin(laWindowWidget* win);

// *****************************************************************************
/* Function:
    laResult laWindowWidget_SetIconMargin(laWindowWidget* win, uint32_t mg)

  Summary:
    Sets the image icon margin

  Description:


  Parameters:
    laWindowWidget* win - the widget
    uint32_t mg - the image icon margin value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWindowWidget_SetIconMargin(laWindowWidget* win, uint32_t mg);

// DOM-IGNORE-BEGIN
// internal use only
void laWindowWidget_GetTitleBarRect(laWindowWidget* win, GFX_Rect* barRect);
void laWindowWidget_GetIconRect(laWindowWidget* win, GFX_Rect* imgRect, GFX_Rect* imgSrcRect);
void laWindowWidget_GetTextRect(laWindowWidget* win, GFX_Rect* textRect, GFX_Rect* drawRect);
// DOM-IGNORE-END

#endif // LA_WINDOW_WIDGET_ENABLED
#endif /* LIBARIA_WINDOW_H */