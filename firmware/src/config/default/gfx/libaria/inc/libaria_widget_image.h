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
    libaria_widget_image.h

  Summary:


  Description:
    This module implements image widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_IMAGE_H
#define LIBARIA_IMAGE_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_IMAGE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

typedef struct laImageWidget_t laImageWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************
typedef void (*laImageWidget_DrawEventCallback)(laImageWidget* );
// *****************************************************************************
/* Enumeration:
    laImageWidget_t

  Summary:
    Image widget struct definition

  Description:
    An image widget displays an image asset.

  Remarks:
    None.
*/
typedef struct laImageWidget_t
{
    laWidget widget; // widget base class

    laHAlignment halign; // image horizontal alignment
    laVAlignment valign; // image vertical alignment

    GFXU_ImageAsset* image; // pointer to image asset

    GFXU_ExternalAssetReader* reader; // asset reader

    laImageWidget_DrawEventCallback ImageDrawStart;
    laImageWidget_DrawEventCallback ImageDrawEnd;

} laImageWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laImageWidget_Constructor(laImageWidget* img);
void _laImageWidget_Destructor(laImageWidget* img);

void _laImageWidget_Paint(laImageWidget* img);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laImageWidget* laImageWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:
    void

  Returns:
    laImageWidget* - the widget

  Remarks:

*/
LIB_EXPORT laImageWidget* laImageWidget_New();

// *****************************************************************************
/* Function:
    laHAlignment laImageWidget_GetHAlignment(laImageWidget* img)

  Summary:
    Gets the image horizontal alignment value.

  Description:


  Parameters:
    laImageWidget* img - the widget

  Returns:
    laHAlignment - the horizontal alignment value

  Remarks:

*/
LIB_EXPORT laHAlignment laImageWidget_GetHAlignment(laImageWidget* img);

// *****************************************************************************
/* Function:
    laResult laImageWidget_SetHAlignment(laImageWidget* img,
                                     laHAlignment align)

  Summary:
    Sets the image horizontal alignment value.

  Description:


  Parameters:
    laImageWidget* img - the widget
    laHAlignment align - the horizontal alignment value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImageWidget_SetHAlignment(laImageWidget* img,
                                                laHAlignment align);

// *****************************************************************************
/* Function:
    laVAlignment laImageWidget_GetVAlignment(laImageWidget* img)

  Summary:
    Gets the image vertical alignment value.

  Description:


  Parameters:
    laImageWidget* img - the widget

  Returns:
    laVAlignment - the vertical alignment setting

  Remarks:

*/
LIB_EXPORT laVAlignment laImageWidget_GetVAlignment(laImageWidget* img);

// *****************************************************************************
/* Function:
    laResult laImageWidget_SetVAlignment(laImageWidget* img,
                                         laVAlignment align)

  Summary:
    Sets the image vertical alignment value.

  Description:


  Parameters:
    laImageWidget* img - the widget
    laVAlignment - the vertical alignment setting

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImageWidget_SetVAlignment(laImageWidget* img,
                                                laVAlignment align);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laImageWidget_GetImage(laImageWidget* img)

  Summary:
    Gets the image asset pointer for the widget.

  Description:


  Parameters:
    laImageWidget* img - the widget

  Returns:
    GFXU_ImageAsset* - the image asset pointer

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laImageWidget_GetImage(laImageWidget* img);
// *****************************************************************************
/* Function:
    laResult laImageWidget_SetImage(laImageWidget* img,
                                    GFXU_ImageAsset* imgAst)

  Summary:
    Sets the image asset pointer for the widget.

  Description:


  Parameters:
    llaImageWidget* img - the widget
    GFXU_ImageAsset* imgAst - the image asset pointer

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImageWidget_SetImage(laImageWidget* img,
                                           GFXU_ImageAsset* imgAst);

// DOM-IGNORE-BEGIN
// internal use only
void _laImageWidget_GetImageRect(laImageWidget* img,
                                 GFX_Rect* imgRect,
                                 GFX_Rect* imgSrcRect);

void laImageWidget_SetCallBackStart(laImageWidget* image, laImageWidget_DrawEventCallback cb);
void laImageWidget_SetCallBackEnd(laImageWidget* image, laImageWidget_DrawEventCallback cb);
// DOM-IGNORE-END


#endif // LA_IMAGE_WIDGET_ENABLED
#endif /* LIBARIA_IMAGE_H */
