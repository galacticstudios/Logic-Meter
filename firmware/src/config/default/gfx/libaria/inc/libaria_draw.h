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
    libaria_draw.h

  Summary:
    Internal standard drawing help function definitions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef GOL_DRAW_H
#define GOL_DRAW_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/hal/inc/gfx_rect.h"

// *****************************************************************************
/* Function:
    void laDraw_LineBorder(GFX_Rect* rect, GFX_Color color)

   Summary:
    Internal utility function to draw a basic line border

   Parameters:
    GFX_Rect* rect - the rect to draw (screen space)
    GFX_Color color - the color to draw
*/
LIB_EXPORT void laDraw_LineBorder(GFX_Rect* rect, GFX_Color color);

// *****************************************************************************
/* Function:
    void laDraw_1x2BevelBorder(GFX_Rect* rect,
                               GFX_Color topColor,
                               GFX_Color bottomInnerColor,
                               GFX_Color bottomOuterColor)

   Summary:
    Internal utility function to draw a 1x2 bevel border

   Parameters:
    GFX_Rect* rect - the rect to draw (screen space)
    GFX_Color topColor - the color of the top left lines
    GFX_Color bottomInnerColor - the color of the bottom inner line
    GFX_Color bottomOuterColor - the color of the bottom outer line
*/
LIB_EXPORT void laDraw_1x2BevelBorder(GFX_Rect* rect,
                                      GFX_Color topColor,
                                      GFX_Color bottomInnerColor,
                                      GFX_Color bottomOuterColor);

// *****************************************************************************
/* Function:
    void laDraw_2x1BevelBorder(GFX_Rect* rect,
                               GFX_Color topOuterColor,
                               GFX_Color topInnerColor,
                               GFX_Color bottomOuterColor)

   Summary:
    Internal utility function to draw a 2x1 bevel border

   Parameters:
    GFX_Rect* rect - the rect to draw (screen space)
    GFX_Color topOuterColor - the color of the top outer line
    GFX_Color topInnerColor - the color of the top inner line
    GFX_Color bottomOuterColor - the color of the bottom lines
*/
LIB_EXPORT void laDraw_2x1BevelBorder(GFX_Rect* rect,
                                      GFX_Color topOuterColor,
                                      GFX_Color topInnerColor,
                                      GFX_Color bottomOuterColor);

// *****************************************************************************
/* Function:
    void laDraw_2x2BevelBorder(GFX_Rect* rect,
                               GFX_Color topOuterColor,
                               GFX_Color topInnerColor,
                               GFX_Color bottomInnerColor,
                               GFX_Color bottomOuterColor)

   Summary:
    Internal utility function to draw a 2x2 bevel border

   Parameters:
    GFX_Rect* rect - the rect to draw (screen space)
    GFX_Color topOuterColor - the color of the top outer line
    GFX_Color topInnerColor - the color of the top inner line
    GFX_Color bottomInnerColor - the color of the bottom inner line
    GFX_Color bottomOuterColor - the color of the bottom outer line
*/
LIB_EXPORT void laDraw_2x2BevelBorder(GFX_Rect* rect,
                                      GFX_Color topOuterColor,
                                      GFX_Color topInnerColor,
                                      GFX_Color bottomInnerColor,
                                      GFX_Color bottomOuterColor);

#endif /* GOL_DRAW_H */