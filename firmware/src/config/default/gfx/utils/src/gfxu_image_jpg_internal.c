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


#include "gfx/utils/inc/gfxu_image_jpg_common.h"

static uint32_t internalMemoryRead(void* ptr, uint32_t size, uint32_t n, JPEGDECODER* decoder)
{
    uint32_t count = 0;
    uint8_t* pFile;
    uint8_t* bptr;
    
    pFile = (uint8_t*)decoder->pImageFile->header.dataAddress;
    bptr = (uint8_t *)ptr;
    
    for(count = 0; count < size * n; ++count)
    {
        bptr[count] = (uint8_t) pFile[decoder->fileIndex];
        ++decoder->fileIndex;
    }        
    
    return count;
}

GFX_Result GFXU_DrawImageJpgInternal(GFXU_ImageAsset* img,
                                     int32_t src_x,
                                     int32_t src_y,
                                     int32_t src_width,
                                     int32_t src_height,
                                     int32_t dest_x,
                                     int32_t dest_y,
                                     GFXU_MemoryIntf* memIntf)
{
    static uint16_t whblocks, wvblocks;
    static uint16_t wi, wj;
    static JPEGDECODER JPEG_JpegDecoder;
    GFX_Rect rect;
    
    static enum
    {
        INITIAL,
        HEADER_DECODED,
        BLOCK_DECODE,
        BLOCK_PAINT,
        DECODE_DONE
    } decodestate = INITIAL;
    
    if(memIntf == GFX_NULL)
        return GFX_FAILURE;

    //JPEG masking is not supported
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);

    // jpeg decoder doesn't handle clipped images, enable clipping
    // and draw the entire image
    if(src_x != 0 ||
       src_y != 0 ||
       src_width <= img->width ||
       src_height <= img->height)
    {
        rect.x = dest_x;
        rect.y = dest_y;
        rect.width = src_width;
        rect.height = src_height;
        
        GFX_Set(GFXF_DRAW_CLIP_RECT, &rect);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
        
        dest_x -= src_x;
        dest_y -= src_y;
        src_x = 0;
        src_y = 0;
        src_width = img->width;
        src_height = img->height;
    }
    
    switch(decodestate)
    {    
        case INITIAL:                        
            JPEG_vResetDecoder(&JPEG_JpegDecoder);

            JPEG_JpegDecoder.pImageFile = img;
            JPEG_JpegDecoder.readPtr = &internalMemoryRead;
            JPEG_JpegDecoder.memIntf = memIntf;
            JPEG_JpegDecoder.wStartY = src_y;
            JPEG_JpegDecoder.wStartX = src_x;
            JPEG_JpegDecoder.wDrawWidth = src_width;
            JPEG_JpegDecoder.wDrawHeight = src_height;
            JPEG_JpegDecoder.wDrawX = dest_x;
            JPEG_JpegDecoder.wDrawY = dest_y;

            if(JPEG_bReadHeader(&JPEG_JpegDecoder) != 0)
            {
                //IMG_vImageFail();
                return GFX_FAILURE;
            }
            decodestate = HEADER_DECODED;
        
        case HEADER_DECODED:
//            IMG_wImageWidth = JPEG_JpegDecoder.wWidth;
//            IMG_wImageHeight = JPEG_JpegDecoder.wHeight;
//            IMG_vSetboundaries();

            JPEG_bGenerateHuffmanTables(&JPEG_JpegDecoder);

            whblocks = JPEG_JpegDecoder.wWidth >> 3;
            wvblocks = JPEG_JpegDecoder.wHeight >> 3;

            if((whblocks << 3) < JPEG_JpegDecoder.wWidth) /* Odd sizes */
            {
                whblocks++;
            }

            if((wvblocks << 3) < JPEG_JpegDecoder.wHeight) /* Odd sizes */
            {
                wvblocks++;
            }

            if(JPEG_JpegDecoder.bSubSampleType == JPEG_SAMPLE_1x2)
            {
                wvblocks =  (wvblocks>>1) + (wvblocks&1);
            }
            else if(JPEG_JpegDecoder.bSubSampleType == JPEG_SAMPLE_2x1)
            {
                whblocks = (whblocks>>1) + (whblocks&1);
            }
            else if(JPEG_JpegDecoder.bSubSampleType == JPEG_SAMPLE_2x2)
            {
                wvblocks =  (wvblocks>>1) + (wvblocks&1);
                whblocks = (whblocks>>1) + (whblocks&1);
            }

            JPEG_vInitDisplay(&JPEG_JpegDecoder);

            wi = 0;
            wj = 0;

            decodestate = BLOCK_DECODE;

        case BLOCK_DECODE:  
            while(wi < whblocks)
            {
                JPEG_bDecodeOneBlock(&JPEG_JpegDecoder); /* Fills a block after correcting the zigzag, dequantizing, IDCR and color conversion to RGB */

                decodestate = BLOCK_PAINT;
        case BLOCK_PAINT:
                if(JPEG_bPaintOneBlock(&JPEG_JpegDecoder))   /* Sends the block to the Graphics unit */
                {
                    //IMG_vImageFail();
                    return GFX_FAILURE; //Painting not finished
                }

                wj++;

                if(wj >= wvblocks)
                {
                    wj = 0;
                    wi++;
                }
            }

            //IMG_vImageComplete();
            decodestate = INITIAL;
        default:            
            return GFX_SUCCESS;
    }
    return GFX_SUCCESS;
}
