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


#include "gfx/utils/inc/gfxu_string_utils.h"
#include "gfx/utils/inc/gfxu_font.h"

uint32_t GFXU_ExtractString(GFXU_StringTableAsset* tbl,
                            uint32_t id,
                            uint32_t lang,
                            GFXU_CHAR* buffer,
                            uint32_t size,
                            uint32_t offset)
{
    uint16_t idx;
    uint8_t* stringAddress;
    uint32_t stringSize;
    uint32_t codePoint;
    uint32_t tblOffset;
    
    uint32_t i, j;
    
    if(tbl == GFX_NULL || 
       buffer == NULL ||
       size == 0)
        return 0;
    
    stringSize = GFXU_GetStringSizeInBytes(tbl, id, lang);
    
    //if(stringSize > size)
    //    return 0;
    
    idx = GFXU_StringIndexLookup(tbl, id, lang);
    
    if(GFXU_StringLookup(tbl, idx, &stringAddress, &stringSize))
        return GFX_FAILURE;
        
    j = 0;
    
    buffer[size - 1] = '\0';
    
    for(i = 0; i < stringSize;)
    {
        if(GFXU_DecodeCodePoint(tbl->encodingMode,
                                stringAddress + i,
                                stringSize,
                                &codePoint,
                                &tblOffset) == GFX_FAILURE)
            return 0;
        
        i += tblOffset;
        
        // don't overrun buffer and leave last index for null
        if(j < size - 1)
            buffer[offset + j++] = codePoint;
        else
            return j;
        
    }
    
    j -= 1;
    
    return j;
}

int32_t GFXU_CompareString(GFXU_StringTableAsset* tbl,
                            uint32_t id,
                            uint32_t lang,
                            GFXU_CHAR* buffer)
{
    uint16_t idx;
    uint8_t* stringAddress;
    uint32_t stringSize, buffSize;
    uint32_t codePoint;
    uint32_t offset;
    
    uint32_t i, j;
    
    if(tbl == GFX_NULL)
        return -1;
    
    stringSize = GFXU_GetStringSizeInBytes(tbl, id, lang);
    
    buffSize = 0;
    
    if (buffer != GFX_NULL)
    {
        while(buffer[buffSize] != '\0')
            buffSize++;
    }

    if (stringSize == 0 && buffSize != 0)
        return -1;

    if (stringSize != 0 && buffSize == 0)
        return 1;

    idx = GFXU_StringIndexLookup(tbl, id, lang);

    if(GFXU_StringLookup(tbl, idx, &stringAddress, &stringSize))
        return GFX_FAILURE;

    j = 0;

    for(i = 0; i < stringSize && j < buffSize;)
    {
        if(GFXU_DecodeCodePoint(tbl->encodingMode,
                                stringAddress + i,
                                stringSize,
                                &codePoint,
                                &offset) == GFX_FAILURE)
            return -1;
            
        if(codePoint < buffer[j])
            return -1;
        else if(codePoint > buffer[j])
            return 1;
        else
            j++;
        
        i += offset;
    }
    
    if (i == stringSize && j == buffSize)
        return 0;
    else
        return -1;
}
                                        
uint16_t GFXU_StringIndexLookup(GFXU_StringTableAsset* table,
                                uint32_t stringID,
                                uint32_t languageID)
{
    uint8_t* indexTable = table->stringIndexTable;
    uint16_t* ptr;
    uint32_t indexWidth;
    uint32_t i;
    
    if(stringID >= table->stringCount || languageID >= table->languageCount)
        return GFX_NULL;
        
    // skip header
    indexTable += 3;
    
    indexWidth = indexTable[0];
    
    indexTable += 1;
    
    // skip to string index
    for(i = 0; i < stringID; i++)
        indexTable += (table->languageCount * indexWidth);
    
    if(indexWidth == 1)
    {
        i = indexTable[languageID];
        
        if(i == 0xFF)
            return 0xFFFF;
        else
            return i;
    }
    else
    {
        ptr = (uint16_t*)indexTable;
        
        return ptr[languageID];
    }
}

GFX_Result GFXU_StringLookup(GFXU_StringTableAsset* table,
                           uint32_t stringIndex,
                           uint8_t** stringAddress,
                           uint32_t* stringSize)
{
    uint32_t i;
    uint32_t stringCount, offset;
    uint8_t* data;
        
    if(stringIndex == 0xFFFF)
        return GFX_FAILURE;
        
    data = table->header.dataAddress;

    // get string count
    stringCount = data[0] + (data[1] << 8);

    data += 2;
    
    // find the string data in the string table
    for(i = 0; i < stringCount; i++)
    {
        if(i == stringIndex)
            break;
        
        offset = data[0] + (data[1] << 8);
        
        data += GFXU_STRING_ENTRY_SIZE + offset;
        
        /*word = (uint16_t*)address;
        
        offset = *word;
        offset += GFXU_STRING_ENTRY_SIZE;
    
        address += offset;*/
    }
    
    //word = (uint16_t*)address;
    *stringSize = data[0] + (data[1] << 8);
    
    data += GFXU_STRING_ENTRY_SIZE;
    //address += GFXU_STRING_ENTRY_SIZE;
    
    *stringAddress = data;
    
    return GFX_SUCCESS;
}
                                
GFXU_FontAsset* GFXU_StringFontIndexLookup(GFXU_StringTableAsset* table,
                                           uint32_t stringID,
                                           uint32_t languageID)
{
    uint8_t* indexTable;
    uint32_t i;
    
    if(table == NULL || 
       stringID >= table->stringCount || 
       languageID >= table->languageCount)
        return GFX_NULL;
    
    indexTable = table->fontIndexTable;
    
    // skip header
    indexTable += 3;
    
    // skip to string index
    for(i = 0; i < stringID; i++)
        indexTable += table->languageCount;
        
    i = indexTable[languageID];
    
    if(i == 0xFF)
        return GFX_NULL;
    
    return table->fontTable[i];
}
                                    

GFX_Result GFXU_FontGetLookupTableEntry(uint8_t* table,
                                        uint32_t index,
                                        uint32_t max,
                                        uint32_t* offset,
                                        uint32_t* width)
{
    uint32_t dataSize = table[0];
    uint32_t widthSize = table[1];
    uint32_t offs, wdth;
    uint32_t i;
    
    table += 2; // advance past table header data
    
    if(index >= max)
        return GFX_FAILURE;
    
    table += index * (dataSize + 1);
    
    offs = 0;
    
    for(i = 0; i < dataSize; i++)
        offs |= (table[i] << (8 * i)); 
    
    *offset = offs;
    
    table += dataSize;
    
    wdth = 0;
    
    for(i = 0; i < widthSize; i++)
        wdth |= (table[i] << (8 * i)); 
    
    *width = wdth;
    
    return GFX_SUCCESS;
}

GFX_Result GFXU_FontGetGlyphInfo(GFXU_FontAsset* fnt,
                                 uint32_t glyph,
                                 uint32_t* offset,
                                 uint32_t* width)
{
    uint32_t i;
    uint32_t idx = 0;
    uint32_t count;
    
    // attempt to find the glyph in the range lookup tables
    for(i = 0; i < fnt->indexTable->count; i++)
    {
        if(glyph >= fnt->indexTable->ranges[i].startID && glyph <= fnt->indexTable->ranges[i].endID)
        {
            idx = glyph - fnt->indexTable->ranges[i].startID;
            count = fnt->indexTable->ranges[i].endID - fnt->indexTable->ranges[i].startID + 1;
            
            return GFXU_FontGetLookupTableEntry(fnt->indexTable->ranges[i].lookupTable,
                                                idx,
                                                count,
                                                offset,
                                                width);
        }
    }
    
    // return width of unknown glyph rectangle
    *width = fnt->height >> 1;
    
    return GFX_FAILURE;
}

GFX_Result GFXU_DecodeASCII(uint8_t* val,
                            uint32_t max,
                            uint32_t* codePoint,
                            uint32_t* size)
{
    *codePoint = val[0];
    *size = 1;
    
    return GFX_SUCCESS;
}

GFX_Result GFXU_DecodeUTF8(uint8_t* val,
                           uint32_t max,
                           uint32_t* codePoint,
                           uint32_t* size)
{
    uint32_t point = 0;
    uint32_t length = 0;
    
    if((val[0] & 0x80) == 0x0)
    {
        length = 1;
        point = val[0] & 0x7F;
    }
    else if((val[0] & 0xE0) == 0xC0)
    {
        length = 2;
        
        if(max < 2)
            return GFX_FAILURE;
        
        point |= (val[0] & 0x1F) << 6;
        point |= (val[1] & 0x3F);
    } 
    else if((val[0] & 0xF0) == 0xE0)
    {
        length = 3;
        
        if(max < 3)
            return GFX_FAILURE;
        
        point |= (val[0] & 0xF) << 12;
        point |= (val[1] & 0x3F) << 6;
        point |= (val[2] & 0x3F);
    }
    else if((val[0] & 0xF8) == 0xF0)
    {
        length = 4;
        
        if(max < 4)
            return GFX_FAILURE;
        
        point |= (val[0] & 0x7) << 18;
        point |= (val[1] & 0x3F) << 12;
        point |= (val[2] & 0x3F) << 6;
        point |= (val[3] & 0x3F);
    }
    else if((val[0] & 0xFC) == 0xF8)
    {   
        length = 5;
        
        if(max < 5)
            return GFX_FAILURE;
        
        point |= (val[0] & 0x3) << 24;
        point |= (val[1] & 0x3F) << 18;
        point |= (val[2] & 0x3F) << 12;
        point |= (val[3] & 0x3F) << 6;
        point |= (val[4] & 0x3F);
    }
    else if((val[0] & 0xFE) == 0xFC)
    {
        length = 6;
        
        if(max < 6)
            return GFX_FAILURE;
        
        point |= (val[0] & 0x1) << 30;
        point |= (val[1] & 0x3F) << 24;
        point |= (val[2] & 0x3F) << 18;
        point |= (val[3] & 0x3F) << 12;
        point |= (val[4] & 0x3F) << 6;
        point |= (val[5] & 0x3F);
    }
    else
        return GFX_FAILURE;
    
    *size = length;
    *codePoint = point;
    
    return GFX_SUCCESS;
}

GFX_Result GFXU_DecodeUTF16(uint8_t* val,
                            uint32_t max,
                            uint32_t* codePoint,
                            uint32_t* size)
{
    uint32_t point = 0;
    uint32_t high, low;
    
    if((val[1] & 0xFC) == 0xDC)
    {
        if(max < 4)
            return GFX_FAILURE;
            
        *size = 4;
        
        high = val[0] << 8;
        high |= val[1];
        high -= 0xD800;
        
        low = val[2] << 8;
        low |= val[3];
        low -= 0xDC00;
        
        point = ((high << 10) + low) + 0x10000;
    }
    else
    {
        if(max < 2)
            return GFX_FAILURE;
            
        *size = 2;
        
        point += val[0];
        point += (val[1] << 8);
    }
    
    *codePoint = point;
    
    return GFX_SUCCESS;
}

typedef GFX_Result (*decodeGlyph_FnPtr)(uint8_t*,
                                        uint32_t,
                                        uint32_t*,
                                        uint32_t*);
                                         
decodeGlyph_FnPtr decodeGlyph[] =
{
    &GFXU_DecodeASCII,
    &GFXU_DecodeUTF8,
    &GFXU_DecodeUTF16
};

GFX_Result GFXU_DecodeCodePoint(uint32_t encoding,
                                uint8_t* data,
                                uint32_t max,
                                uint32_t* codePoint,
                                uint32_t* offset)
{
    return decodeGlyph[encoding](data, max, codePoint, offset);
}

GFX_Result GFXU_DecodeAndDrawString(uint8_t* string,
                                    uint32_t length,
                                    GFXU_StringEncodingMode mode,
                                    GFXU_FontAsset* fnt,
                                    int32_t clipX,
                                    int32_t clipY,
                                    int32_t clipWidth,
                                    int32_t clipHeight,
                                    int32_t x,
                                    int32_t y)
{
    uint32_t glyph, width;
    uint32_t i;
    
    for(i = 0; i < length;)
    {
        if(GFXU_DecodeCodePoint(mode,
                                &string[i],
                                length - i,
                                &glyph,
                                &width) == GFX_FAILURE)
            return GFX_FAILURE;
            
        i += width;
        
        x += GFXU_DrawGlyph(fnt,
                            glyph,
                            clipX,
                            clipY,
                            clipWidth,
                            clipHeight,
                            x,
                            y);
    }
    
    return GFX_SUCCESS;
}


GFX_Result GFXU_DecodeAndDrawSubString(uint8_t* string,
                                    uint32_t length,
                                    GFXU_StringEncodingMode mode,
                                    GFXU_FontAsset* fnt,
									uint32_t start,
                                    uint32_t end,
                                    int32_t clipX,
                                    int32_t clipY,
                                    int32_t clipWidth,
                                    int32_t clipHeight,
                                    int32_t x,
                                    int32_t y)
{
    uint32_t glyph, width;
    uint32_t i;
    
    length = (end != 0 && end < length) ? end : length;

    for(i = start; i < length;)
    {
        if(GFXU_DecodeCodePoint(mode,
                                &string[i],
                                length - i,
                                &glyph,
                                &width) == GFX_FAILURE)
            return GFX_FAILURE;
            
        i += width;
        
        x += GFXU_DrawGlyph(fnt,
                            glyph,
                            clipX,
                            clipY,
                            clipWidth,
                            clipHeight,
                            x,
                            y);
    }
    
    return GFX_SUCCESS;
}

int32_t GFXU_DrawGlyph(GFXU_FontAsset* fnt,
                       uint32_t glyph,
                       int32_t clipX,
                       int32_t clipY,
                       int32_t clipWidth,
                       int32_t clipHeight,
                       int32_t x,
                       int32_t y)
{
    uint32_t width;
    uint32_t offset;
    int32_t row;
    uint8_t* data;
    GFX_Rect glyphRect;
    GFX_Rect clipArea;
    GFX_Rect clippedGlyph;
    int32_t glyphDataWidth;
    int32_t clipX1, clipX2;
    
    // attempt to find the glyph raster data
    // if no data found then draw empty rectangle
    if(GFXU_FontGetGlyphInfo(fnt,
                             glyph,
                             &offset,
                             &width) == GFX_FAILURE)
    {
        return GFXU_DrawUnknownGlyph(x, 
                                     y + (fnt->height - (fnt->ascent + fnt->descent)),
                                     fnt->ascent + fnt->descent,
                                     clipX,
                                     clipY,
                                     clipWidth,
                                     clipHeight);
    }
    else
    {    
        glyphRect.x = x;
        glyphRect.y = y;
        glyphRect.width = width;
        glyphRect.height = (int32_t)fnt->height;
        
        clipArea.x = clipX;
        clipArea.y = clipY;
        clipArea.width = clipWidth;
        clipArea.height = clipHeight;
        
        if(GFX_RectIntersects(&glyphRect, &clipArea) == GFX_FALSE)
            return width;
            
        GFX_RectClip(&glyphRect, &clipArea, &clippedGlyph);
        
        data = ((uint8_t*)fnt->header.dataAddress);
        data += offset;
        
        glyphDataWidth = GFXU_GetGlyphRowDataSize(fnt->bpp, width);
        
        if(x >= clipX)
            clipX1 = 0;
        else
            clipX1 = clipX - x;
            
        if(x + (int32_t)width <= clipX + clipWidth)
            clipX2 = width;
        else
            clipX2 = width + ((clipX + clipWidth) - (x + (int32_t)width));

        for(row = clippedGlyph.y - glyphRect.y; row < (clippedGlyph.y - glyphRect.y) + (clippedGlyph.height); row++)
        {
            GFXU_DrawGlyphRow(fnt->bpp,
                              data + (row * glyphDataWidth),
                              width,
                              x,
                              y + row,
                              clipX1,
                              clipX2);
        }
    }
    
    return width;
}

int32_t getGlyphRowDataSize1bpp(int32_t width)
{
    int32_t offset;
    
    offset = width >> 3;
    offset += (width % 8) > 0 ? 1 : 0;
    
    return offset;
}
                            
int32_t getGlyphRowDataSize8bpp(int32_t width)
{
    return width;
} 

typedef int32_t (*getGlyphRowDataSize_FnPtr)(int32_t);
                                         
getGlyphRowDataSize_FnPtr getGlyphRowDataSize[] =
{
    &getGlyphRowDataSize1bpp,
    &getGlyphRowDataSize8bpp
};

int32_t GFXU_GetGlyphRowDataSize(GFXU_FontAssetBPP bpp,
                                 int32_t width)
{
    return getGlyphRowDataSize[bpp](width);
}

       

typedef void (*drawGlyphRow_FnPtr)(uint8_t*,
                                   int32_t,
                                   int32_t,
                                   int32_t,
                                   int32_t,
                                   int32_t);

void drawGlyphRow1bpp(uint8_t* data, 
                      int32_t width,
                      int32_t x,
                      int32_t y,
                      int32_t clipStart,
                      int32_t clipEnd)
{
    int32_t i;
    uint32_t offs;
    uint8_t val;
    
    for(i = clipStart; i < clipEnd; i++)
    {
        offs = (i % 8);
        val = data[i >> 3];
        val = data[i >> 3] & (0x80 >> offs);
        
        if(val > 0)
            GFX_DrawPixel(x + i, y);
    }
}
                            
void drawGlyphRow8bpp(uint8_t* data,
                      int32_t width,
                      int32_t x,
                      int32_t y,
                      int32_t clipStart,
                      int32_t clipEnd)
{
    int32_t i;
    GFX_BlendMode mode;
    
    // store old blending mode
    GFX_Get(GFXF_DRAW_BLEND_MODE, &mode);
            
    // enable alpha blending on the current blend mode
    GFX_Set(GFXF_DRAW_BLEND_MODE, mode | GFX_BLEND_GLOBAL);
    
    for(i = clipStart; i < clipEnd; i++)
    {
        if(data[i] > 0)
        {
            GFX_Set(GFXF_DRAW_ALPHA_VALUE, data[i]);
            GFX_DrawPixel(x + i, y);
        }
    }
    
    // restore old blending mode
    GFX_Set(GFXF_DRAW_BLEND_MODE, mode);
} 
                                         
drawGlyphRow_FnPtr drawGlyphRow[] =
{
    &drawGlyphRow1bpp,
    &drawGlyphRow8bpp
};

void GFXU_DrawGlyphRow(GFXU_FontAssetBPP bpp,
                       uint8_t* data,
                       int32_t width,
                       int32_t x,
                       int32_t y,
                       int32_t clipXStart,
                       int32_t clipXEnd)
{
    drawGlyphRow[bpp](data, width, x, y, clipXStart, clipXEnd);
}

int32_t GFXU_DrawUnknownGlyph(int32_t x,
                              int32_t y,
                              int32_t height,
                              int32_t clipX,
                              int32_t clipY,
                              int32_t clipWidth,
                              int32_t clipHeight)
{
    int32_t width = (height >> 1);
    GFX_Rect rect, clipRect, drawRect, clipDrawRect;
    
    if(x + width < clipX || x > clipX + clipWidth)
        return width;
    
    rect.x = x + 1;
    rect.y = y + 1;
    rect.width = width - 3;
    rect.height = height - 1;
    
    clipRect.x = clipX;
    clipRect.y = clipY;
    clipRect.width = clipWidth;
    clipRect.height = clipHeight;
     
    // left line
    drawRect = rect;
    drawRect.width = 1;
    
    if(GFX_RectIntersects(&drawRect, &clipRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &clipRect, &clipDrawRect);
        
        GFX_DrawLine(clipDrawRect.x,
                     clipDrawRect.y,
                     clipDrawRect.x,
                     clipDrawRect.y + clipDrawRect.height - 1);     
    }
    
    // right line
    drawRect = rect;
    drawRect.width = 1;
    drawRect.x += width - 4;
    
    if(GFX_RectIntersects(&drawRect, &clipRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &clipRect, &clipDrawRect);
        
        GFX_DrawLine(clipDrawRect.x,
                     clipDrawRect.y,
                     clipDrawRect.x,
                     clipDrawRect.y + clipDrawRect.height - 1);     
    }
    
    // top line
    drawRect = rect;
    drawRect.height = 1;
    drawRect.width = width - 4;
        
    if(GFX_RectIntersects(&drawRect, &clipRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &clipRect, &clipDrawRect);
        
        GFX_DrawLine(clipDrawRect.x,
                     clipDrawRect.y,
                     clipDrawRect.x + clipDrawRect.width - 1,
                     clipDrawRect.y);     
    }
    
    // bottom line
    drawRect = rect;
    drawRect.y += height - 2;
    drawRect.height = 1;
    drawRect.width = width - 4;
        
    if(GFX_RectIntersects(&drawRect, &clipRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &clipRect, &clipDrawRect);
        
        GFX_DrawLine(clipDrawRect.x,
                     clipDrawRect.y,
                     clipDrawRect.x + clipDrawRect.width - 1,
                     clipDrawRect.y);     
    }
    
    return width;
}
