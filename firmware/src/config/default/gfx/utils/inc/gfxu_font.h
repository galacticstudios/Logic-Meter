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
    gfx_font.h

  Summary:
    Describes font assets

  Description:
    Type definitions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_FONT_H
#define GFX_FONT_H
//DOM-IGNORE-END

#include "gfx/utils/inc/gfxu_global.h"

// *****************************************************************************
/* Enumeration:
    GFXU_FontAssetBPP

  Summary:
    Indicates the bits per pixel mode of a font
*/
typedef enum GFXU_FontAssetBPP
{
    GFXU_FONT_BPP_1, // standard
    GFXU_FONT_BPP_8  // antialiased
} GFXU_FontAssetBPP;

// *****************************************************************************
/* Structure:
    GFXU_FontGlyphRange_t

  Summary:
    Describes a range of glyphs for a font.  All IDs are in raw code points and
    are not encoded in any way
    
    glyphCount - number of glyphs in the range
    startID - the starting glyph id
    endID - the ending glyph id
    lookupTable - the corresponding look up table to find the glyph raster data
*/
typedef struct GFXU_FontGlyphRange_t
{
    uint32_t glyphCount;
    uint32_t startID;
    uint32_t endID;
    uint8_t* lookupTable;
} GFXU_FontGlyphRange;

// *****************************************************************************
/* Structure:
    GFXU_FontGlyphIndexTable_t

  Summary:
    Describes a font glyph index table.  Essentially a series of glyph look up
    tables where each non-consecutive range of glyphs occupies one range entry
    
    count - number of ranges in the index table
    ranges - the glyph range array
*/
typedef struct GFXU_FontGlyphIndexTable_t
{
    uint32_t count;
    GFXU_FontGlyphRange ranges[];    
} GFXU_FontGlyphIndexTable;

// *****************************************************************************
/* Structure:
    GFXU_FontAsset_t

  Summary:
    Describes a font asset.  A font asset is a series of raster images that
    represet linguistic characters.  These characters are referenced by an index
    called a 'code point'.  This code point is 1-4 bytes in length.  Code points
    may be encoded to save space.  Fonts also contain kerning data that describes
    character positioning data.
    
    header - standard asset header
    height - font height in pixels
    ascent - font ascent in pixels
    descent - font descent in pixels
    baseline - font baseline in pixels
    bpp - font pixel size, either 1 or 8.  8 is for anti-aliased font data
    indexTable - pointer to the corresponding glyph index table.  this table
                 is used to reference code points to glyph data.
*/
typedef struct GFXU_FontAsset_t
{
    GFXU_AssetHeader header;
    uint32_t height;
    uint32_t ascent;
    uint32_t descent;
    uint32_t baseline;
    GFXU_FontAssetBPP bpp; 
    GFXU_FontGlyphIndexTable* indexTable;
} GFXU_FontAsset;

#endif /* GFX_FONT_H */