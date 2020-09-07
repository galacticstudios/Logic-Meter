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
 Module for Microchip Graphics Library - Aria User Interface Library

  Company:
    Microchip Technology Inc.

  File Name:
    libaria_scheme.h

  Summary:
    A scheme is a collection of colors that can be referenced by one or more
    widgets.  Widgets may use schemes in different ways.  While the color
    names strive to be intuitive they aren't always used in the manner in
    which they describe.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_SCHEME_H
#define LIBARIA_SCHEME_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/utils/gfx_utils.h"

#ifdef __cplusplus
    extern "C" {
#endif
      
// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

/*
  Enumeration:
    laScheme_t

  Summary:
    This structure specifies the style scheme components of an object.

  Description:
    A scheme is a collection of colors that can be referenced by widgets
    or other objects. While the color names strive to be intuitive they
    aren't always used in the manner in which they describe.

  Remarks:
   None.
 */
typedef struct laScheme_t
{
    GFX_Color base;
    GFX_Color highlight;
    GFX_Color highlightLight;
    GFX_Color shadow;
    GFX_Color shadowDark;
    GFX_Color foreground;
    GFX_Color foregroundInactive;
    GFX_Color foregroundDisabled;
    GFX_Color background;
    GFX_Color backgroundInactive;
    GFX_Color backgroundDisabled;
    GFX_Color text;
    GFX_Color textHighlight;
    GFX_Color textHighlightText;
    GFX_Color textInactive;
    GFX_Color textDisabled;
} laScheme;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

/* Function:
     void laScheme_Initialize(laScheme* scheme, GFX_ColorMode mode)
 
   Summary:
     Initialize the scheme to the default values as per the specified color mode. 

   Description:
     Initialize the scheme to the default values as per the specified color mode.

   Parameters:
     laSceme* scheme - the scheme to modify
     GFX_ColorMode - the color mode to use
    
   Returns:
     void    

*/
LIB_EXPORT void laScheme_Initialize(laScheme* scheme, GFX_ColorMode mode);

#ifdef __cplusplus
    }
#endif
    
#endif // LIBARIA_SCHEME_H 
