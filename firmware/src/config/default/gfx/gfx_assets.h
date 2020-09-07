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
  MPLAB Harmony Graphics Asset Header File

  File Name:
    gfx_assets.h

  Summary:
    Header file containing a list of asset specifications for use with the
	MPLAB Harmony Graphics Stack.

  Description:
    Header file containing a list of asset specifications for use with the
	MPLAB Harmony Graphics Stack.

    Created with MPLAB Harmony Version 3.0
*******************************************************************************/


#ifndef GFX_ASSETS_H
#define GFX_ASSETS_H

#include "gfx/utils/gfx_utils.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END 

/*** Generated Asset Descriptors ***/
/*****************************************************************************
 * MPLAB Harmony Graphics Asset Location IDs
 *****************************************************************************/
#define GFXU_ASSET_LOCATION_ID_INTERNAL    0
 
/*****************************************************************************
 * MPLAB Harmony Graphics Image Assets
 *****************************************************************************/
/*********************************
 * GFX Image Asset
 * Name:   RobinsonMap
 * Size:   300x154 pixels
 * Mode:   RGB_565
 * Format: RAW
 ***********************************/
extern GFXU_ImageAsset RobinsonMap;
	
/*****************************************************************************
 * MPLAB Harmony Graphics Font Assets
 *****************************************************************************/
/*********************************
 * GFX Font Asset
 * Name:         Consolas
 * Height:       15
 * Style:        Plain
 * Glyph Count:  103
 * Range Count:  5
 * Glyph Ranges: 0x20-0x7E
			     0xB0-0xB1
			     0x3A9
			     0x3BC
			     0x2190-0x2193
 ***********************************/
extern GFXU_FontAsset Consolas;
	
/*********************************
 * GFX Font Asset
 * Name:         Arial
 * Height:       19
 * Style:        Plain
 * Glyph Count:  102
 * Range Count:  5
 * Glyph Ranges: 0x20-0x7E
			     0xB1
			     0x3A9
			     0x3BC
			     0x2190-0x2193
 ***********************************/
extern GFXU_FontAsset Arial;
	
/*********************************
 * GFX Font Asset
 * Name:         Arial20
 * Height:       24
 * Style:        Plain
 * Glyph Count:  1
 * Range Count:  1
 * Glyph Ranges: 0xD7
 ***********************************/
extern GFXU_FontAsset Arial20;
	
/*****************************************************************************
 * MPLAB Harmony Graphics String Table
 *****************************************************************************/
/*********************************
 * GFX String Table
 * Name:         stringTable
 * Encoding:     UTF8
 * Languages:    default
 * String Count: 17
 ***********************************/
// language IDs
#define language_default    0

// string IDs
#define string_IColonString    0
#define string_LineGraphTickLabels    1
#define string_SampleGPSLoc    2
#define string_SampleTime    3
#define string_SpecialCharsMono    4
#define string_SpecialCharsRegular    5
#define string_VfColonString    6
#define string_VfString    7
#define string_WorldMapCursorChar    8
#define string_mAString    9
#define string_n0String    10
#define string_n10String    11
#define string_n1_0String    12
#define string_n20String    13
#define string_n2_0String    14
#define string_n30String    15
#define string_n3_0String    16
 
extern GFXU_StringTableAsset stringTable;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* GFX_ASSETS_H */

