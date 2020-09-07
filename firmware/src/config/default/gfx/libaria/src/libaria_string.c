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


#include "gfx/libaria/inc/libaria_string.h"

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/utils/gfx_utils.h"

static uint32_t la_strlen(const GFXU_CHAR* str)
{
    int length = 0;

    while(str[length] != '\0')
        length++;

    return length;
}

static GFXU_CHAR* la_strcpy(GFXU_CHAR* dst, const GFXU_CHAR* src)
{
    int i = 0;

    while(src[i] != '\0')
    {
        dst[i] = src[i];
        i++;
    }

    dst[i] = src[i]; // copy terminator

    return dst;
}

LIB_EXPORT int32_t la_strcmp(GFXU_CHAR* str1, const GFXU_CHAR* str2)
{
    int i = 0;
    
    if(str1 == NULL || str2 == NULL)
        return -1;
            
//    if(la_strlen(str1) != la_strlen(str2))
//        return -1;
        
    while(str1[i] != '\0' || str2[i] != '\0')
    {
        if(str1[i] < str2[i])
            return -1;
        else if(str1[i] > str2[i])
            return 1;
        else
            i++;
    }

    return 0;
}

static GFXU_CHAR* la_strcat(GFXU_CHAR* dst, GFXU_CHAR* src)
{
    int i = 0;
    int len = la_strlen(dst);

    while(src[i] != '\0')
    {
        dst[len + i] = src[i];
        i++;
    }
    
    dst[len + i] = '\0';

    return dst;
}

laString* laString_New(laString* src)
{
    laString* str;
    
    if(laContext_GetActive() == NULL)
        return NULL;
        
    str = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laString));
    
    if(str == NULL)
        return NULL;
    
    str->font = GFX_NULL;
        
    laString_Copy(str, src);
    
    return str;
}

void laString_Delete(laString** str)
{
    if(laContext_GetActive() == NULL || str == NULL || *str == NULL)
        return;
        
    if((*str)->data != NULL)
        laContext_GetActive()->memIntf.heap.free((*str)->data);

    laContext_GetActive()->memIntf.heap.free(*str);
    
    *str = NULL;
}

void laString_Initialize(laString* str)
{
    if(str == NULL)
        return;

    str->data = NULL;
    str->capacity = 0;
    str->length = 0;
    str->font = NULL;
    str->table_index = LA_STRING_NULLIDX;
}

laString laString_CreateFromBuffer(const GFXU_CHAR* chr, GFXU_FontAsset* fnt)
{
    laString str = {0};
    
    if(laContext_GetActive() == NULL || chr == NULL)
    {
        laString_Initialize(&str);
        return str;
    }
    
    laString_Set(&str, chr);
    str.font = fnt;
    str.table_index = LA_STRING_NULLIDX;
    
    return str;
}

laString laString_CreateFromCharBuffer(const char* chr, GFXU_FontAsset* fnt)
{
	laString str = { 0, 0, 0, 0, LA_STRING_NULLIDX };
    uint32_t len = 0;
    uint32_t i;
    
    if(laContext_GetActive() == NULL || chr == NULL)
    {
        laString_Initialize(&str);
        return str;
    }
    
    len = strlen(chr);
    if(len != 0)
    {
        laString_Allocate(&str, len);

        if(str.capacity == 0)
            return str;

        for(i = 0; i < len; i++)
        {
            str.data[i] = (GFXU_CHAR)((unsigned char)chr[i]);
        }

        str.data[i] = '\0';
    }
 
    str.font = fnt;
    str.table_index = LA_STRING_NULLIDX;
    
    return str;
}

laString laString_CreateFromCharBuffer16(const uint16_t* chr, uint16_t size, GFXU_FontAsset* fnt)
{
	laString str = { 0, 0, 0, 0, LA_STRING_NULLIDX };
	uint32_t i;

	if (laContext_GetActive() == NULL || chr == NULL)
	{
		laString_Initialize(&str);
		return str;
	}

	if (size != 0)
	{
		laString_Allocate(&str, size);

		if (str.capacity == 0)
			return str;

		for (i = 0; i < size; i++)
		{
			str.data[i] = (GFXU_CHAR)(chr[i]);
		}

		str.data[i] = '\0';
	}

	str.font = fnt;
	str.table_index = LA_STRING_NULLIDX;

	return str;
}

laString laString_CreateFromID(uint32_t id)
{
    laString str = {0};
    
    laString_Initialize(&str);
        
    str.table_index = id;
        
    return str;
}

laResult laString_Allocate(laString* str, uint32_t length)
{
    if(laContext_GetActive() == NULL || str == NULL || length == 0)
        return LA_FAILURE;

    if(str->data != NULL)
        laContext_GetActive()->memIntf.heap.free(str->data);

    laString_Initialize(str);

    if(length == 0)
        return LA_SUCCESS;
    
    str->data = laContext_GetActive()->memIntf.heap.calloc(1, (length + 1) * sizeof(GFXU_CHAR));
    
    if(str->data == NULL)
        return LA_FAILURE;
    
    str->capacity = length + 1;
    str->length = length;
    
    return LA_SUCCESS;
}

laResult laString_Set(laString* str, const GFXU_CHAR* buffer)
{
    uint32_t len;
    
    if(str == NULL)
        return LA_FAILURE;
    
    if(laContext_GetActive() == NULL || buffer == NULL)
        return LA_FAILURE;
    
    len = la_strlen(buffer);
    
    // need to adjust destination capacity?
    if(str->capacity < len + 1)
    {
        str->data = laContext_GetActive()->memIntf.heap.realloc(str->data, (len + 6) * sizeof(GFXU_CHAR));

        // failed to reallocate data?
        if(str->data == NULL)
        {
            laString_Initialize(str);
            return LA_FAILURE;
        }
        
        str->capacity = len + 6;
    }

    str->length = len;

    la_strcpy(str->data, buffer);
    
    return LA_SUCCESS;
}

void laString_Destroy(laString* str)
{
    if(laContext_GetActive() == NULL || str == NULL)
        return;

    if(str->data != NULL)
        laContext_GetActive()->memIntf.heap.free(str->data);

    str->data = NULL;
    str->capacity = 0;
    str->length = 0;
    str->font = NULL;
    str->table_index = LA_STRING_NULLIDX;
}

laResult laString_Copy(laString* dst, const laString* src)
{
    if(laContext_GetActive() == NULL)
        return LA_FAILURE;
        
    if(src == NULL || dst == NULL)
        return LA_FAILURE;
    
    dst->length = src->length;
    dst->capacity = 0;
    dst->font = src->font;
    dst->table_index = src->table_index;
    
    // if source has no data but destination does, free destination data
    if(src->length == 0 && dst->data != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(dst->data);
        dst->data = NULL;

        return LA_SUCCESS;
    }
    
    if(dst->length > 0)
    {
        dst->capacity = dst->length + 6;
        dst->data = laContext_GetActive()->memIntf.heap.realloc(dst->data, dst->capacity * sizeof(GFXU_CHAR));

        if(dst->data == NULL)
        {
            laString_Initialize(dst);
            
            return LA_FAILURE;
        }

        la_strcpy(dst->data, src->data);
    }
    
    return LA_SUCCESS;
}

void laString_ExtractFromTable(laString* dst, uint32_t table_index)
{
    if(laContext_GetActive() == NULL)
        return;
    
    if(table_index == LA_STRING_NULLIDX)
        return;

    if(dst->data != NULL)
        laString_Destroy(dst);

    // copy from string table
    dst->table_index = LA_STRING_NULLIDX;
    
    laString_Allocate(dst,
                      GFXU_GetStringLength(laContext_GetActive()->stringTable,
                                           table_index,
                                           laContext_GetActive()->languageID));
                      
    GFXU_ExtractString(laContext_GetActive()->stringTable,
                       table_index,
                       laContext_GetActive()->languageID,
                       dst->data,
                       dst->capacity,
                       0);
                       
    dst->font = GFXU_StringFontIndexLookup(laContext_GetActive()->stringTable,
                                           table_index,
                                           laContext_GetActive()->languageID);
}

GFXU_CHAR laString_CharAt(const laString* str, uint32_t idx)
{
    if(str->table_index == LA_STRING_NULLIDX)
    {
        if(idx > str->length)
            return 0;
            
        return str->data[idx];
    }
    else
    {
        return GFXU_GetCharAt(laContext_GetActive()->stringTable,
                              str->table_index,
                              laContext_GetActive()->languageID,
                              idx);
    }
}

uint32_t laString_Length(const laString* str)
{
    if(str == NULL)
        return 0;

    if(str->table_index != LA_STRING_NULLIDX)
        return GFXU_GetStringLength(laContext_GetActive()->stringTable,
                                    str->table_index,
                                    laContext_GetActive()->languageID);

    return str->length;
}

void laString_ReduceLength(laString* str, uint32_t length)
{
    if(str == NULL || length >= str->length || str->data == NULL)
        return;
        
    str->length = length;
    str->data[str->length] = '\0';
}

uint32_t laString_Capacity(const laString* str)
{
    if(str == NULL)
        return 0;

    return str->capacity;
}

laResult laString_SetCapacity(laString* str, uint32_t cap)
{
    if(laContext_GetActive() == NULL || str == NULL || str->capacity == cap)
        return LA_FAILURE;
        
    if(str->capacity == 0)
    {
        str->data = laContext_GetActive()->memIntf.heap.malloc(cap * sizeof(GFXU_CHAR));
        
        if(str->data == NULL)
            return LA_FAILURE;
            
        str->capacity = cap;
        str->length = 0;
    }
    else if(cap > str->capacity)
    {
        str->capacity = 0;
        str->data = laContext_GetActive()->memIntf.heap.realloc(str->data, cap * sizeof(GFXU_CHAR));
        
        if(str->data == NULL)
        {
            str->capacity = 0;
            str->length = 0;
            
            return LA_FAILURE;
        }
            
        str->capacity = cap;
    }
    else if(cap < str->capacity)
    {
        str->data = laContext_GetActive()->memIntf.heap.realloc(str->data, cap * sizeof(GFXU_CHAR));
        
        if(str->data == NULL)
        {
            str->capacity = 0;
            str->length = 0;
            
            return LA_FAILURE;
        }
        
        str->capacity = cap;
        str->length = cap - 1;
        str->data[str->length] = '\0';
    }
    
    return LA_SUCCESS;
}

int32_t laString_Compare(const laString* lstr, const laString* rstr)
{
    if(lstr == NULL || rstr == NULL)
        return LA_FAILURE;
        
    if(lstr == rstr)
        return 0;

    if(lstr->table_index == LA_STRING_NULLIDX &&
       rstr->table_index == LA_STRING_NULLIDX)
    {
        if (lstr->font != rstr->font)
            return -1;
            
        return la_strcmp(lstr->data, rstr->data);
    }
    else if(lstr->table_index == LA_STRING_NULLIDX &&
            rstr->table_index != LA_STRING_NULLIDX)
    {
        if (lstr->font != GFXU_StringFontIndexLookup(
                            laContext_GetActive()->stringTable,
                            rstr->table_index,
                            laContext_GetActive()->languageID))
            return -1;
        
        return GFXU_CompareString(laContext_GetActive()->stringTable,
                                  rstr->table_index,
                                  laContext_GetActive()->languageID,
                                  lstr->data) * -1;
    }
    else if(lstr->table_index != LA_STRING_NULLIDX &&
            rstr->table_index == LA_STRING_NULLIDX)
    {
        if (rstr->font != GFXU_StringFontIndexLookup(
                            laContext_GetActive()->stringTable,
                            lstr->table_index,
                            laContext_GetActive()->languageID))
            return -1;
        
        return GFXU_CompareString(laContext_GetActive()->stringTable,
                                  lstr->table_index,
                                  laContext_GetActive()->languageID,
                                  rstr->data);
    }
    else if(lstr->table_index != LA_STRING_NULLIDX &&
            lstr->table_index == rstr->table_index)
    {
        return 0;
    }
    
    return -1;
}

int32_t laString_CompareBuffer(const laString* str, const GFXU_CHAR* buffer)
{
    if(str == NULL || buffer == NULL)
        return -1;
        
    return la_strcmp(str->data, buffer);
}

laResult laString_Append(laString* dst, const laString* src)
{
    uint32_t oldLength;
    
    if(laContext_GetActive() == NULL)
        return LA_FAILURE;

    if(src->table_index == LA_STRING_NULLIDX && 
       src->data == NULL)
        return LA_SUCCESS;

    if(dst->data == NULL)
    {
        laString_ExtractFromTable(dst, src->table_index);
    }
    else
    {
        if(src->table_index != LA_STRING_NULLIDX)
        {
            oldLength = dst->length;
            
            dst->table_index = LA_STRING_NULLIDX;
            dst->length = dst->length + GFXU_GetStringLength(laContext_GetActive()->stringTable,
                                                             src->table_index,
                                                             laContext_GetActive()->languageID);
            dst->capacity = dst->length + 6;
        
            dst->data = laContext_GetActive()->memIntf.heap.realloc(dst->data, dst->capacity * sizeof(GFXU_CHAR));
                 
            if(dst->data == NULL)
                return LA_FAILURE;
                              
            GFXU_ExtractString(laContext_GetActive()->stringTable,
                               src->table_index,
                               laContext_GetActive()->languageID,
                               dst->data,
                               dst->capacity,
                               oldLength);
                               
            dst->data[dst->length] = 0;
        }
        else
        {
            dst->length = dst->length + src->length;
            dst->capacity = dst->length + 1;

            dst->data = laContext_GetActive()->memIntf.heap.realloc(dst->data, dst->capacity * sizeof(GFXU_CHAR));
            
            if(dst->data == NULL)
                return LA_FAILURE;
            
            dst->data = la_strcat(dst->data, src->data);
        }
    }
    
    return LA_SUCCESS;
}

laResult laString_Insert(laString* dst, const laString* src, uint32_t idx)
{
    uint32_t totalLength, rightLength;
    int32_t i;
    
    if(dst == NULL || dst->data == NULL)
    {
        if(src->table_index != LA_STRING_NULLIDX)
        {
            laString_ExtractFromTable(dst, src->table_index);
            
            return LA_SUCCESS;
        }
        else if(src->data != NULL)
        {
            return laString_Copy(dst, src);
        }
    }
       
    if(idx >= dst->length)
        return laString_Append(dst, src);
    
    if (idx == 0)
    {
        laString temp;
        laResult retval = LA_FAILURE;

        laString_Initialize(&temp);

        retval = laString_Copy(&temp, src);
        if (retval != LA_SUCCESS)
            return LA_FAILURE;

        retval = laString_Append(&temp, dst);
        if (retval != LA_SUCCESS)
        {
            laString_Destroy(&temp);
            return LA_FAILURE;
        }

        retval = laString_Copy(dst, &temp);
        laString_Destroy(&temp);

        return retval;
    }

    rightLength = laString_Length(src);
    totalLength = dst->length + laString_Length(src);
    
    // reallocate if necessary
    if(totalLength >= dst->capacity)
        laString_SetCapacity(dst, totalLength + 6); // null plus some extra padding
    
    if(dst->data == NULL)
        return LA_FAILURE;
    
    // copy data
    for(i = dst->length - 1; i >= idx; i--)
        dst->data[i + rightLength] = dst->data[i];
    
    for(i = 0; i < rightLength; i++)
        dst->data[i + idx] = laString_CharAt(src, i);
    
    dst->length = totalLength;
    
    dst->data[dst->length] = 0;
    
    return LA_SUCCESS;
}

uint32_t laString_Remove(laString* str, uint32_t idx, uint32_t count)
{
    uint32_t i;
    
    if(str == NULL || idx >= str->length)
        return 0;
        
    if(count > str->length - idx)
        count = str->length - idx;
        
    for(i = idx; i < str->length; i++)
        str->data[i] = str->data[i + count];
    
    str->length -= count;
    
    return count;
}

void laString_Clear(laString* str)
{
    if(str == NULL)
        return;
    
    str->table_index = LA_STRING_NULLIDX;
    
    if(str->capacity > 0)
    {
        str->length = 0;
        str->data[0] = '\0';
    }
}

uint32_t laString_ToCharBuffer(const laString* str,
                               GFXU_CHAR* buffer,
                               uint32_t size)
{
    uint32_t sz = 0;

    if(str == NULL || buffer == NULL || size == 0)
        return 0;

    if(str->length + 1 < size)
        size = str->length + 1;

    for(sz = 0; sz < size; sz++)
        buffer[sz] = str->data[sz];
        
    return sz;
}

void laString_GetRect(laString* str, GFX_Rect* rect)
{
    if(rect == NULL)
        return;
        
    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;
    
    if(str == NULL)
        return;
    
    if(str->table_index != LA_STRING_NULLIDX)
    {
        GFXU_GetStringRect(laContext_GetStringTable(),
                           str->table_index,
                           laContext_GetStringLanguage(),
                           rect);
    }
    else
    {
        if(str->font == NULL)
            return;
            
        rect->x = 0;
        rect->y = 0;
        rect->height = str->font->height;
        rect->width = GFXU_CalculateCharStringWidth(str->data, str->font);
    }
}

void laString_GetLineRect(laString* str, uint32_t start, GFX_Rect* rect, uint32_t * end)
{
    if(rect == NULL)
        return;

    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;

    if(str == NULL)
        return;

    if(str->table_index != LA_STRING_NULLIDX)
    {
		*end = GFXU_GetStringLineRect(laContext_GetActive()->stringTable, 
                                      str->table_index,
                                      laContext_GetActive()->languageID,
                                      start,
                                      rect);
    }
    else
    {
        if(str->font == NULL)
            return;

        *end = GFXU_GetCharStringLineRect(str->data,
                                          str->font,
                                          start,
                                          rect);
    }
}

void laString_GetMultiLineRect(laString* str, GFX_Rect* rect, int32_t lineSpace)
{
    uint32_t offset = 0, newoffset = 0;
    GFX_Rect lineRect;

    rect->height = 0;
    rect->width = 0;

    while (1)
	{
		offset = newoffset;

		laString_GetLineRect(str, offset, &lineRect, &newoffset);

        //At end of string
        if (offset == newoffset)
        {
            if (lineSpace >= 0)
                rect->height += lineSpace;
            else
                rect->height += laString_GetHeight(str) - laString_GetAscent(str);                
            break;
        }

        if (lineSpace >= 0)
            rect->height += lineSpace;
        else
            rect->height += laString_GetAscent(str);
        

		if (lineRect.width > rect->width)
		{
			rect->width = lineRect.width;
		}
	}
}

uint32_t laString_GetHeight(laString* str)
{
    if(str == NULL)
        return 0;
    
    if(str->table_index != LA_STRING_NULLIDX)
    {
        return GFXU_GetStringHeight(laContext_GetStringTable(),
                                    str->table_index,
                                    laContext_GetStringLanguage());
    }
    else
    {
        if(str->font == NULL)
            return 0;
            
        return str->font->height;
    }
    
    return 0;
}

uint32_t laString_GetAscent(laString* str)
{
    if(str == NULL)
        return 0;
    
    if(str->table_index != LA_STRING_NULLIDX)
    {
        return GFXU_GetStringAscent(laContext_GetStringTable(),
                                    str->table_index,
                                    laContext_GetStringLanguage());
    }
    else
    {
        if(str->font == NULL)
            return 0;
            
        return str->font->ascent;
    }
    
    return 0;
}

uint32_t laString_GetCharOffset(laString* str, uint32_t idx)
{
    if(str->table_index != LA_STRING_NULLIDX)
    {
        return GFXU_CalculatePartialStringWidth(laContext_GetStringTable(),
                                                str->table_index,
                                                laContext_GetStringLanguage(),
                                                idx);
    }
    else
    {
        if(str->font == NULL)
            return 0;
            
        return GFXU_CalculatePartialCharStringWidth(str->data, str->font, idx);
    }
}

uint32_t laString_GetCharIndexAtPoint(laString* str, int32_t x)
{
    int32_t last, curr, width, length;
    int32_t i;
    
    length = laString_Length(str);
    
    if(x < 0)
        return 0;
    
    last = 0;
        
    for(i = 0; i < length; i++)
    {
        width = laString_GetCharWidth(str, i);
        curr = last + width;
        
        if(x < curr)
        {
            if(x - last < curr - x)
                return i;
            else
                return i + 1;
        }
        
        last = curr;
    }
    
    return length;
}

uint32_t laString_GetCharWidth(laString* str, uint32_t idx)
{
    GFXU_CHAR chr;
    GFXU_FontAsset* ast;
    
    if(str->table_index == LA_STRING_NULLIDX)
    {
        return GFXU_GetCharWidth(str->data[idx], str->font);
    }
    else
    {
        chr = GFXU_GetCharAt(laContext_GetActive()->stringTable,
                             str->table_index,
                             laContext_GetActive()->languageID,
                             idx);
                             
        ast = GFXU_StringFontIndexLookup(laContext_GetActive()->stringTable,
                                         str->table_index,
                                         laContext_GetActive()->languageID);
                             
        return GFXU_GetCharWidth(chr, ast);
    }
}

void laString_Draw(laString* str,
                   int32_t x,
                   int32_t y,
                   GFXU_ExternalAssetReader** reader)
{
    if(str == NULL || laContext_GetActive() == NULL)
        return;
        
    if(str->table_index == LA_STRING_NULLIDX)
    {
        GFXU_DrawCharString(str->data,
                            str->font,
                            x,
                            y,
                            &laContext_GetActive()->memIntf,
                            reader);
    }
    else
    {
        GFXU_DrawString(laContext_GetStringTable(),
                        str->table_index,
                        laContext_GetStringLanguage(),
                        x,
                        y,
                        &laContext_GetActive()->memIntf,
                        reader);
    }
}

void laString_DrawClipped(laString* str,
                          int32_t clipX,
                          int32_t clipY,
                          int32_t clipWidth,
                          int32_t clipHeight,
                          int32_t x,
                          int32_t y,
                          GFXU_ExternalAssetReader** reader)
{
    if(str == NULL || laContext_GetActive() == NULL)
        return;
        
    if(str->table_index == LA_STRING_NULLIDX)
    {
        GFXU_DrawCharStringClipped(str->data,
                            str->font,
                            clipX,
                            clipY,
                            clipWidth,
                            clipHeight,
                            x,
                            y,
                            &laContext_GetActive()->memIntf,
                            reader);
    }
    else
    {
        GFXU_DrawStringClipped(laContext_GetStringTable(),
                        str->table_index,
                        laContext_GetStringLanguage(),
                        clipX,
                        clipY,
                        clipWidth,
                        clipHeight,
                        x,
                        y,
                        &laContext_GetActive()->memIntf,
                        reader);
    }
}

void laString_DrawSubStringClipped(laString* str,
                                   uint32_t start,
                                   uint32_t end,
                                   int32_t clipX,
                                   int32_t clipY,
                                   int32_t clipWidth,
                                   int32_t clipHeight,
                                   int32_t x,
                                   int32_t y,
                                   GFXU_ExternalAssetReader** reader)
{
    if(str == NULL || laContext_GetActive() == NULL)
        return;

    if(str->table_index == LA_STRING_NULLIDX)
    {
        GFXU_DrawCharSubStringClipped(str->data,
                                      str->font,
                                      start,
                                      end,
                                      clipX,
                                      clipY,
                                      clipWidth,
                                      clipHeight,
                                      x,
                                      y,
                                      &laContext_GetActive()->memIntf,
                                      reader);
    }
    else
    {
        GFXU_DrawSubStringClipped(laContext_GetStringTable(),
                                  str->table_index,
                                  laContext_GetStringLanguage(),
                                  start,
                                  end,
                                  clipX,
                                  clipY,
                                  clipWidth,
                                  clipHeight,
                                  x,
                                  y,
                                  &laContext_GetActive()->memIntf,
                                  reader);
    }
}

laBool laString_IsEmpty(const laString* str)
{
    if(str == NULL)
        return LA_TRUE;
        
    if(str->table_index != LA_STRING_NULLIDX)
        return LA_FALSE;
        
    if(str->length != 0)
        return LA_FALSE;
        
    return LA_TRUE;
}
