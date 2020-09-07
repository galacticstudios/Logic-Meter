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
 Module for Microchip Graphics Library - Graphics Utilities Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_image.h

  Summary:
    Defines image assets

  Description:
    Image drawing at specified coordinates
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFXU_IMAGE_H
#define GFXU_IMAGE_H
//DOM-IGNORE-END

#include "gfx/utils/inc/gfxu_global.h"

typedef struct GFXU_PaletteAsset_t GFXU_PaletteAsset;

// *****************************************************************************
/* Enumeration:
    GFXU_ImageFormat_t

  Summary:
    Indicates the image encoding format
*/
typedef enum GFXU_ImageFormat_t
{
    GFXU_IMAGE_FORMAT_RAW = 0,
    GFXU_IMAGE_FORMAT_JPEG,
    GFXU_IMAGE_FORMAT_PNG
} GFXU_ImageFormat;

// *****************************************************************************
/* Enumeration:
    GFXU_ImageCompressionType_t

  Summary:
    Indicates the image compression type, only applies to RAW types
*/
typedef enum GFXU_ImageCompressionType_t
{
    GFXU_IMAGE_COMPRESSION_NONE = 0,
    GFXU_IMAGE_COMPRESSION_RLE
} GFXU_ImageCompressionType;

// *****************************************************************************
/* Enumeration:
    GFXU_ImageFlags_t

  Summary:
    A list of flags describing an image asset
*/
typedef enum GFXU_ImageFlags_t
{
    GFXU_IMAGE_USE_MASK          = 0x1,
    GFXU_IMAGE_SUPPORTS_CLIPPING = 0x2,
    GFXU_IMAGE_DIRECT_BLIT       = 0x4,
} GFXU_ImageFlags;

// *****************************************************************************
/* Structure:
    GFXU_ImageAsset_t

  Summary:
    Describes an image asset.
    
    header - standard asset header
    format - the format of the image.  this directly affects which decoder
             is invoked when rendering the image
    width - the width of the image in pixels
    height - the height of the image in pixels
    colorMode - the color mode of the image
    compType - the compression mode of the image
    useMask - indicates of the mask field is used
    mask - may contain a masking color for the image.  blit operations may 
           reference this value and reject image pixels that match this
           value
    palette - will contain a valid pointer to a palette asset if thie image
              is an index map instead of a color image
*/
typedef struct GFXU_ImageAsset_t
{
    GFXU_AssetHeader header;
    GFXU_ImageFormat format;
    uint32_t width;
    uint32_t height;
    uint32_t bufferWidth;
    uint32_t bufferHeight;
    GFX_ColorMode colorMode;
    GFXU_ImageCompressionType compType;
    GFXU_ImageFlags flags;
    GFX_Color mask;
    GFXU_PaletteAsset* palette;
} GFXU_ImageAsset;

// *****************************************************************************
/* Function:
    GFX_Result GFXU_DrawImage(void);

  Summary:
    Draws a portion of the given image at the specified coordinates.
     
  Parameters:
    GFXU_ImageAsset* img - pointer to the image asset to draw
    int32_t src_x - the x position of the source image to draw
                    (0 if whole image)
    int32_t src_y - the y position of the source image to draw
                    (0 if whole image)
    int32_t src_width - the width of the source rectangle to draw
                        (source width if whole image)
    int32_t src_width - the height of the source rectangle to draw
                        (source height if whole image)   
    int32_t dest_x - the x position to draw to
    int32_t dest_y - the y position to draw to
    GFXU_MemoryIntf* read_cb - a pointer to a memory interface to use for
                               memory operations, not needed for internal
                               assets
    GFXU_ExternalAssetReader** reader - will return as a valid pointer if
                          the image asset is located in an external source.
                          If this is the case then the caller is responsible
                          for servicing the external asset reader state
                          machine until completion.  The caller is then
                          responsible for freeing the reader's memory.
  Returns:
    GFX_Result
*/
LIB_EXPORT GFX_Result GFXU_DrawImage(GFXU_ImageAsset* img,
                                     int32_t src_x,
                                     int32_t src_y,
                                     int32_t src_width,
                                     int32_t src_height,
                                     int32_t dest_x,
                                     int32_t dest_y,
                                     GFXU_MemoryIntf* read_cb,
                                     GFXU_ExternalAssetReader** reader);

#endif /* GFXU_IMAGE_H */