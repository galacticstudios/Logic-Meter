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

#include <stdlib.h>
#include <string.h>

#include "gfx/hal/inc/gfx_color.h"
#include "gfx/utils/inc/gfxu_palette.h"
#include "gfx/utils/inc/gfxu_image_utils.h"

typedef enum imageReaderState_t
{
    PREPARE_IMAGE_DATA_REQUEST,
    ATTEMPT_IMAGE_DATA_REQUEST,
    WAITING_FOR_IMAGE_DATA,
    DRAW_IMAGE_DATA,
    PREPARE_PALETTE_DATA_REQUEST,
    ATTEMPT_PALETTE_DATA_REQUEST,
    WAITING_FOR_PALETTE_DATA,
    DRAW_PALETTE_DATA,
    CLEANUP,
    DONE
} drawState;

typedef struct GFXU_ImageAssetReader_t
{
    GFXU_ExternalAssetReader header;
    
    GFXU_ImageAsset* img;
    
    GFX_Rect sourceRect;
    GFX_Point destPoint;
    
    uint32_t row_max;
    uint32_t col_max;
    
    uint32_t row;
    uint32_t col;
    
    uint32_t imageIndex;
    GFX_Color imageColor;
    
    uint32_t paletteIndex;
    uint32_t paletteCache;
    GFX_Color paletteColor;
    
} GFXU_ImageAssetReader;

static GFX_Result calculateImageIndex(GFXU_ImageAssetReader* imgReader)
{
    imgReader->imageIndex = imgReader->sourceRect.x + imgReader->col + 
                           ((imgReader->sourceRect.y + imgReader->row) * imgReader->img->width);

    imgReader->header.state = ATTEMPT_IMAGE_DATA_REQUEST;
    
    return GFX_SUCCESS;
}

static void imageReadRequestCompleted(GFXU_ImageAssetReader* imgReader)
{
    uint32_t offs;

    if(imgReader->img->palette != NULL)
    {
        if(imgReader->img->colorMode == GFX_COLOR_MODE_INDEX_1)
        {
            offs = imgReader->imageIndex % 8;
            
            imgReader->paletteIndex = imgReader->imageColor & (0x80 >> offs);
            imgReader->paletteIndex >>= 7 - offs;
        }
        else if(imgReader->img->colorMode == GFX_COLOR_MODE_INDEX_4)
        {
            if(imgReader->imageIndex % 2 > 0)
            {
                offs = 0xF;
                imgReader->paletteIndex = imgReader->imageColor & offs;
            }
            else
            {
                offs = 0xF0;
                imgReader->paletteIndex = (imgReader->imageColor & offs) >> 0x4;
            }
        }
        else
            imgReader->paletteIndex = imgReader->imageColor;
    
    
        imgReader->header.state = ATTEMPT_PALETTE_DATA_REQUEST;
        imgReader->header.status = GFXU_READER_STATUS_WAITING;
    }
    else
    {
        imgReader->header.state = DRAW_IMAGE_DATA;
        imgReader->header.status = GFXU_READER_STATUS_DRAWING;
    }
}

static GFX_Result requestImageData(GFXU_ImageAssetReader* imgReader)
{
    void* address;
    uint32_t offs;
       
    if(imgReader->header.memIntf->read == GFX_NULL)
        return GFX_FAILURE;
       
    offs = getOffsetFromIndexAndBPP(imgReader->imageIndex,
                                    GFX_ColorModeInfoGet(imgReader->img->colorMode).bppOrdinal);
    
    address = imgReader->img->header.dataAddress;
    address = (void*)(((uint8_t*)address) + offs);

    if(imgReader->header.memIntf->read((GFXU_ExternalAssetReader*)imgReader,
                                       (GFXU_AssetHeader*)imgReader->img,
                                       address,
                                       GFX_ColorModeInfoGet(imgReader->img->colorMode).size,
                                       (uint8_t*)&imgReader->imageColor,
                                       (GFXU_MediaReadRequestCallback_FnPtr)&imageReadRequestCompleted) == GFX_FAILURE)
    {
        imgReader->header.state = CLEANUP; 
        
        return GFX_FAILURE;
    }
    
    // check if request was handled immediately
    if(imgReader->header.status != GFXU_READER_STATUS_DRAWING &&
       imgReader->header.state != ATTEMPT_PALETTE_DATA_REQUEST)
    {
        imgReader->header.status = GFXU_READER_STATUS_WAITING;
        imgReader->header.state = WAITING_FOR_IMAGE_DATA; 
    }
    
    return GFX_SUCCESS;                             
}

static GFX_Result drawImageData(GFXU_ImageAssetReader* imgReader)
{
    GFX_ColorMode colorMode;
    
    GFX_Get(GFXF_COLOR_MODE, &colorMode);
    GFX_Set(GFXF_DRAW_COLOR, GFX_ColorConvert(imgReader->img->colorMode,
                                              colorMode,
                                              imgReader->imageColor));
    
    if((imgReader->img->flags & GFXU_IMAGE_USE_MASK) > 0)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_TRUE);
        GFX_Set(GFXF_DRAW_MASK_VALUE, GFX_ColorConvert(imgReader->img->colorMode,
                                                       colorMode,
                                                       imgReader->img->mask));
    }
    else
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    }
    
    switch(imgReader->img->colorMode)
    {
        case GFX_COLOR_MODE_RGBA_5551:
        case GFX_COLOR_MODE_RGBA_8888:
        case GFX_COLOR_MODE_ARGB_8888:
        {
            //Process alpha channel for these RAW formats
            GFX_BlendMode blendMode;
            GFX_Get(GFXF_DRAW_BLEND_MODE, &blendMode);
            GFX_Set(GFXF_DRAW_BLEND_MODE, blendMode | GFX_BLEND_CHANNEL);
            GFX_Set(GFXF_DRAW_ALPHA_ENABLE, GFX_TRUE);
            break;
        }
        default:
            break;
    }        
       
    GFX_DrawPixel(imgReader->destPoint.x + imgReader->col,
                  imgReader->destPoint.y + imgReader->row);
        
    imgReader->col++;
   
    if(imgReader->col == 40 && imgReader->row == 140)
    {
        imgReader->col = 40;
    }
    
    if(imgReader->col == imgReader->col_max)
    {
        imgReader->row++;
        imgReader->col = 0;
    }
    
    if(imgReader->row == imgReader->row_max)
    {
        imgReader->header.state = CLEANUP;
    }
    else
    {
        imgReader->header.state = PREPARE_IMAGE_DATA_REQUEST;   
    }

    return GFX_SUCCESS;
}

static void paletteReadRequestCompleted(GFXU_ImageAssetReader* imgReader)
{
    imgReader->header.state = DRAW_PALETTE_DATA;
    imgReader->header.status = GFXU_READER_STATUS_DRAWING;
}

static GFX_Result requestPaletteData(GFXU_ImageAssetReader* imgReader)
{
    void* address;
    uint32_t offs;
    
    // already have the requested data from previous read
    if(imgReader->paletteIndex == imgReader->paletteCache)
    {
        imgReader->header.state = DRAW_PALETTE_DATA;
        imgReader->header.status = GFXU_READER_STATUS_DRAWING;
    
        return GFX_SUCCESS;
    }
    
    imgReader->paletteCache = imgReader->paletteIndex;
    
    offs = getOffsetFromIndexAndBPP(imgReader->paletteIndex,
                                    GFX_ColorModeInfoGet(imgReader->img->palette->colorMode).bppOrdinal);
    
    address = imgReader->img->palette->header.dataAddress;
    address = (void*)(((uint8_t*)address) + offs);

    if(imgReader->header.memIntf->read((GFXU_ExternalAssetReader*)imgReader,
                                       (GFXU_AssetHeader*)imgReader->img->palette,
                                       address,
                                       GFX_ColorModeInfoGet(imgReader->img->palette->colorMode).size,
                                       (uint8_t*)&imgReader->paletteColor,
                                       (GFXU_MediaReadRequestCallback_FnPtr)&paletteReadRequestCompleted) == GFX_FAILURE)
    {
        imgReader->header.state = CLEANUP; 
        
        return GFX_FAILURE;
    }
        
    if(imgReader->header.status != GFXU_READER_STATUS_DRAWING)
    {
        imgReader->header.status = GFXU_READER_STATUS_WAITING;
        imgReader->header.state = WAITING_FOR_PALETTE_DATA;
    }
    
    return GFX_SUCCESS;
}

static GFX_Result drawPaletteData(GFXU_ImageAssetReader* imgReader)
{       
    GFX_ColorMode colorMode;
    
    GFX_Get(GFXF_COLOR_MODE, &colorMode);
    
    GFX_Set(GFXF_DRAW_COLOR, GFX_ColorConvert(imgReader->img->palette->colorMode,
                                              colorMode,
                                              imgReader->paletteColor));
    
    // enable mask color if necessary
    if((imgReader->img->flags & GFXU_IMAGE_USE_MASK) > 0)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_TRUE);
        GFX_Set(GFXF_DRAW_MASK_VALUE, GFX_ColorConvert(imgReader->img->palette->colorMode,
                                                       colorMode,
                                                       imgReader->img->mask));
    }
    else
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    }
    
    GFX_DrawPixel(imgReader->destPoint.x + imgReader->col,
                  imgReader->destPoint.y + imgReader->row);
        
    imgReader->col++;
   
    if(imgReader->col == imgReader->col_max)
    {
        imgReader->row++;
        imgReader->col = 0;
    }
    
    if(imgReader->row == imgReader->row_max)
    {
        imgReader->header.state = CLEANUP;
    }
    else
    {
        imgReader->header.state = PREPARE_IMAGE_DATA_REQUEST;   
    }

    return GFX_SUCCESS;
}

static void cleanup(GFXU_ImageAssetReader* imgReader)
{
    imgReader->header.memIntf->close((GFXU_AssetHeader*)imgReader->img);
    
    imgReader->header.status = GFXU_READER_STATUS_FINISHED;
    imgReader->header.state = DONE;
}

static GFX_Result run(GFXU_ExternalAssetReader* reader)
{
    GFXU_ImageAssetReader* imgReader = (GFXU_ImageAssetReader*)reader;
    
    if(imgReader == GFX_NULL)
        return GFX_FAILURE;
    
    switch(imgReader->header.state)
    {
        case PREPARE_IMAGE_DATA_REQUEST:
            return calculateImageIndex(imgReader);
        case ATTEMPT_IMAGE_DATA_REQUEST:
            return requestImageData(imgReader);
        case DRAW_IMAGE_DATA:
            return drawImageData(imgReader);
        case ATTEMPT_PALETTE_DATA_REQUEST:
            return requestPaletteData(imgReader);
        case DRAW_PALETTE_DATA:
            return drawPaletteData(imgReader);
        case CLEANUP:
            cleanup(imgReader);
        case WAITING_FOR_IMAGE_DATA:
        case WAITING_FOR_PALETTE_DATA:
        default:
            return GFX_SUCCESS;
    }
}

GFX_Result GFXU_DrawImageRawExternal(GFXU_ImageAsset* img,
                                     int32_t src_x,
                                     int32_t src_y,
                                     int32_t src_width,
                                     int32_t src_height,
                                     int32_t dest_x,
                                     int32_t dest_y,
                                     GFXU_MemoryIntf* memIntf,
                                     GFXU_ExternalAssetReader** reader)
{
    GFXU_ImageAssetReader* imgReader;
    
    if(memIntf == NULL || reader == NULL)
        return GFX_FAILURE;
        
    if(*reader != NULL)
        memIntf->heap.free(*reader);
    
    imgReader = (GFXU_ImageAssetReader*)memIntf->heap.calloc(1, 
                                             sizeof(GFXU_ImageAssetReader));
    
    if(imgReader == GFX_NULL)
        return GFX_FAILURE;
    
    imgReader->header.memIntf = memIntf;
    imgReader->header.run = &run;
    imgReader->header.status = GFXU_READER_STATUS_READY;
    imgReader->header.state = PREPARE_IMAGE_DATA_REQUEST;
    
    imgReader->img = img;
    
    imgReader->sourceRect.x = src_x;
    imgReader->sourceRect.y = src_y;
    imgReader->sourceRect.width = src_width;
    imgReader->sourceRect.height = src_height;
    
    imgReader->destPoint.x = dest_x;
    imgReader->destPoint.y = dest_y;
    
    imgReader->row_max = src_height;
    imgReader->col_max = src_width;
    
    if(imgReader->img->palette != GFX_NULL)
    {
        imgReader->paletteIndex = 0;
        imgReader->paletteCache = -1;
    }
    
    imgReader->header.memIntf->open((GFXU_AssetHeader*)img);
    
    *reader = (GFXU_ExternalAssetReader*)imgReader;

    return GFX_SUCCESS;
}
