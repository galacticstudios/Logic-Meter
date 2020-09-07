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


#include "gfx/utils/inc/gfxu_image.h"
#include "gfx/utils/inc/gfxu_palette.h"

static GFX_Result drawIndexImage(GFXU_ImageAsset* img,
                                 int32_t src_x,
                                 int32_t src_y,
                                 int32_t src_width,
                                 int32_t src_height,
                                 int32_t dest_x,
                                 int32_t dest_y)
{
    //GFX_ColorMode colorMode;
    GFX_PixelBuffer pixBuffer, paletteBuffer;
    
    GFX_PixelBufferCreate(img->bufferWidth,
                          img->bufferHeight,
                          img->colorMode,
                          img->header.dataAddress,
                          &pixBuffer);
                          
    GFX_PixelBufferCreate(img->palette->colorCount,
                          1,
                          img->palette->colorMode,
                          img->palette->header.dataAddress,
                          &paletteBuffer);                     
    
    //if(GFX_Get(GFXF_COLOR_MODE, &colorMode) == GFX_FAILURE)
    //    return GFX_FAILURE;
        
    GFX_Set(GFXF_DRAW_PALETTE, &paletteBuffer);
        
    if((img->flags & GFXU_IMAGE_USE_MASK) > 0)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_TRUE);
        GFX_Set(GFXF_DRAW_MASK_VALUE, img->mask);
    }
    else
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    }
    
    return GFX_DrawBlit(&pixBuffer, src_x, src_y, src_width, src_height, dest_x, dest_y);
}

static GFX_Result drawColorImage(GFXU_ImageAsset* img,
                                 int32_t src_x,
                                 int32_t src_y,
                                 int32_t src_width,
                                 int32_t src_height,
                                 int32_t dest_x,
                                 int32_t dest_y)
{
    GFX_ColorMode colorMode;
    GFX_PixelBuffer pixBuffer;
    
    GFX_PixelBufferCreate(img->bufferWidth,
                          img->bufferHeight,
                          img->colorMode,
                          img->header.dataAddress,
                          &pixBuffer);
                          
    GFX_Get(GFXF_COLOR_MODE, &colorMode);
    
    if((img->flags & GFXU_IMAGE_USE_MASK) > 0)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_TRUE);
        GFX_Set(GFXF_DRAW_MASK_VALUE, img->mask);
    }
    else
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    }
    
    if((img->flags & GFXU_IMAGE_DIRECT_BLIT) > 0)
    {
        return GFX_DrawDirectBlit(&pixBuffer,
                                  src_x,
                                  src_y,
                                  src_width,
                                  src_height,
                                  dest_x,
                                  dest_y);
    }
    else
    {
        return GFX_DrawBlit(&pixBuffer,
                            src_x,
                            src_y,
                            src_width,
                            src_height,
                            dest_x,
                            dest_y);
    }                            
}

GFX_Result GFXU_DrawImageRawInternal(GFXU_ImageAsset* img,
                                     int32_t src_x,
                                     int32_t src_y,
                                     int32_t src_width,
                                     int32_t src_height,
                                     int32_t dest_x,
                                     int32_t dest_y)
{
    switch(img->colorMode)
    {
        case GFX_COLOR_MODE_INDEX_1:
        case GFX_COLOR_MODE_INDEX_4:
        case GFX_COLOR_MODE_INDEX_8:
        {
            return drawIndexImage(img,
                                  src_x,
                                  src_y,
                                  src_width,
                                  src_height,
                                  dest_x,
                                  dest_y);
        }
        case GFX_COLOR_MODE_RGBA_5551:
        case GFX_COLOR_MODE_RGBA_8888:
        case GFX_COLOR_MODE_ARGB_8888:
        {
            //Process alpha channel for these RAW formats
            GFX_BlendMode blendMode;
            GFX_Get(GFXF_DRAW_BLEND_MODE, &blendMode);
            GFX_Set(GFXF_DRAW_BLEND_MODE, blendMode | GFX_BLEND_CHANNEL);
            GFX_Set(GFXF_DRAW_ALPHA_ENABLE, GFX_TRUE);
            //No break, fall through
        }
        case GFX_COLOR_MODE_GS_8:
        case GFX_COLOR_MODE_RGB_332:
        case GFX_COLOR_MODE_RGB_565:
        case GFX_COLOR_MODE_RGB_888:            
        case GFX_COLOR_MODE_YUV:
        {
            return drawColorImage(img,
                                  src_x,
                                  src_y,
                                  src_width,
                                  src_height,
                                  dest_x,
                                  dest_y);
        }
    };
        
    return GFX_FAILURE;
}