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


#include "gfx/utils/inc/gfxu_image_utils.h"

#define RLE_HEADER_SIZE 2

uint32_t getRLEDataAtIndex(uint8_t* data,
                           uint32_t max,
                           uint32_t idx,
                           uint32_t* startBlock,
                           uint32_t* startOffset)
{
    uint32_t rleRunLength;
    uint32_t rleDataSize;
    uint32_t length = 0;
    uint32_t val = 0;
    uint32_t offset = 0;
    uint32_t i = 0;
    
    rleRunLength = (uint32_t)data[0];
    rleDataSize = (uint32_t)data[1];
    
    data += RLE_HEADER_SIZE + ((*startBlock) * (rleRunLength + rleDataSize));
    offset = *startOffset;
    
    while(offset < max)
    {
        // get the length
        for(i = 0; i < rleRunLength; i++)
            ((uint8_t*)&length)[i] = data[i];
        
        // index is in this range, return value
        if(idx < offset + length)
        {
            // get the value
            for(i = 0; i < rleDataSize; i++)
                ((uint8_t*)&val)[i] = data[i + rleRunLength];
            
            return val;
        }
            
        data += rleRunLength + rleDataSize;
            
        offset += length;
        
        *startBlock += 1;
        *startOffset = offset;
    }
    
    return 0; 
}

uint32_t getDiscreteValueAtIndex1(uint32_t idx, uint32_t val)
{
    uint32_t offs;
    
    offs = idx % 8;
    idx = val & (0x80 >> offs);
    idx >>= 7 - offs;
    
    return idx;
}

uint32_t getDiscreteValueAtIndex4(uint32_t idx, uint32_t val)
{
    if(idx % 2 > 0)
        idx = val & 0xF;
    else
        idx = (val & 0xF0) >> 0x4;
    
    return idx;
}

uint32_t getDiscreteValueAtIndex8(uint32_t idx, uint32_t val)
{
    return val;
}

typedef uint32_t (*getDiscreteValue_FnPtr)(uint32_t, uint32_t);

getDiscreteValue_FnPtr getDiscreteValue[GFX_COLOR_MODE_COUNT] =
{
    &getDiscreteValueAtIndex8,
    &getDiscreteValueAtIndex8,
    &getDiscreteValueAtIndex8,
    &getDiscreteValueAtIndex8,
    &getDiscreteValueAtIndex8,
    &getDiscreteValueAtIndex8,
    &getDiscreteValueAtIndex8,
    &getDiscreteValueAtIndex8,
    &getDiscreteValueAtIndex1,
    &getDiscreteValueAtIndex4,
    &getDiscreteValueAtIndex8,
};

uint32_t getDiscreteValueAtIndex(uint32_t idx, uint32_t val, GFX_ColorMode mode)
{
    /*uint32_t offs;
    
    if(mode == GFX_COLOR_MODE_INDEX_1)
    {
        offs = idx % 8;
        idx = val & (0x80 >> offs);
        idx >>= 7 - offs;
    }
    else if(mode == GFX_COLOR_MODE_INDEX_4)
    {
        if(idx % 2 > 0)
        {
            offs = 0xF;
            idx = val & offs;
        }
        else
        {
            offs = 0xF0;
            idx = (val & offs) >> 0x4;
        }
    }
    else
        idx = val;
        
    return idx;*/
    
    return getDiscreteValue[mode](idx, val);
}

static uint32_t calcOffset1bpp(uint32_t index)
{
    return index >> 3;
}

static uint32_t calcOffset4bpp(uint32_t index)
{
    return index >> 1;
}

static uint32_t calcOffset8bpp(uint32_t index)
{
    return index;
}

static uint32_t calcOffset16bpp(uint32_t index)
{
    return index * 2;
}

static uint32_t calcOffset24bpp(uint32_t index)
{
    return index * 3;
}

static uint32_t calcOffset32bpp(uint32_t index)
{
    return index * 4;
}

typedef uint32_t (*calcOffset_FnPtr)(uint32_t);

calcOffset_FnPtr calcOffset[] =
{
    &calcOffset1bpp,
    &calcOffset4bpp,
    &calcOffset8bpp,
    &calcOffset16bpp,
    &calcOffset24bpp,
    &calcOffset32bpp
};

uint32_t getOffsetFromIndexAndBPP(uint32_t index, GFX_BitsPerPixel bpp)
{
    return calcOffset[bpp](index);
}

GFX_Result convertColorAndSetDraw(uint32_t color, GFX_ColorMode mode)
{
    GFX_ColorMode colorMode;
    
    if(GFX_Get(GFXF_COLOR_MODE, &colorMode) == GFX_FAILURE)
        return GFX_FAILURE;
        
    return GFX_Set(GFXF_DRAW_COLOR, GFX_ColorConvert(mode, colorMode, color));
}

void createDefaultMemIntf(GFXU_MemoryIntf* memIntf)
{
    memIntf->heap.malloc = (GFX_Malloc_FnPtr) &malloc;
    memIntf->heap.free = (GFX_Free_FnPtr) &free;
    memIntf->heap.calloc = (GFX_Calloc_FnPtr) &calloc;
    memIntf->heap.realloc = (GFX_Realloc_FnPtr) &realloc;
    memIntf->heap.memset = (GFX_Memset_FnPtr) &memset;
    memIntf->heap.memcpy = (GFX_Memcpy_FnPtr) &memcpy;
    memIntf->heap.coherent_alloc = (GFX_Malloc_FnPtr) &malloc;
    memIntf->heap.coherent_free = (GFX_Free_FnPtr) &free;
    memIntf->read = NULL;
}