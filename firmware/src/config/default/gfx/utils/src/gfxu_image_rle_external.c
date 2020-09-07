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

#include "gfx/utils/inc/gfxu_palette.h"
#include "gfx/utils/inc/gfxu_image_utils.h"

#define RLE_HEADER_SIZE 2
#define RLE_BLOCK_SIZE_MAX 8

typedef enum imageReaderState_t
{
    ATTEMPT_RLE_HEADER_DATA_REQUEST,
    WAITING_FOR_RLE_HEADER_DATA,
    PREPARE_RLE_DATA_REQUEST,
    REPREPARE_RLE_DATA_REQUEST,
    ATTEMPT_RLE_DATA_REQUEST,
    WAITING_FOR_RLE_DATA,
    DRAW_RLE_DATA,
    PREPARE_PALETTE_DATA_REQUEST,
    ATTEMPT_PALETTE_DATA_REQUEST,
    WAITING_FOR_PALETTE_DATA,
    DRAW_PALETTE_DATA,
    CLEANUP,
    DONE
} drawState;

typedef struct GFXU_RLEAssetReader_t
{
    GFXU_ExternalAssetReader header;
    
    GFXU_ImageAsset* img;
    
    GFX_Rect sourceRect;
    GFX_Point destPoint;
    
    uint32_t row_max;
    uint32_t col_max;
    
    uint32_t row;
    uint32_t col;
    
    uint32_t rleLengthSize;
    uint32_t rleDataSize;
    
    //uint32_t addressOffset;
    
    uint8_t rleBlock[RLE_BLOCK_SIZE_MAX];
    
    uint32_t rleLength;
    uint32_t rleData;
    uint32_t rleIndexOffset;
    uint32_t rleBlockOffset;
    
    //GFX_BitsPerPixel imageBPPOrdinal;
    uint32_t imageIndex;
    //uint32_t imageIndexSize;
    //uint32_t imageReadSize;
    
    //GFX_BitsPerPixel paletteBPPOrdinal;
    uint32_t paletteIndex;
    uint32_t paletteCache;
    //uint32_t paletteIndexSize;
    //uint32_t paletteReadSize;
    GFX_Color paletteColor;
    
} GFXU_RLEAssetReader;

static void headerReadRequestCompleted(GFXU_RLEAssetReader* imgReader)
{
    imgReader->rleDataSize = (imgReader->rleLengthSize & 0xFF00) >> 8;
    imgReader->rleLengthSize = (imgReader->rleLengthSize & 0xFF);
    
    imgReader->header.status = GFXU_READER_STATUS_READY;
    imgReader->header.state = PREPARE_RLE_DATA_REQUEST;
}

static GFX_Result requestHeaderData(GFXU_RLEAssetReader* imgReader)
{
    if(imgReader->header.memIntf->read((GFXU_ExternalAssetReader*)imgReader,
                                       (GFXU_AssetHeader*)imgReader->img,
                                       imgReader->img->header.dataAddress,
                                       RLE_HEADER_SIZE,
                                       (uint8_t*)&imgReader->rleLengthSize,
                                       (GFXU_MediaReadRequestCallback_FnPtr)&headerReadRequestCompleted) == GFX_FAILURE)
    {
        imgReader->header.state = CLEANUP; 
        
        return GFX_FAILURE;
    }
        
    if(imgReader->header.state != PREPARE_RLE_DATA_REQUEST)
    {
        imgReader->header.status = GFXU_READER_STATUS_WAITING;
        imgReader->header.state = WAITING_FOR_RLE_HEADER_DATA; 
    }
    
    return GFX_SUCCESS;                             
}

static GFX_Result calculateImageIndex(GFXU_RLEAssetReader* imgReader)
{
    imgReader->imageIndex = imgReader->sourceRect.x + imgReader->col + 
                           ((imgReader->sourceRect.y + imgReader->row) * imgReader->img->width);

    imgReader->header.state = ATTEMPT_RLE_DATA_REQUEST;
    
    return GFX_SUCCESS;
}

static void rleReadRequestCompleted(GFXU_RLEAssetReader* imgReader)
{
    uint32_t i;
    uint32_t offs;
    
    imgReader->rleLength = 0;
    imgReader->rleData = 0;
    
    for(i = 0; i < imgReader->rleLengthSize; i++)
        imgReader->rleLength |= imgReader->rleBlock[i] << (i * 8);
        
    for(i = 0; i < imgReader->rleDataSize; i++)
        imgReader->rleData |= imgReader->rleBlock[i+imgReader->rleLengthSize] << (i * 8);
        
    
    if(GFX_COLOR_MODE_IS_INDEX(imgReader->img->colorMode))
        offs = getOffsetFromIndexAndBPP(imgReader->imageIndex,
                                        GFX_ColorModeInfoGet(imgReader->img->colorMode).bppOrdinal);
    else
        offs = imgReader->imageIndex;
    
    // index was not in this RLE block, request the next block                                       
    if(offs >= imgReader->rleIndexOffset + imgReader->rleLength)
    {
        imgReader->rleIndexOffset += imgReader->rleLength;
        imgReader->rleBlockOffset += 1;
        
        imgReader->header.status = GFXU_READER_STATUS_READY;
        imgReader->header.state = REPREPARE_RLE_DATA_REQUEST;
        
        return;
    }
    
    if(imgReader->img->palette != NULL)
    {    
        imgReader->paletteIndex = getDiscreteValueAtIndex(imgReader->imageIndex,
                                                          imgReader->rleData,
                                                          imgReader->img->colorMode);
 
        imgReader->header.state = ATTEMPT_PALETTE_DATA_REQUEST;
        imgReader->header.status = GFXU_READER_STATUS_READY;
    }
    else
    {
        imgReader->header.state = DRAW_RLE_DATA;
        imgReader->header.status = GFXU_READER_STATUS_DRAWING;
    }
}

static GFX_Result requestRLEData(GFXU_RLEAssetReader* imgReader)
{
    void* address;
    uint32_t offs;
           
    offs = getOffsetFromIndexAndBPP(imgReader->imageIndex, 
                                    GFX_ColorModeInfoGet(imgReader->img->colorMode).bppOrdinal);
    
    if(offs > imgReader->rleIndexOffset &&
       offs <= imgReader->rleIndexOffset + imgReader->rleLength)
    {
        rleReadRequestCompleted(imgReader);
        
        return GFX_SUCCESS;
    }
    
    address = imgReader->img->header.dataAddress;
    address = (void*)(((uint8_t*)address) + 
                       RLE_HEADER_SIZE +
                      (imgReader->rleBlockOffset * (imgReader->rleLengthSize + imgReader->rleDataSize)));

    imgReader->header.state = WAITING_FOR_RLE_DATA;
    
    if(imgReader->header.memIntf->read((GFXU_ExternalAssetReader*)imgReader,
                                       (GFXU_AssetHeader*)imgReader->img,
                                       address,
                                       imgReader->rleLengthSize + imgReader->rleDataSize,
                                       imgReader->rleBlock,
                                       (GFXU_MediaReadRequestCallback_FnPtr)&rleReadRequestCompleted) == GFX_FAILURE)
    {
        imgReader->header.state = CLEANUP; 
        
        return GFX_FAILURE;
    }
        
    if(imgReader->header.state == PREPARE_RLE_DATA_REQUEST)
    {
        imgReader->header.status = GFXU_READER_STATUS_WAITING;
        imgReader->header.state = WAITING_FOR_RLE_DATA; 
    }
    
    return GFX_SUCCESS;                             
}

static GFX_Result drawRLEData(GFXU_RLEAssetReader* imgReader)
{
    GFX_ColorMode colorMode;
    GFX_Color color;
    
    GFX_Get(GFXF_COLOR_MODE, &colorMode);
    
    color = GFX_ColorConvert(imgReader->img->colorMode,
                             colorMode,
                             imgReader->rleData);
    
    GFX_Set(GFXF_DRAW_COLOR, color);
    
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
        imgReader->header.state = PREPARE_RLE_DATA_REQUEST;   
    }

    return GFX_SUCCESS;
}

static void paletteReadRequestCompleted(GFXU_RLEAssetReader* imgReader)
{
    imgReader->header.state = DRAW_PALETTE_DATA;
    imgReader->header.status = GFXU_READER_STATUS_DRAWING;
}

static GFX_Result drawPaletteData(GFXU_RLEAssetReader* imgReader)
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
        imgReader->header.status = GFXU_READER_STATUS_READY;
        imgReader->header.state = PREPARE_RLE_DATA_REQUEST;   
    }

    return GFX_SUCCESS;
}

static GFX_Result requestPaletteData(GFXU_RLEAssetReader* imgReader)
{
    void* address;
    uint32_t offs;
    
    // already have the requested data from previous read
    if(imgReader->paletteIndex == imgReader->paletteCache)
    {        
        drawPaletteData(imgReader);
    
        return GFX_SUCCESS;
    }
    
    imgReader->paletteCache = imgReader->paletteIndex;
    
    offs = getOffsetFromIndexAndBPP(imgReader->paletteIndex,
                                    GFX_ColorModeInfoGet(imgReader->img->palette->colorMode).bppOrdinal);
    
    address = imgReader->img->palette->header.dataAddress;
    address = (void*)(((uint8_t*)address) + offs);

    imgReader->paletteColor = 0;

    if(imgReader->header.memIntf->read((GFXU_ExternalAssetReader*)imgReader,
                                       (GFXU_AssetHeader*)imgReader->img,
                                       address,
                                       GFX_ColorModeInfoGet(imgReader->img->palette->colorMode).size,
                                       (uint8_t*)&imgReader->paletteColor,
                                       (GFXU_MediaReadRequestCallback_FnPtr)&paletteReadRequestCompleted) == GFX_FAILURE)
    {
        imgReader->header.state = CLEANUP; 
        
        return GFX_FAILURE;
    }
        
    if(imgReader->header.state != DRAW_PALETTE_DATA)
    {
        imgReader->header.status = GFXU_READER_STATUS_WAITING;
        imgReader->header.state = WAITING_FOR_PALETTE_DATA;
    }
    
    return GFX_SUCCESS;
}

static void cleanup(GFXU_RLEAssetReader* imgReader)
{
    imgReader->header.memIntf->close((GFXU_AssetHeader*)imgReader->img);
    
    imgReader->header.status = GFXU_READER_STATUS_FINISHED;
    imgReader->header.state = DONE;
}

static GFX_Result run(GFXU_ExternalAssetReader* reader)
{
    GFXU_RLEAssetReader* imgReader = (GFXU_RLEAssetReader*)reader;
    
    if(imgReader == GFX_NULL)
        return GFX_FAILURE;
    
    switch(imgReader->header.state)
    {
        case ATTEMPT_RLE_HEADER_DATA_REQUEST:
            return requestHeaderData(imgReader);
        case PREPARE_RLE_DATA_REQUEST:
        case REPREPARE_RLE_DATA_REQUEST:
            return calculateImageIndex(imgReader);
        case ATTEMPT_RLE_DATA_REQUEST:
            return requestRLEData(imgReader);
        case DRAW_RLE_DATA:
            return drawRLEData(imgReader);
        case ATTEMPT_PALETTE_DATA_REQUEST:
            return requestPaletteData(imgReader);
        case DRAW_PALETTE_DATA:
            return drawPaletteData(imgReader);
        case CLEANUP:
            cleanup(imgReader);
        case WAITING_FOR_RLE_DATA:
        case WAITING_FOR_PALETTE_DATA:
        default:
            return GFX_SUCCESS;
    }
}

GFX_Result GFXU_DrawImageRLEExternal(GFXU_ImageAsset* img,
                                     int32_t src_x,
                                     int32_t src_y,
                                     int32_t src_width,
                                     int32_t src_height,
                                     int32_t dest_x,
                                     int32_t dest_y,
                                     GFXU_MemoryIntf* memIntf,
                                     GFXU_ExternalAssetReader** reader)
{
    GFXU_RLEAssetReader* imgReader;
    
    if(memIntf == NULL || reader == NULL)
        return GFX_FAILURE;
    
    if(*reader != NULL)
        memIntf->heap.free(*reader);
    
    imgReader = (GFXU_RLEAssetReader*)memIntf->heap.calloc(1, 
                                             sizeof(GFXU_RLEAssetReader));
    
    if(imgReader == GFX_NULL)
        return GFX_FAILURE;
    
    imgReader->header.memIntf = memIntf;
    imgReader->header.run = &run;
    imgReader->header.status = GFXU_READER_STATUS_READY;
    imgReader->header.state = ATTEMPT_RLE_HEADER_DATA_REQUEST;
    
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
    
    imgReader->header.memIntf->open((GFXU_AssetHeader*)imgReader->img);
    
    *reader = (GFXU_ExternalAssetReader*)imgReader;

    return GFX_SUCCESS;
}
