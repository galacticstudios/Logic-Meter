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
    libaria_widget_skin_classic_common.c

  Summary:
    Common functions for the classic widget skin.

  Description:
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_WIDGET_SKIN_CLASSIC_COMMON_H
#define LIBARIA_WIDGET_SKIN_CLASSIC_COMMON_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"

// DOM-IGNORE-BEGIN
// internal use only

void laWidget_SkinClassic_FillRect(const GFX_Rect* rect,
                                   GFX_Color clr,
                                   const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawBackground(laWidget* wgt, GFX_Color clr);
void laWidget_SkinClassic_DrawRoundCornerBackground(laWidget* wgt, GFX_Color clr);

void laWidget_SkinClassic_DrawStandardBackground(laWidget* wgt);

void laWidget_SkinClassic_DrawLineBorderLeft(const GFX_Rect* rect,
                                             GFX_Color clr,
                                             const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawLineBorderTop(const GFX_Rect* rect,
                                            GFX_Color clr,
                                            const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawLineBorderRight(const GFX_Rect* rect,
                                              GFX_Color clr,
                                              const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawLineBorderBottom(const GFX_Rect* rect,
                                               GFX_Color clr,
                                               const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawLineBorder(const GFX_Rect* rect,
                                         GFX_Color clr,
                                         const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawBevelBorderLeft(const GFX_Rect* rect,
                                              GFX_Color outer,
                                              GFX_Color inner,
                                              const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawBevelBorderTop(const GFX_Rect* rect,
                                             GFX_Color outer,
                                             GFX_Color inner,
                                             const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawBevelBorderRight(const GFX_Rect* rect,
                                               GFX_Color outer,
                                               GFX_Color inner,
                                               const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawBevelBorderBottom(const GFX_Rect* rect,
                                                GFX_Color outer,
                                                GFX_Color inner,
                                                const GFX_Rect* dmgRect);

void laWidget_SkinClassic_Draw2x2BeveledBorder(const GFX_Rect* rect,
                                               GFX_Color leftUpOuter,
                                               GFX_Color leftUpInner,
                                               GFX_Color bottomRightOuter,
                                               GFX_Color bototmRightInner,
                                               const GFX_Rect* dmgRect);

void laWidget_SkinClassic_Draw1x2BeveledBorder(const GFX_Rect* rect,
                                              GFX_Color leftUp,
                                              GFX_Color bottomRightOuter,
                                              GFX_Color bototmRightInner,
                                              const GFX_Rect* dmgRect);

void laWidget_SkinClassic_DrawBlit(laWidget* wgt,
                                   GFX_PixelBuffer* buffer);

void laWidget_SkinClassic_DrawStandardLineBorder(laWidget* wgt);

void laWidget_SkinClassic_DrawStandardRoundCornerLineBorder(laWidget* wgt);

void laWidget_SkinClassic_DrawStandardRaisedBorder(laWidget* wgt);

void laWidget_SkinClassic_DrawStandardLoweredBorder(laWidget* wgt);

void laWidget_SkinClassic_DrawStandardHybridBorder(laWidget* wgt);

void laWidget_SkinClassic_InvalidateBorderAreas(laWidget*);

// DOM-IGNORE-END

#endif /* LIBARIA_WIDGET_SKIN_CLASSIC_COMMON_H */