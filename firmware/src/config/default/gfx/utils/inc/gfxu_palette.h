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
    gfx_palette.h

  Summary:
    Defines palette assets

  Description:
    Get palette color
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef GFXU_PALETTE_H
#define GFXU_PALETTE_H
//DOM-IGNORE-END

#include "gfx/utils/inc/gfxu_global.h"

// *****************************************************************************
/* Structure:
    GFXU_PaletteAsset_t

  Summary:
    Describes a palette asset.  A palette is a lookup table for unique colors.
    Given in an index, a color can be retrieved.
    
    header - standard asset header
    colorCount - the number of colors contained in the palette
    colorMode - the color mode of the image
*/
typedef struct GFXU_PaletteAsset_t
{
    GFXU_AssetHeader header;
    uint32_t colorCount;
    GFX_ColorMode colorMode;
} GFXU_PaletteAsset;

// *****************************************************************************
/* Function:
    GFX_Result GFXU_PaletteGetColor(void);

  Summary:
    Gets a color from a palette asset given an index value.
     
  Parameters:
    GFXU_PaletteAsset* pal - pointer to the palette to read
    uint32_t idx - the index of the color to look up
    GFXU_MemoryIntf* read_cb - a pointer to a memory interface to use for
                               memory operations, not needed for internal
                               assets
    GFXU_ExternalAssetReader** reader - will return as a valid pointer if
                          the palette asset is located in an external source.
                          If this is the case then the caller is responsible
                          for servicing the external asset reader state
                          machine until completion.  The caller is then
                          responsible for freeing the reader's memory.
  Returns:
    GFX_Color - the color that was retrieved
*/
GFX_Color GFXU_PaletteGetColor(GFXU_PaletteAsset* pal,
                               uint32_t idx,
                               GFXU_MemoryIntf* read_cb,
                               GFXU_ExternalAssetReader** reader);


#endif /* GFXU_PALETTE_H */