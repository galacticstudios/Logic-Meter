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
    gfx_string.h

  Summary:
    Defines string table struct, string / character functions

  Description:
    String statistics and drawing utilities
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef GFX_STRING_H
#define GFX_STRING_H
//DOM-IGNORE-END

#include "gfx/utils/inc/gfxu_global.h"

// strings are defined as having 32bit characters due to the need
// to support international code points and unicode
// strings encoded using these types will not be compatible with
// standard library string functions like strlen or strcat
typedef uint32_t GFXU_CHAR;

// defines meta data sizes for the string table, don't change!
#define GFXU_STRING_ARRAY_SIZE      4
#define GFXU_STRING_ENTRY_SIZE      2
#define GFXU_STRING_MAX_CHAR_WIDTH  6

typedef struct GFXU_FontAsset_t GFXU_FontAsset;

// *****************************************************************************
/* Enumeration:
    GFXU_StringEncodingMode_t

  Summary:
    Indicates the string encoding mode type.  Any characters above 255 must use
    UTF8 or UTF16
*/
typedef enum GFXU_StringEncodingMode_t
{
    GFXU_STRING_ENCODING_ASCII,
    GFXU_STRING_ENCODING_UTF8,
    GFXU_STRING_ENCODING_UTF16
} GFXU_StringEncodingMode;

// *****************************************************************************
/* Structure:
    GFXU_StringTableAsset_t

  Summary:
    Describes a string table asset.  There is typically only ever one of these
    defined at any one time.
    
    header - standard asset header
    languageCount - the number of languages in the string table
    stringCount - the number of strings in the string table
    stringIndexTable - the pointer to the string index table.  the string index
                       table is a table that contains all of the unique strings
                       defined in the string table.
    fontTable - the font table contains an array of pointers to all defined
                font assets that the string table references
    fontIndexTable - the font index table is a table that maps strings to font
                     indices which can then be used to get an actual font pointer
                     from the font table
    encodingMode - indicates how strings are encoded in the stringIndexTable
*/
typedef struct GFXU_StringTableAsset_t
{
    GFXU_AssetHeader header;
    uint32_t languageCount;
    uint32_t stringCount;
    uint8_t* stringIndexTable;
    GFXU_FontAsset** fontTable;
    uint8_t* fontIndexTable;
    GFXU_StringEncodingMode encodingMode;
} GFXU_StringTableAsset;

// *****************************************************************************
/* Function:
    uint32_t GFXU_GetStringLength(GFXU_StringTableAsset* tbl,
                                  uint32_t id,
                                  uint32_t lang)

  Summary:
    Gets the length of a string contained in a string table.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    
  Returns:
    uint32_t - the length of the string entry
*/
LIB_EXPORT uint32_t GFXU_GetStringLength(GFXU_StringTableAsset* tbl,
                                         uint32_t id,
                                         uint32_t lang);

// *****************************************************************************
/* Function:
    uint32_t GFXU_GetStringSizeInBytes(GFXU_StringTableAsset* tbl,
                                       uint32_t id,
                                       uint32_t lang)

  Summary:
    Gets the size of a string contained in a string table, in bytes.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    
  Returns:
    uint32_t - the size of the string entry in bytes
*/
LIB_EXPORT uint32_t GFXU_GetStringSizeInBytes(GFXU_StringTableAsset* tbl,
                                              uint32_t id,
                                              uint32_t lang);
// *****************************************************************************
/* Function:
    GFX_Result GFXU_GetStringRect(GFXU_StringTableAsset* tbl,
                                  uint32_t id,
                                  uint32_t lang,
                                  GFX_Rect* rect)

  Summary:
    Gets the bounding rectangle for a string contained in the string table.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    GFX_Rect* rect - the resultant rectangle of the string
    
  Returns:
    GFX_Result
*/
LIB_EXPORT GFX_Result GFXU_GetStringRect(GFXU_StringTableAsset* tbl,
                                         uint32_t id,
                                         uint32_t lang,
                                         GFX_Rect* rect);


// *****************************************************************************
/* Function:
    uint32_t GFXU_GetStringLineRect(GFXU_StringTableAsset* tbl,
                                    uint32_t id,
                                    uint32_t lang,
                                    uint32_t offset,
                                    GFX_Rect* rect)

  Summary:
    Gets the bounding rectangle for a line in a string asset.

  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    uint32_t offset - the start offset of the first character in the line
    GFX_Rect* rect - the resultant rectangle of the string

  Returns:
    The offset of end of line (including line feed or end of string)
*/
LIB_EXPORT uint32_t GFXU_GetStringLineRect(GFXU_StringTableAsset* tbl,
                                           uint32_t id,
                                           uint32_t lang,
                                           uint32_t offset,
                                           GFX_Rect* rect);

/* Function:
    uint32_t GFXU_GetCharStringLineRect(GFXU_CHAR* str,
                                        GFXU_FontAsset* font,
                                        uint32_t offset,
                                        GFX_Rect* rect)

  Summary:
    Gets the bounding rectangle for a line in a character string.

  Parameters:
    GFXU_CHAR* str - the string buffer, terimated with a zero value
    GFXU_FontAsset* fnt - the font asset to reference
    uint32_t offset - the start offset of the first character in the line
    uint32_t length - the number of characters to include

  Returns:
    The offset of end of line (including line feed or end of string)
*/   
LIB_EXPORT uint32_t GFXU_GetCharStringLineRect(GFXU_CHAR* str,
                                               GFXU_FontAsset* font,
                                               uint32_t offset,
                                               GFX_Rect* rect);
                                         
// *****************************************************************************
/* Function:
    uint32_t GFXU_GetStringHeight(GFXU_StringTableAsset* tbl,
                                  uint32_t id,
                                  uint32_t lang)

  Summary:
    Gets the height of a string contained in the string table.

  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up

  Returns:
    uint32_t - the height of the string
*/
LIB_EXPORT uint32_t GFXU_GetStringHeight(GFXU_StringTableAsset* tbl,
                                         uint32_t id,
                                         uint32_t lang);

// *****************************************************************************
/* Function:
    uint32_t GFXU_GetStringAscent(GFXU_StringTableAsset* tbl,
                                  uint32_t id,
                                  uint32_t lang)

  Summary:
    Gets the ascent of a string contained in the string table.

  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up

  Returns:
    uint32_t - the ascent of the string
*/
LIB_EXPORT uint32_t GFXU_GetStringAscent(GFXU_StringTableAsset* tbl,
                                         uint32_t id,
                                         uint32_t lang);

// *****************************************************************************
/* Function:
    uint32_t GFXU_GetCharWidth(GFXU_CHAR chr, GFXU_FontAsset* fnt)

  Summary:
    Gets the width of a character for a given font.
     
  Parameters:
    GFXU_CHAR chr - the code point of the character
    GFXU_FontAsset* fnt - the font to reference
    
  Returns:
    uint32_t - the width of the character or zero if the font doesn't contain
               that character
*/                                 
LIB_EXPORT uint32_t GFXU_GetCharWidth(GFXU_CHAR chr, GFXU_FontAsset* fnt);

// *****************************************************************************
/* Function:
    GFXU_CHAR GFXU_GetCharAt(GFXU_StringTableAsset* tbl,
                             uint32_t id,
                             uint32_t lang,
                             uint32_t idx)

  Summary:
    Gets a character of a string contained in the string table.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    uint32_t idx - the index of the characer
    
  Returns:
    GFXU_CHAR - the code point of the character
*/  
LIB_EXPORT GFXU_CHAR GFXU_GetCharAt(GFXU_StringTableAsset* tbl,
                                    uint32_t id,
                                    uint32_t lang,
                                    uint32_t idx);

// *****************************************************************************
/* Function:
    uint32_t GFXU_CalculateStringWidth(GFXU_StringTableAsset* tbl,
                                       uint32_t id,
                                       uint32_t lang)

  Summary:
    Gets the width of a string contained in the string table.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    
  Returns:
    uint32_t - the width of the string
*/
LIB_EXPORT uint32_t GFXU_CalculateStringWidth(GFXU_StringTableAsset* tbl,
                                              uint32_t id,
                                              uint32_t lang);

// *****************************************************************************
/* Function:
    uint32_t GFXU_CalculatePartialStringWidth(GFXU_StringTableAsset* tbl,
                                              uint32_t id,
                                              uint32_t lang,
                                              uint32_t length)

  Summary:
    Gets the partial width, starting from the left, of a string contained in
    the string table.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    uint32_t length - the number of characters to include in the sub-string
    
  Returns:
    uint32_t - the width of the sub-string
*/                                            
LIB_EXPORT uint32_t GFXU_CalculatePartialStringWidth(GFXU_StringTableAsset* tbl,
                                                     uint32_t id,
                                                     uint32_t lang,
                                                     uint32_t length);

// *****************************************************************************
/* Function:
    uint32_t GFXU_CalculateCharStringWidth(GFXU_CHAR* str,
                                           GFXU_FontAsset* fnt)

  Summary:
    Gets the width of a string buffer in pixels.
     
  Parameters:
    GFXU_CHAR* str - the string buffer, terimated with a zero value
    GFXU_FontAsset* fnt - the font asset to reference
    
  Returns:
    uint32_t - the width of the string buffer
*/
LIB_EXPORT uint32_t GFXU_CalculateCharStringWidth(GFXU_CHAR* str,
                                                  GFXU_FontAsset* fnt);

// *****************************************************************************
/* Function:
    uint32_t GFXU_CalculatePartialCharStringWidth(GFXU_CHAR* str,
                                                  GFXU_FontAsset* fnt,
                                                  uint32_t length)

  Summary:
    Gets the width of a partial string buffer in pixels.
     
  Parameters:
    GFXU_CHAR* str - the string buffer, terimated with a zero value
    GFXU_FontAsset* fnt - the font asset to reference
    uint32_t length - the number of characters to include
    
  Returns:
    uint32_t - the width of the partial string buffer
*/                                              
LIB_EXPORT uint32_t GFXU_CalculatePartialCharStringWidth(GFXU_CHAR* str,
                                                         GFXU_FontAsset* fnt,
                                                         uint32_t length);

// *****************************************************************************
/* Function:
    uint32_t GFXU_ExtractString(GFXU_StringTableAsset* tbl,
                                uint32_t id,
                                uint32_t lang,
                                GFXU_CHAR* buffer,
                                uint32_t size,
                                uint32_t offset)

  Summary:
    Extracts a string from the string table into a local character buffer.  The
    local buffer must have already been allocated.  Will attempt to write
    'size - 1' number of characters into the buffer with the last being a zero
    terminator.  
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to reference
    uint32_t lang - the id of the language to reference
    GFXU_CHAR* buffer - the pointer to the buffer to write to
    uint32_t size - the maximum size of the buffer to write to
    uint32_t offset - the buffer write offset if any
    
  Returns:
    uint32_t - the number of characters written
*/                                              
LIB_EXPORT uint32_t GFXU_ExtractString(GFXU_StringTableAsset* tbl,
                                       uint32_t id,
                                       uint32_t lang,
                                       GFXU_CHAR* buffer,
                                       uint32_t size,
                                       uint32_t offset);

// *****************************************************************************
/* Function:
    int32_t GFXU_CompareString(GFXU_StringTableAsset* tbl,
                               uint32_t id,
                               uint32_t lang,
                               GFXU_CHAR* buffer)

  Summary:
    Compares a string table entry and a GFXU_CHAR type string.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    GFXU_CHAR* buffer - char buffer to compare
    
  Returns:
    int32_t - the compare result, should be identical to strcmp()
*/                                 
LIB_EXPORT int32_t GFXU_CompareString(GFXU_StringTableAsset* tbl,
                                      uint32_t id,
                                      uint32_t lang,
                                      GFXU_CHAR* buffer);

// *****************************************************************************
/* Function:
    GFX_Result GFXU_DrawString(GFXU_StringTableAsset* tbl,
                               uint32_t id,
                               uint32_t lang,
                               int32_t x,
                               int32_t y,
                               GFXU_MemoryIntf* memoryInterface,
                               GFXU_ExternalAssetReader** reader)

  Summary:
    Draws a string at the given coordinates.  Strings are drawn from the top down.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    int32_t x - the x position to which to draw the string
    int32_t y - the y position to which to draw the string
    GFXU_MemoryIntf* memoryInterface - a pointer to a memory interface to use for
                                       memory operations, not needed for internal
                                       assets
    GFXU_ExternalAssetReader** reader - will return as a valid pointer if
                          the image asset is located in an external source.
                          If this is the case then the caller is responsible
                          for servicing the external asset reader state
                          machine until completion.  The caller is then
                          responsible for freeing the reader's memory.
  Returns:
    GFX_Result
*/
LIB_EXPORT GFX_Result GFXU_DrawString(GFXU_StringTableAsset* tbl,
                                      uint32_t id,
                                      uint32_t lang,
                                      int32_t x,
                                      int32_t y,
                                      GFXU_MemoryIntf* memoryInterface,
                                      GFXU_ExternalAssetReader** reader);
                                      
// *****************************************************************************
/* Function:
    GFX_Result GFXU_DrawStringClipped(GFXU_StringTableAsset* tbl,
                               uint32_t id,
                               uint32_t lang,
                               int32_t clipX,
                               int32_t clipY,
                               int32_t clipWidth,
                               int32_t clipHeight,
                               int32_t x,
                               int32_t y,
                               GFXU_MemoryIntf* memoryInterface,
                               GFXU_ExternalAssetReader** reader)

  Summary:
    Draws a clipped string at the given coordinates.  Strings are drawn from
    the top down.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    int32_t clipX - clipped x position
    int32_t clipY - clipped y position
    int32_t clipWidth - clipped rectangle width
    int32_t clipHeight - clipped rectangle height
    int32_t x - the x position to which to draw the string
    int32_t y - the y position to which to draw the string
    GFXU_MemoryIntf* memoryInterface - a pointer to a memory interface to use for
                                       memory operations, not needed for internal
                                       assets
    GFXU_ExternalAssetReader** reader - will return as a valid pointer if
                          the image asset is located in an external source.
                          If this is the case then the caller is responsible
                          for servicing the external asset reader state
                          machine until completion.  The caller is then
                          responsible for freeing the reader's memory.
  Returns:
    GFX_Result
*/
LIB_EXPORT GFX_Result GFXU_DrawStringClipped(GFXU_StringTableAsset* tbl,
                                      uint32_t id,
                                      uint32_t lang,
                                      int32_t clipX,
                                      int32_t clipY,
                                      int32_t clipWidth,
                                      int32_t clipHeight,
                                      int32_t x,
                                      int32_t y,
                                      GFXU_MemoryIntf* memoryInterface,
                                      GFXU_ExternalAssetReader** reader);      

// *****************************************************************************
/* Function:
    GFX_Result GFXU_DrawSubStringClipped(GFXU_StringTableAsset* tbl,
                               uint32_t id,
                               uint32_t lang,
                               uint32_t start,
                               uint32_t end,
                               int32_t clipX,
                               int32_t clipY,
                               int32_t clipWidth,
                               int32_t clipHeight,
                               int32_t x,
                               int32_t y,
                               GFXU_MemoryIntf* memoryInterface,
                               GFXU_ExternalAssetReader** reader)

  Summary:
    Draws a sub-string from a string asset in a clipped rectangle at the given coordinates.  
    Strings are drawn from the top down.
     
  Parameters:
    GFXU_StringTableAsset* tbl - the string table to reference
    uint32_t id - the id of the string to look up
    uint32_t lang - the language entry to look up
    uint32_t start - offset of first character of substring
    uint32_t end - offset of last character of substring
    int32_t clipX - clipped x position
    int32_t clipY - clipped y position
    int32_t clipWidth - clipped rectangle width
    int32_t clipHeight - clipped rectangle height
    int32_t x - the x position to which to draw the string
    int32_t y - the y position to which to draw the string
    GFXU_MemoryIntf* memoryInterface - a pointer to a memory interface to use for
                                       memory operations, not needed for internal
                                       assets
    GFXU_ExternalAssetReader** reader - will return as a valid pointer if
                          the image asset is located in an external source.
                          If this is the case then the caller is responsible
                          for servicing the external asset reader state
                          machine until completion.  The caller is then
                          responsible for freeing the reader's memory.
  Returns:
    GFX_Result
*/
LIB_EXPORT GFX_Result GFXU_DrawSubStringClipped(GFXU_StringTableAsset* tbl,
                                      uint32_t id,
                                      uint32_t lang,
                                      uint32_t start,
                                      uint32_t end,
                                      int32_t clipX,
                                      int32_t clipY,
                                      int32_t clipWidth,
                                      int32_t clipHeight,
                                      int32_t x,
                                      int32_t y,
                                      GFXU_MemoryIntf* memoryInterface,
                                      GFXU_ExternalAssetReader** reader);   

// *****************************************************************************
/* Function:
    GFX_Result GFXU_DrawCharString(GFXU_CHAR* str,
                               GFXU_FontAsset* fnt
                               int32_t x,
                               int32_t y,
                               GFXU_MemoryIntf* memoryInterface,
                               GFXU_ExternalAssetReader** reader)

  Summary:
    Draws a clipped string at the given coordinates.  Strings are drawn from
    the top down.
     
  Parameters:
    GFXU_CHAR* str - the string buffer to draw
    int32_t x - the x position to which to draw the string
    int32_t y - the y position to which to draw the string
    GFXU_MemoryIntf* memoryInterface - a pointer to a memory interface to use for
                                       memory operations, not needed for internal
                                       assets
    GFXU_ExternalAssetReader** reader - will return as a valid pointer if
                          the image asset is located in an external source.
                          If this is the case then the caller is responsible
                          for servicing the external asset reader state
                          machine until completion.  The caller is then
                          responsible for freeing the reader's memory.
  Returns:
    GFX_Result
*/
LIB_EXPORT GFX_Result GFXU_DrawCharString(GFXU_CHAR* str,
                                          GFXU_FontAsset* fnt,
                                          int32_t x,
                                          int32_t y,
                                          GFXU_MemoryIntf* memoryInterface,
                                          GFXU_ExternalAssetReader** reader);

// *****************************************************************************
/* Function:
    GFX_Result GFXU_DrawCharStringClipped(GFXU_CHAR* str,
                               GFXU_FontAsset* fnt
                               int32_t clipX,
                               int32_t clipY,
                               int32_t clipWidth,
                               int32_t clipHeight,
                               int32_t x,
                               int32_t y,
                               GFXU_MemoryIntf* memoryInterface,
                               GFXU_ExternalAssetReader** reader)

  Summary:
    Draws a clipped string at the given coordinates.  Strings are drawn from
    the top down.
     
  Parameters:
    GFXU_CHAR* str - the string buffer to draw
    int32_t clipX - clipped x position
    int32_t clipY - clipped y position
    int32_t clipWidth - clipped rectangle width
    int32_t clipHeight - clipped rectangle height
    int32_t x - the x position to which to draw the string
    int32_t y - the y position to which to draw the string
    GFXU_MemoryIntf* memoryInterface - a pointer to a memory interface to use for
                                       memory operations, not needed for internal
                                       assets
    GFXU_ExternalAssetReader** reader - will return as a valid pointer if
                          the image asset is located in an external source.
                          If this is the case then the caller is responsible
                          for servicing the external asset reader state
                          machine until completion.  The caller is then
                          responsible for freeing the reader's memory.
  Returns:
    GFX_Result
*/
LIB_EXPORT GFX_Result GFXU_DrawCharStringClipped(GFXU_CHAR* str,
                                          GFXU_FontAsset* fnt,
                                          int32_t clipX,
                                          int32_t clipY,
                                          int32_t clipWidth,
                                          int32_t clipHeight,
                                          int32_t x,
                                          int32_t y,
                                          GFXU_MemoryIntf* memoryInterface,
                                          GFXU_ExternalAssetReader** reader);

// *****************************************************************************
/* Function:
    GFX_Result GFXU_DrawCharSubStringClipped(GFXU_CHAR* str,
                               GFXU_FontAsset* fnt
                               uint32_t start,
                               uint32_t end,
                               int32_t clipX,
                               int32_t clipY,
                               int32_t clipWidth,
                               int32_t clipHeight,
                               int32_t x,
                               int32_t y,
                               GFXU_MemoryIntf* memoryInterface,
                               GFXU_ExternalAssetReader** reader)

  Summary:
    Draws a clipped string at the given coordinates.  Strings are drawn from
    the top down.
     
  Parameters:
    GFXU_CHAR* str - the string buffer to draw
    GFXU_FontAsset* fnt - font asset to use
    uint32_t start - start offset of substring
    uint32_t end - end offset of substring
    int32_t clipX - clipped x position
    int32_t clipY - clipped y position
    int32_t clipWidth - clipped rectangle width
    int32_t clipHeight - clipped rectangle height
    int32_t x - the x position to which to draw the string
    int32_t y - the y position to which to draw the string
    GFXU_MemoryIntf* memoryInterface - a pointer to a memory interface to use for
                                       memory operations, not needed for internal
                                       assets
    GFXU_ExternalAssetReader** reader - will return as a valid pointer if
                          the image asset is located in an external source.
                          If this is the case then the caller is responsible
                          for servicing the external asset reader state
                          machine until completion.  The caller is then
                          responsible for freeing the reader's memory.
  Returns:
    GFX_Result
*/
LIB_EXPORT GFX_Result GFXU_DrawCharSubStringClipped(GFXU_CHAR* str,
                                          GFXU_FontAsset* fnt,
                                          uint32_t start,
                                          uint32_t end,
                                          int32_t clipX,
                                          int32_t clipY,
                                          int32_t clipWidth,
                                          int32_t clipHeight,
                                          int32_t x,
                                          int32_t y,
                                          GFXU_MemoryIntf* memoryInterface,
                                          GFXU_ExternalAssetReader** reader);

#endif /* GFX_STRING_H */