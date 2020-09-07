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

/*******************************************************************************
 Module for Microchip Graphics Library - Graphics Utilities Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_string_utils.h

  Summary:
    Contains definitions for various internal string utility functions.

  Description:
    Internal use only
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef GFXU_STRING_UTILS_H
#define GFXU_STRING_UTILS_H
//DOM-IGNORE-END

#include "gfx/utils/inc/gfxu_string.h"
#include "gfx/utils/inc/gfxu_font.h"

// DOM-IGNORE-BEGIN
// internal use only
uint16_t GFXU_StringIndexLookup(GFXU_StringTableAsset* table,
                                uint32_t stringID,
                                uint32_t languageID);

// internal use only                                
GFX_Result GFXU_StringLookup(GFXU_StringTableAsset* table,
                             uint32_t stringIndex,
                             uint8_t** stringAddress,
                             uint32_t* stringSize);

// internal use only
LIB_EXPORT GFXU_FontAsset* GFXU_StringFontIndexLookup(GFXU_StringTableAsset* table,
                                                      uint32_t stringID,
                                                      uint32_t languageID);

// internal use only
GFX_Result GFXU_FontGetLookupTableEntry(uint8_t* table,
                                        uint32_t index,
                                        uint32_t max,
                                        uint32_t* offset,
                                        uint32_t* width);

// internal use only
GFX_Result GFXU_FontGetGlyphInfo(GFXU_FontAsset* fnt,
                                 uint32_t glyph,
                                 uint32_t* offset,
                                 uint32_t* width);

// internal use only
GFX_Result GFXU_DecodeUTF8(uint8_t* val,
                           uint32_t max,
                           uint32_t* codePoint,
                           uint32_t* size);

// internal use only                           
GFX_Result GFXU_DecodeUTF16(uint8_t* val,
                            uint32_t max,
                            uint32_t* codePoint,
                            uint32_t* size);

// internal use only                            
GFX_Result GFXU_DecodeCodePoint(uint32_t encoding,
                                uint8_t* data,
                                uint32_t max,
                                uint32_t* codePoint,
                                uint32_t* offset);                             

// internal use only                            
GFX_Result GFXU_DecodeAndDrawString(uint8_t* string,
                                    uint32_t length,
                                    GFXU_StringEncodingMode mode,
                                    GFXU_FontAsset* fnt,
                                    int32_t clipX,
                                    int32_t clipY,
                                    int32_t clipWidth,
                                    int32_t clipHeight,
                                    int32_t x,
                                    int32_t y);

// internal use only 
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
                                    int32_t y);

// internal use only
int32_t GFXU_DrawGlyph(GFXU_FontAsset* fnt,
                       uint32_t glyph,
                       int32_t clipX,
                       int32_t clipY,
                       int32_t clipWidth,
                       int32_t clipHeight,
                       int32_t x,
                       int32_t y);

// internal use only
int32_t GFXU_GetGlyphRowDataSize(GFXU_FontAssetBPP bpp,
                                 int32_t width);

// internal use only                                 
void GFXU_DrawGlyphRow(GFXU_FontAssetBPP bpp,
                       uint8_t* data,
                       int32_t width,
                       int32_t x,
                       int32_t y,
                       int32_t clipXStart,
                       int32_t clipXEnd);

// internal use only                             
int32_t GFXU_DrawUnknownGlyph(int32_t x,
                           int32_t y,
                           int32_t height,
                           int32_t clipX,
                           int32_t clipY,
                           int32_t clipWidth,
                           int32_t clipHeight);

// DOM-IGNORE-END

#endif /* GFXU_STRING_UTILS_H */