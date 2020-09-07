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
    libaria_widget_imagesequence.h

  Summary:


  Description:
    This module implements image sequence (slide show) widget drawing functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_IMAGESEQUENCE_H
#define LIBARIA_IMAGESEQUENCE_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_IMAGESEQUENCE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_string.h"

// DOM-IGNORE-BEGIN
#define LA_IMAGESEQ_RESTART -1
// DOM-IGNORE-END

typedef struct laImageSequenceWidget_t laImageSequenceWidget;

// *****************************************************************************
/* Function Pointer:
    laImageSequenceImageChangedEvent_FnPtr

  Summary:
    Image changed event function callback type
*/
typedef void (*laImageSequenceImageChangedEvent_FnPtr)(laImageSequenceWidget*);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laImageSequenceEntry_t

  Summary:
    Image sequence entry definition

  Description:
    Defines a single entry for the image sequence widget

  Remarks:
    None.
*/
typedef struct laImageSequenceEntry_t
{
    GFXU_ImageAsset* image; // image asset pointer

    uint32_t delay; // how many time units to display this entry

    laHAlignment halign; // the horizontal alignment for this entry
    laVAlignment valign; // the vertical alignment for this entry
} laImageSequenceEntry;

// *****************************************************************************
/* Enumeration:
    laImageSequenceWidget_t

  Summary:
    Image sequence widget struct definition

  Description:
    An image sequence widget is similar to an image widget with the additional
    capability of showing a sequence of images and automating the transition
    between them.

    This widget is dependent on the time value provided to laUpdate.  If
    laUpdate is not provided with time information this widget will not be
    able to automatically cycle.

  Remarks:
    None.
*/
typedef struct laImageSequenceWidget_t
{
    laWidget widget; // widget base class

    uint32_t count; // number of image entries for this widget
    laImageSequenceEntry* images; // image entry array

    int32_t activeIdx; // currently displayed entry

    laBool playing; // indicates that the widget is automatically cycling
    uint32_t time; // current cycle time

    laBool repeat; // indicates that the sequence should repeat when it
                   // reaches the end of the sequence

    laImageSequenceImageChangedEvent_FnPtr cb; // callback when the image changes

    GFXU_ExternalAssetReader* reader; // asset reader pointer
} laImageSequenceWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laImageSequenceWidget_Constructor(laImageSequenceWidget* img);
void _laImageSequenceWidget_Destructor(laImageSequenceWidget* img);

laWidgetUpdateState _laImageSequenceWidget_Update(laImageSequenceWidget* img, uint32_t dt);
void _laImageSequenceWidget_Paint(laImageSequenceWidget* img);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laImageSequenceWidget* laImageSequenceWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Parameters:
    void

  Returns:
    laImageSequenceWidget*

  Remarks:

*/
LIB_EXPORT laImageSequenceWidget* laImageSequenceWidget_New();

// *****************************************************************************
/* Function:
    uint32_t laImageSequenceWidget_GetImageCount(laImageSequenceWidget* img)

  Summary:
    Gets the number of image entries for this widget.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    uint32_t - the number of entries for this sequence widget

  Remarks:

*/
LIB_EXPORT uint32_t laImageSequenceWidget_GetImageCount(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_SetImageCount(laImageSequenceWidget* img,
                                                 uint32_t count)

  Summary:
    Sets the number of image entries for this widget.  An image entry that is
    null will show nothing.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t count - the desired number of entries

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_SetImageCount(laImageSequenceWidget* img,
                                                        uint32_t count);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laImageSequenceWidget_GetImage(laImageSequenceWidget* img,
                                                    uint32_t idx)

  Summary:
    Gets the image asset pointer for an entry.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the index

  Returns:
    GFXU_ImageAsset* - the image asset pointer

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laImageSequenceWidget_GetImage(laImageSequenceWidget* img,
                                                           uint32_t idx);
// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_SetImage(laImageSequenceWidget* img,
                                            uint32_t idx,
                                            GFXU_ImageAsset* imgAst)

  Summary:
    Sets the image asset pointer for an entry.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the index
    GFXU_ImageAsset* imgAst - the image asset pointer

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_SetImage(laImageSequenceWidget* img,
                                                   uint32_t idx,
                                                   GFXU_ImageAsset* imgAst);

// *****************************************************************************
/* Function:
    uint32_t laImageSequenceWidget_GetImageDelay(laImageSequenceWidget* img,
                                                        uint32_t idx)

  Summary:
    Gets the image delay for an entry.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the index

  Returns:
    uint32_t - the delay value

  Remarks:

*/
LIB_EXPORT uint32_t laImageSequenceWidget_GetImageDelay(laImageSequenceWidget* img,
                                                        uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_SetImageDelay(laImageSequenceWidget* img,
                                                        uint32_t idx,
                                                        uint32_t delay)

  Summary:
    Sets the image delay for an entry.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the index
    uint32_t delay - the delay value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_SetImageDelay(laImageSequenceWidget* img,
                                                        uint32_t idx,
                                                        uint32_t delay);

// *****************************************************************************
/* Function:
    laHAlignment laImageSequenceWidget_GetImageHAlignment(laImageSequenceWidget* img,
                                                          uint32_t idx)

  Summary:
    Gets the horizontal alignment for an image entry

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the index

  Returns:
    laHAlignment - the halign value

  Remarks:

*/
LIB_EXPORT laHAlignment laImageSequenceWidget_GetImageHAlignment(laImageSequenceWidget* img,
                                                                 uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_SetImageHAlignment(laImageSequenceWidget* img,
                                                      uint32_t idx,
                                                      laHAlignment align)

  Summary:
    Sets the horizontal alignment for an image entry.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the index
    laHAlignment align - the halign value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_SetImageHAlignment(laImageSequenceWidget* img,
                                                             uint32_t idx,
                                                             laHAlignment align);

// *****************************************************************************
/* Function:
    laVAlignment laImageSequenceWidget_GetImageVAlignment(laImageSequenceWidget* img,
                                                          uint32_t idx)

  Summary:
    Sets the vertical alignment for an image entry

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the index

  Returns:
    laVAlignment - the valign value

  Remarks:

*/
LIB_EXPORT laVAlignment laImageSequenceWidget_GetImageVAlignment(laImageSequenceWidget* img,
                                                                 uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_SetImageVAlignment(laImageSequenceWidget* img,
                                                             uint32_t idx,
                                                             laVAlignment align)

  Summary:
    Sets the vertical alignment value for an image entry

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the index
    laVAlignment align - the vertical alignment setting

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_SetImageVAlignment(laImageSequenceWidget* img,
                                                             uint32_t idx,
                                                             laVAlignment align);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_Stop(laImageSequenceWidget* img)

  Summary:
    Stops the widget from automatically cycling through the image entries.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_Stop(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_Play(laImageSequenceWidget* img)

  Summary:
    Starts the widget automatically cycling through the image entries.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_Play(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_Rewind(laImageSequenceWidget* img)

  Summary:
    Resets the current image sequence display index to zero.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_Rewind(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laBool laImageSequenceWidget_IsPlaying(laImageSequenceWidget* img)

  Summary:
    Indicates if the widget is currently cycling through the image entries.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    laBool - indicates if the widget is automatically cycling

  Remarks:

*/
LIB_EXPORT laBool laImageSequenceWidget_IsPlaying(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laBool laImageSequenceWidget_GetRepeat(laImageSequenceWidget* img)

  Summary:
    Indicates if the widget will repeat through the image entries.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    laBool - indicates if the widget is automatically repeating

  Remarks:

*/
LIB_EXPORT laBool laImageSequenceWidget_GetRepeat(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_SetRepeat(laImageSequenceWidget* img,
                                                    laBool repeat)

  Summary:
    Sets the repeat flag for the widget

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    laBool repeat - the desired repeat setting

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_SetRepeat(laImageSequenceWidget* img,
                                                    laBool repeat);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_ShowImage(laImageSequenceWidget* img,
                                             uint32_t idx)

  Summary:
    Sets the active display index to the indicated value.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    uint32_t idx - the desired index

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_ShowImage(laImageSequenceWidget* img,
                                                    uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_ShowNextImage(laImageSequenceWidget* img)

  Summary:
    Sets the active display index to the next index value.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_ShowNextImage(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_ShowPreviousImage(laImageSequenceWidget* img)

  Summary:
    Sets the active display index to the previous index value.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_ShowPreviousImage(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laImageSequenceImageChangedEvent_FnPtr laImageSequenceWidget_GetImageChangedEventCallback(laImageSequenceWidget* img)

  Summary:
    Gets the image changed event callback pointer.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget

  Returns:
    laImageSequenceImageChangedEvent_FnPtr - a valid callback pointer or NULL

  Remarks:

*/
LIB_EXPORT laImageSequenceImageChangedEvent_FnPtr laImageSequenceWidget_GetImageChangedEventCallback(laImageSequenceWidget* img);

// *****************************************************************************
/* Function:
    laResult laImageSequenceWidget_SetImageChangedEventCallback(laImageSequenceWidget* img,
                                                                laImageSequenceImageChangedEvent_FnPtr cb)

  Summary:
    Sets the image changed event callback pointer.  This callback is called
    whenever the active display index is changed.

  Description:


  Parameters:
    laImageSequenceWidget* img - the widget
    laImageSequenceImageChangedEvent_FnPtr cb - a valid callback pointer or NULL

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laImageSequenceWidget_SetImageChangedEventCallback(laImageSequenceWidget* img,
                                                                       laImageSequenceImageChangedEvent_FnPtr cb);

// DOM-IGNORE-BEGIN
// internal use only
void _laImageSequenceWidget_GetImageRect(laImageSequenceWidget* img,
                                         GFX_Rect* imgRect,
                                         GFX_Rect* imgSrcRect);
// DOM-IGNORE-END

#endif // LA_IMAGESEQUENCE_WIDGET_ENABLED
#endif /* LIBARIA_IMAGESEQUENCE_H */
