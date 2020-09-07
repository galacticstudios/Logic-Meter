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


#include "gfx/utils/inc/gfxu_string.h"

#include <stdlib.h>
#include <string.h>

#include "gfx/utils/inc/gfxu_font.h"
#include "gfx/utils/inc/gfxu_string_utils.h"

typedef enum imageReaderState_t
{
    NEXT_GLYPH,
    ATTEMPT_FONT_DATA_REQUEST,
    WAITING_FOR_FONT_DATA,
    DRAW_GLYPH_DATA,
    DONE
} drawState;

typedef struct GFXU_StringAssetReader_t
{
    GFXU_ExternalAssetReader header;
    
    GFXU_CHAR* buffer;
    uint32_t length;
    uint32_t index;
    
    GFXU_FontAsset* font;
    
    int32_t x;
    int32_t y;
    int32_t clipX;
    int32_t clipY;
    int32_t clipWidth;
    int32_t clipHeight;
    
    uint8_t stringData[GFXU_STRING_MAX_CHAR_WIDTH];
    
    uint32_t glyphOffset;
    uint32_t glyphRow;
    uint32_t glyphWidth;
    uint8_t* glyphDataAddress;
    uint8_t* glyphData;
    uint32_t glyphDataWidth;
    uint32_t glyphDataSize;
    uint32_t glyphDataCapacity;
} GFXU_StringAssetReader;

static void done(GFXU_StringAssetReader* strReader)
{
    strReader->header.state = DONE;
    strReader->header.status = GFXU_READER_STATUS_FINISHED;
    
    strReader->header.memIntf->close((GFXU_AssetHeader*)strReader->font);
        
    if(strReader->glyphData != NULL)
    {
        strReader->header.memIntf->heap.free(strReader->glyphData);
        strReader->glyphData = NULL;
    }
}

static GFX_Result prepareNextGlyph(GFXU_StringAssetReader* strReader)
{
    if(strReader->clipY - strReader->y > 0)
        strReader->glyphRow = strReader->clipY - strReader->y;
    else
        strReader->glyphRow = 0;
    
    // attempt to find the glyph raster data
    // if no data found then draw empty rectangle
    if(GFXU_FontGetGlyphInfo(strReader->font,
                             strReader->buffer[strReader->index],
                             &strReader->glyphOffset,
                             &strReader->glyphWidth) == GFX_FAILURE)
    {
        GFXU_DrawUnknownGlyph(strReader->x, 
                                     strReader->y + (strReader->font->height - (strReader->font->ascent + strReader->font->descent)),
                                     strReader->font->ascent + strReader->font->descent,
                                     strReader->clipX,
                                     strReader->clipY,
                                     strReader->clipWidth,
                                     strReader->clipHeight);
                              
        strReader->index++;
        
        if(strReader->index == strReader->length)
            done(strReader);
        
        return GFX_SUCCESS;
    }
    
    // calculate the size in bytes of a row of glyph pixels
    if(strReader->font->bpp == GFXU_FONT_BPP_1)
    {
        strReader->glyphDataSize = strReader->glyphWidth >> 3;
        
        if(strReader->glyphWidth % 8 > 0)
            strReader->glyphDataSize++;
    }
    else if(strReader->font->bpp == GFXU_FONT_BPP_8)
    {
        strReader->glyphDataSize = strReader->glyphWidth;
    }
    
    // adjust the glyph row data array
    if(strReader->glyphDataCapacity == 0 ||
       strReader->glyphDataCapacity < strReader->glyphDataSize)
    {
        // calculate sizes and capacities
        strReader->glyphDataCapacity = strReader->glyphDataSize;
        strReader->glyphData = strReader->header.memIntf->heap.realloc(strReader->glyphData,
                                                                       strReader->glyphDataCapacity);
    
        if(strReader->glyphData == GFX_NULL)
        {
            // failed to get the necessary memory, can't do anything else
            done(strReader);
            
            return GFX_FAILURE;
        }
    }
    
    // set the glyph address
    strReader->glyphDataAddress = ((uint8_t*)strReader->font->header.dataAddress) +
                                  strReader->glyphOffset;
    
    strReader->header.status = GFXU_READER_STATUS_READY;
    strReader->header.state = ATTEMPT_FONT_DATA_REQUEST;
    
    return GFX_SUCCESS;
}

static void fontDataRequestCompleted(GFXU_StringAssetReader* strReader)
{
    // got some font data, prepare to draw it
    strReader->header.status = GFXU_READER_STATUS_READY;
    strReader->header.state = DRAW_GLYPH_DATA; 
}

static GFX_Result requestFontData(GFXU_StringAssetReader* strReader)
{
    // read a chunk of font data
    if(strReader->header.memIntf->read((GFXU_ExternalAssetReader*)strReader,
                                       (GFXU_AssetHeader*)strReader->font,
                                       strReader->glyphDataAddress,
                                       strReader->glyphDataSize,
                                       (uint8_t*)strReader->glyphData,
                                       (GFXU_MediaReadRequestCallback_FnPtr)&fontDataRequestCompleted) == GFX_FAILURE)
    {
        return GFX_FAILURE;
    }
        
    if(strReader->header.state != DRAW_GLYPH_DATA)
    {
        // wait for the font data
        strReader->header.status = GFXU_READER_STATUS_WAITING;
        strReader->header.state = WAITING_FOR_FONT_DATA; 
    }
    
    return GFX_SUCCESS;                           
}

static GFX_Result drawGlyphRow(GFXU_StringAssetReader* strReader)
{
    int32_t clipX1, clipX2;
    
    // draw the glyph                      
    if(strReader->x >= strReader->clipX)
        clipX1 = 0;
    else
        clipX1 = strReader->clipX - strReader->x;
        
    if(strReader->x + (int32_t)strReader->glyphWidth <= strReader->clipX + strReader->clipWidth)
        clipX2 = strReader->glyphWidth;
    else
        clipX2 = strReader->glyphWidth + ((strReader->clipX + strReader->clipWidth) - (strReader->x + (int32_t)strReader->glyphWidth));
                      
    GFXU_DrawGlyphRow(strReader->font->bpp,
                      strReader->glyphData,
                      strReader->glyphWidth,
                      strReader->x,
                      strReader->y + strReader->glyphRow,
                      clipX1,
                      clipX2);
    
    // prepare for next string data read
    strReader->glyphRow += 1;
    
    // check to see if the entire glyph has been rendered
    if(strReader->glyphRow == strReader->font->height ||
       (int32_t)strReader->glyphRow + strReader->y >= strReader->clipY + strReader->clipHeight)
    {
        // advance draw point
        strReader->x += strReader->glyphWidth;
        
        strReader->index++;
        
        // done decoding entire string?
        if(strReader->index >= strReader->length)
        {
            done(strReader);
        }
        else
        {
            // read some more glyph data
            strReader->header.state = NEXT_GLYPH;
            strReader->header.status = GFXU_READER_STATUS_READY;
        }
        
        return GFX_SUCCESS;
    }
    
    // advance the glyph data address to the next row
    strReader->glyphDataAddress = ((uint8_t*)strReader->font->header.dataAddress) +
                                  strReader->glyphOffset +
                                  (strReader->glyphDataSize * strReader->glyphRow);
    
    // read some more glyph data
    strReader->header.state = ATTEMPT_FONT_DATA_REQUEST;
    strReader->header.status = GFXU_READER_STATUS_READY;
    
    return GFX_SUCCESS;                             
}

static GFX_Result run(GFXU_ExternalAssetReader* reader)
{
    GFXU_StringAssetReader* strReader = (GFXU_StringAssetReader*)reader;
    
    if(strReader == GFX_NULL)
        return GFX_FAILURE;
    
    switch(strReader->header.state)
    {
        case NEXT_GLYPH:
            return prepareNextGlyph(strReader);
        case ATTEMPT_FONT_DATA_REQUEST:
            return requestFontData(strReader);
        case DRAW_GLYPH_DATA:
            return drawGlyphRow(strReader);
        case WAITING_FOR_FONT_DATA:
        default:
            return GFX_SUCCESS;
    }
}

GFX_Result GFXU_DrawStringBufferExternal(GFXU_CHAR* str,
                                         GFXU_FontAsset* fnt,
                                         int32_t clipX,
                                         int32_t clipY,
                                         int32_t clipWidth,
                                         int32_t clipHeight,
                                         int32_t x,
                                         int32_t y,
                                         GFXU_MemoryIntf* memIntf,
                                         GFXU_ExternalAssetReader** reader)
{
    GFXU_StringAssetReader* strReader;
    uint32_t len = 0;
    
    while(str[len] != '\0')
        len++;
        
    if(len == 0)
        return GFX_FAILURE;
    
    // create the reader state manager object
    strReader = (GFXU_StringAssetReader*)memIntf->heap.calloc(1, 
                                             sizeof(GFXU_StringAssetReader));
    
    if(strReader == GFX_NULL)
        return GFX_FAILURE;
        
    // set reader state manager member variables
    strReader->header.memIntf = memIntf;
    strReader->header.run = &run;
    strReader->header.status = GFXU_READER_STATUS_READY;
    
    strReader->buffer = str;
    strReader->index = 0;
    strReader->length = len;
    strReader->font = fnt;
    
    strReader->x = x;
    strReader->y = y;
    strReader->clipX = clipX;
    strReader->clipY = clipY;
    strReader->clipWidth = clipWidth;
    strReader->clipHeight = clipHeight;
    
    strReader->header.state = NEXT_GLYPH;
    
    memIntf->open((GFXU_AssetHeader*)fnt);
    
    // return the reader as a generic pointer
    *reader = (GFXU_ExternalAssetReader*)strReader;

    return GFX_SUCCESS;
}

GFX_Result GFXU_DrawSubStringBufferExternal(GFXU_CHAR* str,
                                         GFXU_FontAsset* fnt,
							             uint32_t start,
                                         uint32_t end,        
                                         int32_t clipX,
                                         int32_t clipY,
                                         int32_t clipWidth,
                                         int32_t clipHeight,
                                         int32_t x,
                                         int32_t y,
                                         GFXU_MemoryIntf* memIntf,
                                         GFXU_ExternalAssetReader** reader)
{
    GFXU_StringAssetReader* strReader;
    uint32_t len = 0;
    
    while(str[len] != '\0')
        len++;
        
    if(len == 0)
        return GFX_FAILURE;
    
    len = (end < len) ? end : len;
    
    // create the reader state manager object
    strReader = (GFXU_StringAssetReader*)memIntf->heap.calloc(1, 
                                             sizeof(GFXU_StringAssetReader));
    
    if(strReader == GFX_NULL)
        return GFX_FAILURE;
        
    // set reader state manager member variables
    strReader->header.memIntf = memIntf;
    strReader->header.run = &run;
    strReader->header.status = GFXU_READER_STATUS_READY;
    
    strReader->buffer = str;
    strReader->index = start;
    strReader->length = len;
    strReader->font = fnt;
    
    strReader->x = x;
    strReader->y = y;
    strReader->clipX = clipX;
    strReader->clipY = clipY;
    strReader->clipWidth = clipWidth;
    strReader->clipHeight = clipHeight;
    
    strReader->header.state = NEXT_GLYPH;
    
    memIntf->open((GFXU_AssetHeader*)fnt);
    
    // return the reader as a generic pointer
    *reader = (GFXU_ExternalAssetReader*)strReader;

    return GFX_SUCCESS;
}