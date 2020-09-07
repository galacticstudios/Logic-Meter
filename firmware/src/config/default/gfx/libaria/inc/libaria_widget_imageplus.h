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

#ifndef LIBARIA_IMAGEPLUS_H
#define LIBARIA_IMAGEPLUS_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_IMAGEPLUS_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

typedef struct laImagePlusWidget_t laImagePlusWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

typedef enum laImagePlusWidget_ResizeFilter_t
{
    LA_IMAGEFILTER_NEARESTNEIGHBOR = 0x0,
    LA_IMAGEFILTER_BILINEAR
} laImagePlusWidget_ResizeFilter;

// *****************************************************************************
/* Enumeration:
    laImagePlusWidget_t

  Summary:
    Image plus widget struct definition

  Description:
    An image plus widget displays an image asset and can translate and resize that image.

  Remarks:
    None.
*/
typedef struct laImagePlusWidget_t
{
    laWidget widget; // widget base class

    GFXU_ImageAsset* image; // pointer to image asset

    int32_t transformX;
    int32_t transformY;
    int32_t transformWidth;
    int32_t transformHeight;

    laBool resizeToFit;
    laBool preserveAspect;

    laImagePlusWidget_ResizeFilter resizeFilter;

    laBool inputEnabled;

    GFX_Point touch0;
    laBool touch0_down;

    GFX_Point touch1;
    laBool touch1_down;

    GFX_PixelBuffer buffer;

} laImagePlusWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laImagePlusWidget_Constructor(laImagePlusWidget* img);
void _laImagePlusWidget_Destructor(laImagePlusWidget* img);

void _laImagePlusWidget_Paint(laImagePlusWidget* img);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laImagePlusWidget* laImagePlusWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:
    void

  Returns:
    laImagePlusWidget* - the widget

  Remarks:

*/
LIB_EXPORT laImagePlusWidget* laImagePlusWidget_New();

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laImagePlusWidget_GetImage(laImagePlusWidget* img)

  Summary:
    Gets the image asset pointer for the widget.

  Description:


  Parameters:
    laImagePlusWidget* img - the widget

  Returns:
    GFXU_ImageAsset* - the image asset pointer

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laImagePlusWidget_GetImage(laImagePlusWidget* img);

// *****************************************************************************
/* Function:
    laResult laImagePlusWidget_SetImage(laImagePlusWidget* img,
                                        GFXU_ImageAsset* imgAst)

  Summary:
    Sets the image asset pointer for the widget.

  Description:


  Parameters:
    laImagePlusWidget* img - the widget
    GFXU_ImageAsset* imgAst - the image asset pointer

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImagePlusWidget_SetImage(laImagePlusWidget* img,
                                               GFXU_ImageAsset* imgAst);


/*    Function:
        int32_t laImagePlusWidget_GetTransformX(laImagePlusWidget* img)

    Summary:
        Returns the image transform x coefficient

    Description:
        Returns the image transform x coefficient

    Parameters:
        laImagePlusWidget* img - the widget

    Returns:
        int32_t - the x translation coefficient

*/
LIB_EXPORT int32_t laImagePlusWidget_GetTransformX(laImagePlusWidget* img);

/*    Function:
        laResult laImagePlusWidget_SetTransformX(laImagePlusWidget* img, int32_t x)

    Summary:
        Sets the image transform x coefficient

    Description:
        Sets the image transform x coefficient

    Parameters:
        laImagePlusWidget* img - the widget
        int32_t x - the desired x value

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laImagePlusWidget_SetTransformX(laImagePlusWidget* img, int32_t x);

/*    Function:
        int32_t laImagePlusWidget_GetTransformY(laImagePlusWidget* img)

    Summary:
        Returns the image transform y coefficient

    Description:
        Returns the image transform y coefficient

    Parameters:
        laImagePlusWidget* img - the widget

    Returns:
        int32_t - the y translation coefficient

*/
LIB_EXPORT int32_t laImagePlusWidget_GetTransformY(laImagePlusWidget* img);

/*    Function:
        laResult laImagePlusWidget_SetTransformY(laImagePlusWidget* img, int32_t y)

    Summary:
        Sets the image transform y coefficient

    Description:
        Sets the image transform y coefficient

    Parameters:
        laImagePlusWidget* img - the widget
        int32_t y - the desired y value

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laImagePlusWidget_SetTransformY(laImagePlusWidget* img, int32_t y);

/*    Function:
        int32_t laImagePlusWidget_GetTransformWidth(laImagePlusWidget* img)

    Summary:
        Returns the image scale width coefficient

    Description:
        Returns the image scale width coefficient

    Parameters:
        laImagePlusWidget* img - the widget

    Returns:
        int32_t - the scale width coordinate value in pixels

*/
LIB_EXPORT int32_t laImagePlusWidget_GetTransformWidth(laImagePlusWidget* img);

/*    Function:
        laResult laImagePlusWidget_SetTransformWidth(laImagePlusWidget* img,
                                                     int32_t width)

    Summary:
        Sets the image scale width coefficient.  This value is in pixels not
        percentage

    Description:
        Sets the image scale width coefficient.  This value is in pixels not
        percentage

    Parameters:
        laImagePlusWidget* img - the widget
        int32_t width - the desired width value, must be > 0

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laImagePlusWidget_SetTransformWidth(laImagePlusWidget* img, int32_t width);

/*    Function:
        int32_t laImagePlusWidget_GetTransformHeight(laImagePlusWidget* img)

    Summary:
        Returns the image scale height coefficient

    Description:
        Returns the image scale height coefficient

    Parameters:
        laImagePlusWidget* img - the widget

    Returns:
        int32_t - the scale width coordinate value in pixels

*/
LIB_EXPORT int32_t laImagePlusWidget_GetTransformHeight(laImagePlusWidget* img);

/*    Function:
        laResult laImagePlusWidget_SetTransformHeight(laImagePlusWidget* img,
                                                      int32_t height)

    Summary:
        Sets the image scale height coefficient.  This value is in pixels not
        percentage

    Description:
        Sets the image scale height coefficient.  This value is in pixels not
        percentage

    Parameters:
        laImagePlusWidget* img - the widget
        int32_t height - the desired height value, must be > 0

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laImagePlusWidget_SetTransformHeight(laImagePlusWidget* img,
                                                         int32_t height);


/*    Function:
        laResult laImagePlusWidget_ResetTransform(laImagePlusWidget* img)

    Summary:
        Resets the image transform values to zero

    Description:
        Resets the image transform values to zero

    Parameters:
        laImagePlusWidget* img - the widget

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laImagePlusWidget_ResetTransform(laImagePlusWidget* img);

// *****************************************************************************
/* Function:
    laBool laImagePlusWidget_GetStretchEnabled(laImagePlusWidget* img)

  Summary:
    Returns the boolean value of the 'stretch to fit' property

  Description:
    Returns the boolean value of the 'stretch to fit' property.  This flag will
    cause the image to be stretched to fill the space of the container widget.
    Widget margins are still considered.

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laBool - the value of the resize flag

  Remarks:

*/
LIB_EXPORT laBool laImagePlusWidget_GetStretchEnabled(laImagePlusWidget* img);

// *****************************************************************************
/* Function:
    laResult laImagePlusWidget_SetStretchEnabled(laImagePlusWidget* img, laBool enable)

  Summary:
    Sets the boolean value of the stretch property

  Description:
    Sets the boolean value of the stretch property

  Parameters:
    laImagePlusWidget* img - the widget
    laBool - the desired flag value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImagePlusWidget_SetStretchEnabled(laImagePlusWidget* img,
                                                        laBool enable);


// *****************************************************************************
/* Function:
    laBool laImagePlusWidget_GetPreserveAspectEnabled(laImagePlusWidget* img)

  Summary:
    Returns the boolean value of the 'preserve aspect' property

  Description:
    Returns the boolean value of the 'preserve aspect' property.  If the stretch
    flag is enabled then this flag will cause the resized image to fill as much
    of the container widget as possible while still preserving its original aspect
    ratio.

  Parameters:
    laImagePlusWidget* img - the widget

  Returns:
    laBool - the value of the aspect flag

  Remarks:

*/
LIB_EXPORT laBool laImagePlusWidget_GetPreserveAspectEnabled(laImagePlusWidget* img);

// *****************************************************************************
/* Function:
    laResult laImagePlusWidget_SetStretchEnabled(laImagePlusWidget* img, laBool enable)

  Summary:
    Sets the boolean value of the 'preserve aspect' property

  Description:
    Sets the boolean value of the 'preserve aspect' property

  Parameters:
    laImagePlusWidget* img - the widget
    laBool - the desired flag value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImagePlusWidget_SetPreserveAspectEnabled(laImagePlusWidget* img,
                                                               laBool enable);


// *****************************************************************************
/* Function:
    laImagePlusWidget_ResizeFilter laImagePlusWidget_GetResizeFilter(laImagePlusWidget* img)

  Summary:
    Returns the resize filter setting for this image widget

  Description:
    Returns the resize filter setting for this image widget.  This flag affects
    the speed and quality of the image resize operation.

  Parameters:
    laImagePlusWidget* img - the widget

  Returns:
    laImagePlusWidget_ResizeFilter - the filter setting

  Remarks:

*/
LIB_EXPORT laImagePlusWidget_ResizeFilter laImagePlusWidget_GetResizeFilter(laImagePlusWidget* img);

// *****************************************************************************
/* Function:
    laResult laImagePlusWidget_SetResizeFilter(laImagePlusWidget* img,
                                               laImagePlusWidget_ResizeFilter filter)

  Summary:
    Sets the resize filter value of the widget

  Description:
    Sets the resize filter value of the widget

  Parameters:
    laImagePlusWidget* img - the widget
    laImagePlusWidget_ResizeFilter filter - the desired filter

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImagePlusWidget_SetResizeFilter(laImagePlusWidget* img,
                                                      laImagePlusWidget_ResizeFilter filter);


// *****************************************************************************
/* Function:
    laBool laImagePlusWidget_GetInteractive(laImagePlusWidget* img)

  Summary:
    Returns true if the widget is configured to respond to input events

  Description:
    This widget can be configured to respond to input events.  It can react to
    single and double touch events.  These events will directly modify the
    transform state and cause the image to translate and resize as desired.

  Parameters:
    laImagePlusWidget* img - the widget

  Returns:
    laBool - the value of the interactive flag

  Remarks:

*/
LIB_EXPORT laBool laImagePlusWidget_GetInteractive(laImagePlusWidget* img);

// *****************************************************************************
/* Function:
    laResult laImagePlusWidget_SetInteractive(laImagePlusWidget* img, laBool interactive)

  Summary:
    Sets the widget interactive flag

  Description:
    Sets the widget interactive flag

  Parameters:
    laImagePlusWidget* img - the widget
    laBool - the desired flag value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImagePlusWidget_SetInteractive(laImagePlusWidget* img,
                                                     laBool interactive);

// DOM-IGNORE-BEGIN
// internal use only
void _laImagePlusWidget_GetImageRect(laImagePlusWidget* img,
                                     GFX_Rect* imgRect,
                                     GFX_Rect* imgSrcRect);
// DOM-IGNORE-END

#endif // LA_IMAGE_WIDGET_ENABLED
#endif /* LIBARIA_IMAGE_H */