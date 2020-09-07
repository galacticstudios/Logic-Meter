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
#include "gfx/third_party/lodepng/lodepng.h"
#include "gfx/hal/inc/gfx_context.h"

GFX_Result GFXU_DrawImagePngExternal(GFXU_ImageAsset* img,
                                     int32_t src_x,
                                     int32_t src_y,
                                     int32_t src_width,
                                     int32_t src_height,
                                     int32_t dest_x,
                                     int32_t dest_y,
                                     GFXU_MemoryIntf* mem)
{
    LodePNGColorType colorType;
    unsigned bitDepth;
    unsigned error;
    unsigned char* image;
    GFX_PixelBuffer pixBuffer;
    size_t pngsize;
    GFX_Result result = GFX_FAILURE;
    GFX_ColorMode colorMode;
    GFX_Point pnt;
    GFX_Color color;
    uint8_t* localData;
    GFX_BlendMode blendMode;

    switch(img->colorMode)
    {
        case GFX_COLOR_MODE_INDEX_8:
        {
            colorMode = GFX_COLOR_MODE_RGB_332;
            colorType = LCT_PALETTE;
            bitDepth = 8;
            break;
        }
        case GFX_COLOR_MODE_RGB_888:
        {
            colorMode = img->colorMode;
            colorType = LCT_RGB;
            bitDepth = 8;
            break;
        }
        case GFX_COLOR_MODE_ARGB_8888:
        {
            colorMode = img->colorMode;
            colorType = LCT_RGBA;
            bitDepth = 8;
            break;
        }
        default:
        {
            return result;
        }
    };

    pngsize = img->header.dataSize;

    if(mem->open((GFXU_AssetHeader*)img) == GFX_FAILURE)
        return GFX_FAILURE;

    localData = (uint8_t*)mem->heap.malloc(pngsize);

    if(localData == NULL)
    {
        mem->close((GFXU_AssetHeader*)img);
        return GFX_FAILURE;
    }

    mem->read(NULL,
              (GFXU_AssetHeader*)img,
              img->header.dataAddress,
              img->header.dataSize,
              localData,
              NULL);

    error = lodepng_decode_memory(&image,
                                 (unsigned*) &img->width,
                                 (unsigned*) &img->height,
                                 localData, pngsize,
                                 colorType, bitDepth);
#ifndef _WIN32
    //assert(error == 0); // Trap for no memory available for image decode. FIXME for H3
#endif

    mem->heap.free((void*)localData);

    mem->close((GFXU_AssetHeader*)img);

    if (error == 0)
    {
        GFX_PixelBufferCreate(img->bufferWidth,
                              img->bufferHeight,
                              colorMode,
                              image,
                              &pixBuffer);

        for (pnt.y = 0; pnt.y < pixBuffer.size.height; pnt.y++)
        {

            for (pnt.x = 0; pnt.x < pixBuffer.size.width; pnt.x++)
            {
                // get color value/index
                color = GFX_PixelBufferGet(&pixBuffer, &pnt);

                //swap channnels with ARGB
                if (img->colorMode == GFX_COLOR_MODE_ARGB_8888)
                {
                    color = ((color >> 16) & 0xff) |
                        (color & 0xff) << 16 |
                        (color & 0xff00) |
                        (color & 0xff000000);
                }
                else //RGB_888
                {
                    color = ((color >> 16) & 0xff) |
                        (color & 0xff) << 16 |
                        (color & 0xff00);
                }

                GFX_PixelBufferSet(&pixBuffer, &pnt, color);
            }
        }

        GFX_Get(GFXF_DRAW_BLEND_MODE, &blendMode);
        GFX_Set(GFXF_DRAW_BLEND_MODE, blendMode | GFX_BLEND_CHANNEL);
        GFX_Set(GFXF_DRAW_ALPHA_ENABLE, GFX_TRUE);

        result = GFX_DrawBlit(&pixBuffer,
                              src_x,
                              src_y,
                              src_width,
                              src_height,
                              dest_x,
                              dest_y);

        GFX_Set(GFXF_DRAW_BLEND_MODE, blendMode);
    }

    if(image != NULL)
        GFX_ActiveContext()->memory.free(image);

    return result;
}