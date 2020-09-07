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

#include "gfx/utils/inc/gfxu_string_utils.h"

GFXU_CHAR GFXU_GetCharAt(GFXU_StringTableAsset* tbl,
                         uint32_t id,
                         uint32_t lang,
                         uint32_t idx)
{
    GFXU_FontAsset* fnt = GFX_NULL;
    uint16_t stringIdx;
    uint8_t* stringAddress;
    uint32_t stringSize;
    uint32_t i, j;
    uint32_t codePoint;
    uint32_t offset = 0;
    
    if(tbl == GFX_NULL)
        return 0;
    
    fnt = GFXU_StringFontIndexLookup(tbl, id, lang);
    
    if(fnt == GFX_NULL)
        return 0;
    
    stringIdx = GFXU_StringIndexLookup(tbl, id, lang);
    
    if(GFXU_StringLookup(tbl, stringIdx, &stringAddress, &stringSize))
        return GFX_FAILURE;
    
    j = 0;
    
    for(i = 0; i < stringSize;)
    {
        if(GFXU_DecodeCodePoint(tbl->encodingMode,
                                stringAddress + i,
                                stringSize,
                                &codePoint,
                                &offset) == GFX_FAILURE)
            return GFX_FAILURE;
        
        i += offset;
        
        if(j == idx)
            return codePoint;
        
        j++;
    }
    
    return 0;
}

uint32_t GFXU_GetStringLength(GFXU_StringTableAsset* tbl,
                              uint32_t id,
                              uint32_t lang)
{
    uint16_t idx;
    uint8_t* stringAddress;
    uint32_t stringSize;
    uint32_t stringLength = 0;
    uint32_t codePoint;
    uint32_t offset = 0;
    uint32_t i;
    
    idx = GFXU_StringIndexLookup(tbl, id, lang);
    
    if(GFXU_StringLookup(tbl, idx, &stringAddress, &stringSize))
        return GFX_FAILURE;
        
    for(i = 0; i < stringSize;)
    {
        if(GFXU_DecodeCodePoint(tbl->encodingMode,
                                stringAddress + i,
                                stringSize,
                                &codePoint,
                                &offset) == GFX_FAILURE)
            return GFX_FAILURE;
        
        i += offset;
        
        stringLength += 1;
    }
        
    return stringLength;
}

uint32_t GFXU_GetStringSizeInBytes(GFXU_StringTableAsset* tbl,
                              uint32_t id,
                              uint32_t lang)
{
    uint16_t idx;
    uint8_t* stringAddress;
    uint32_t stringSize;
    
    idx = GFXU_StringIndexLookup(tbl, id, lang);
    
    if(GFXU_StringLookup(tbl, idx, &stringAddress, &stringSize))
        return GFX_FAILURE;
        
    return stringSize;
}

GFX_Result GFXU_GetStringRect(GFXU_StringTableAsset* tbl,
                              uint32_t id,
                              uint32_t lang,
                              GFX_Rect* rect)
{
    if(tbl == GFX_NULL ||
       id >= tbl->stringCount || 
       lang >= tbl->languageCount ||
       rect == GFX_NULL)
        return GFX_FAILURE;
        
    rect->x = 0;
    rect->y = 0;
    rect->width = GFXU_CalculateStringWidth(tbl, id, lang);
    rect->height = GFXU_GetStringHeight(tbl, id, lang);
    
    return GFX_SUCCESS;
}

uint32_t GFXU_GetStringLineRect(GFXU_StringTableAsset* tbl,
                                uint32_t id,
                                uint32_t lang,
                                uint32_t start,
                                GFX_Rect* rect)
{
    uint16_t idx;
    uint8_t* stringAddress;
    uint32_t stringSize;
    uint32_t codePoint;
	uint32_t i;
	uint32_t width;
    uint32_t totalWidth = 0;
	uint32_t offs = 0;
	GFXU_FontAsset* fnt = GFX_NULL;
    
    idx = GFXU_StringIndexLookup(tbl, id, lang);
    
    if(GFXU_StringLookup(tbl, idx, &stringAddress, &stringSize))
    {
        return GFX_FAILURE;
    }

	fnt = GFXU_StringFontIndexLookup(tbl, id, lang);
	if(fnt == GFX_NULL)
    {
        return 0;
    }
    
    for(i = start; i < stringSize;)
    {
        if(GFXU_DecodeCodePoint(tbl->encodingMode,
                                stringAddress + i,
                                stringSize,
                                &codePoint,
                                &offs) == GFX_FAILURE)
            return GFX_FAILURE;

		i += offs;

		//Test for line feed
		if (codePoint == 0x5c) // '\' character (92d)
		{
			if (i < (stringSize - 1))
			{
				if (GFXU_DecodeCodePoint(tbl->encodingMode,
                                stringAddress + i,
                                stringSize,
                                &codePoint,
                                &offs) == GFX_FAILURE)
				return GFX_FAILURE;

				if (codePoint == 0x6E) // 'n' character (110d)
				{
					i += offs;
					break;
				}
			}
		}
		
		GFXU_FontGetGlyphInfo(fnt,
                              codePoint,
                              &offs,
                              &width);
        
		totalWidth += width;
    }

	rect->width = totalWidth;
	rect->height = fnt->height;

    return i;
}

uint32_t GFXU_GetCharStringLineRect(GFXU_CHAR* str,
                                    GFXU_FontAsset* font,
                                    uint32_t start,
									GFX_Rect* rect)
{
    uint32_t len;
    uint32_t codePoint;
    uint32_t width;
    uint32_t glyphoff;
    uint32_t i;
    
    len = 0;
    i = 0;

    rect->height = 0;
    rect->width = 0;

    if(str == NULL || font == NULL || rect == NULL)
        return start;

    while (str[i++] != '\0')
        len++;

    if (start > len)
        return start;
    
    for(i = start;; i++)
    {
        codePoint = str[i];

        //Test for line feed or end of string
        if ((codePoint == '\n') ||
            (codePoint == '\0'))
        {
            i++;
            break;
        }

        //Test for string "\n"        
        if ((codePoint == 0x5c) && 
             (str[i+1] == 0x6E))
        {
            i += 2;
            break;
        }
        
        GFXU_FontGetGlyphInfo(font,
                              codePoint,
                              &glyphoff,
                              &width);

        rect->width += width;
    }

    rect->height = font->height;
    
    return i;
}

static uint32_t calculateWidth(GFXU_StringTableAsset* tbl,
                               GFXU_FontAsset* fnt,
                               uint32_t id,
                               uint32_t lang)
{
    uint16_t idx;
    uint8_t* stringAddress;
    uint32_t stringSize;
    uint32_t i;
    uint32_t codePoint;
    uint32_t offset = 0;
    uint32_t width;
    uint32_t totalWidth = 0;
    
    if(tbl == GFX_NULL || fnt == GFX_NULL)
        return 0;
        
    idx = GFXU_StringIndexLookup(tbl, id, lang);
    
    if(GFXU_StringLookup(tbl, idx, &stringAddress, &stringSize))
        return GFX_FAILURE;
    
    for(i = 0; i < stringSize;)
    {
        if(GFXU_DecodeCodePoint(tbl->encodingMode,
                                stringAddress + i,
                                stringSize,
                                &codePoint,
                                &offset) == GFX_FAILURE)
            return GFX_FAILURE;
        
        i += offset;
        
        GFXU_FontGetGlyphInfo(fnt,
                              codePoint,
                              &offset,
                              &width);
            
        totalWidth += width;
    }
    
    return totalWidth;
}

static uint32_t calculatePartialWidth(GFXU_StringTableAsset* tbl,
                                      GFXU_FontAsset* fnt,
                                      uint32_t id,
                                      uint32_t lang,
                                      uint32_t length)
{
    uint16_t idx;
    uint8_t* stringAddress;
    uint32_t stringSize;
    uint32_t i;
    uint32_t codePoint;
    uint32_t offset = 0;
    uint32_t width;
    uint32_t totalWidth = 0;
    uint32_t maxLength = 0;
    
    if(tbl == GFX_NULL || fnt == GFX_NULL)
        return 0;
        
    maxLength = GFXU_GetStringLength(tbl, id, lang);
        
    if(length > maxLength)
        length = maxLength;
        
    idx = GFXU_StringIndexLookup(tbl, id, lang);
    
    if(GFXU_StringLookup(tbl, idx, &stringAddress, &stringSize))
        return GFX_FAILURE;
    
    idx = 0;
    
    for(i = 0; i < stringSize;)
    {
        if(GFXU_DecodeCodePoint(tbl->encodingMode,
                                stringAddress + i,
                                stringSize,
                                &codePoint,
                                &offset) == GFX_FAILURE)
            return GFX_FAILURE;
        
        i += offset;
        
        GFXU_FontGetGlyphInfo(fnt,
                              codePoint,
                              &offset,
                              &width);
            
        totalWidth += width;
        
        idx++;
        
        if(idx >= length)
            break;
    }
    
    return totalWidth;
}

uint32_t GFXU_GetStringHeight(GFXU_StringTableAsset* tbl,
                              uint32_t id,
                              uint32_t lang)
{
    GFXU_FontAsset* ast;
    
    if(tbl == NULL || id >= tbl->stringCount || lang >= tbl->languageCount)
        return 0;
        
    ast = GFXU_StringFontIndexLookup(tbl, id, lang);
    
    if(ast == GFX_NULL)
        return 0;
    
    return ast->height;
}

uint32_t GFXU_GetStringAscent(GFXU_StringTableAsset* tbl,
                              uint32_t id,
                              uint32_t lang)
{
    GFXU_FontAsset* ast;
    
    if(tbl == NULL || id >= tbl->stringCount || lang >= tbl->languageCount)
        return 0;
        
    ast = GFXU_StringFontIndexLookup(tbl, id, lang);
    
    if(ast == GFX_NULL)
        return 0;
    
    return ast->ascent;
}

uint32_t GFXU_GetCharWidth(GFXU_CHAR chr, GFXU_FontAsset* fnt)
{
    uint32_t offset;
    uint32_t width;
    
    if(fnt == NULL)
        return 0;
    
    GFXU_FontGetGlyphInfo(fnt, chr, &offset, &width);
    
    return width;
}

uint32_t GFXU_CalculateStringWidth(GFXU_StringTableAsset* tbl, 
                                   uint32_t id,
                                   uint32_t lang)
{
    GFXU_FontAsset* fnt = GFX_NULL;
    
    if(tbl == GFX_NULL)
        return 0;
    
    fnt = GFXU_StringFontIndexLookup(tbl, id, lang);
    
    if(fnt == GFX_NULL)
        return 0;
        
    return calculateWidth(tbl, fnt, id, lang);
}

uint32_t GFXU_CalculatePartialStringWidth(GFXU_StringTableAsset* tbl, 
                                          uint32_t id,
                                          uint32_t lang,
                                          uint32_t length)
{
    GFXU_FontAsset* fnt = GFX_NULL;
    
    if(tbl == GFX_NULL)
        return 0;
    
    fnt = GFXU_StringFontIndexLookup(tbl, id, lang);
    
    if(fnt == GFX_NULL)
        return 0;
        
    return calculatePartialWidth(tbl, fnt, id, lang, length);
}

uint32_t GFXU_CalculateCharStringWidth(GFXU_CHAR* str, GFXU_FontAsset* fnt)
{
    uint32_t len;
    uint32_t codePoint;
    uint32_t offset;
    uint32_t totalWidth;
    uint32_t width;
    uint32_t i;
    
    len = 0;
    i = 0;
    
    if(str == NULL || fnt == NULL)
        return 0;
    
    while(str[len] != '\0')
        len++;
        
    if(len == 0)
        return 0;
    
    totalWidth = 0;
    
    for(i = 0; i < len; i++)
    {
        codePoint = str[i];
        
        GFXU_FontGetGlyphInfo(fnt,
                              codePoint,
                              &offset,
                              &width);
        
        totalWidth += width;
    }
    
    return totalWidth;
}

uint32_t GFXU_CalculatePartialCharStringWidth(GFXU_CHAR* str,
                                              GFXU_FontAsset* fnt,
                                              uint32_t size)
{
    uint32_t len;
    uint32_t codePoint;
    uint32_t offset;
    uint32_t totalWidth;
    uint32_t width;
    uint32_t i;
    
    len = 0;
    i = 0;
    
    if(str == NULL || fnt == NULL)
        return 0;
    
    while(str[len] != '\0')
        len++;
        
    if(len == 0)
        return 0;
        
    if(size > len)
        size = len;
    
    totalWidth = 0;
    
    for(i = 0; i < size; i++)
    {
        codePoint = str[i];
        
        GFXU_FontGetGlyphInfo(fnt,
                              codePoint,
                              &offset,
                              &width);
        
        totalWidth += width;
    }
    
    return totalWidth;
}