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


#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_context.h"

#include "gfx/hal/gfx.h"

void laDraw_LineBorder(GFX_Rect* rect, GFX_Color color)
{
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);

    GFX_Set(GFXF_DRAW_COLOR, color);
    GFX_DrawRect(rect->x, rect->y, rect->width, rect->height);
}

void laDraw_2x1BevelBorder(GFX_Rect* rect,
                           GFX_Color topOuterColor,
                           GFX_Color topInnerColor,
                           GFX_Color bottomOuterColor)
{
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);

    // top outer lines
    GFX_Set(GFXF_DRAW_COLOR, topOuterColor);
    GFX_DrawLine(rect->x,
                 rect->y,
                 rect->x + rect->width - 1,
                 rect->y);

    GFX_DrawLine(rect->x,
                 rect->y + 1,
                 rect->x,
                 rect->y + rect->height - 1);

    // top inner lines
    GFX_Set(GFXF_DRAW_COLOR, topInnerColor);
    GFX_DrawLine(rect->x + 1,
                 rect->y + 1,
                 rect->x + rect->width - 2,
                 rect->y + 1);

    GFX_DrawLine(rect->x + 1,
                 rect->y + 1,
                 rect->x + 1,
                 rect->y + rect->height - 2);

    // bottom outer lines
    GFX_Set(GFXF_DRAW_COLOR, bottomOuterColor);
    GFX_DrawLine(rect->x,
                 rect->y + rect->height - 1,
                 rect->x + rect->width - 1,
                 rect->y + rect->height - 1);
    
    GFX_DrawLine(rect->x + rect->width - 1,
                 rect->y + 1,
                 rect->x + rect->width - 1,
                 rect->y + rect->height - 1);
}

void laDraw_1x2BevelBorder(GFX_Rect* rect,
                           GFX_Color topColor,
                           GFX_Color bottomInnerColor,
                           GFX_Color bottomOuterColor)
{
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);

    // top outer lines
    GFX_Set(GFXF_DRAW_COLOR, topColor);
    GFX_DrawLine(rect->x,
                 rect->y,
                 rect->x + rect->width - 2,
                 rect->y);

    GFX_DrawLine(rect->x,
                 rect->y + 1,
                 rect->x,
                 rect->y + rect->height - 2);

    // bottom inner lines
    GFX_Set(GFXF_DRAW_COLOR, bottomInnerColor);
    GFX_DrawLine(rect->x + 1,
                 rect->y + rect->height - 2,
                 rect->x + rect->width - 2,
                 rect->y + rect->height - 2);
    
    GFX_DrawLine(rect->x + rect->width - 2,
                 rect->y + 1,
                 rect->x + rect->width - 2,
                 rect->y + rect->height - 3);

    // bottom outer lines
    GFX_Set(GFXF_DRAW_COLOR, bottomOuterColor);
    GFX_DrawLine(rect->x,
                 rect->y + rect->height - 1,
                 rect->x + rect->width - 1,
                 rect->y + rect->height - 1);
    
    GFX_DrawLine(rect->x + rect->width - 1,
                 rect->y,
                 rect->x + rect->width - 1,
                 rect->y + rect->height - 2);
}

void laDraw_2x2BevelBorder(GFX_Rect* rect,
                           GFX_Color topOuterColor,
                           GFX_Color topInnerColor,
                           GFX_Color bottomInnerColor,
                           GFX_Color bottomOuterColor)
{
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);

    // top outer lines
    GFX_Set(GFXF_DRAW_COLOR, topOuterColor);
    GFX_DrawLine(rect->x,
                 rect->y,
                 rect->x + rect->width - 2,
                 rect->y);

    GFX_DrawLine(rect->x,
                 rect->y + 1,
                 rect->x,
                 rect->y + rect->height - 2);

    // top inner lines
    GFX_Set(GFXF_DRAW_COLOR, topInnerColor);
    GFX_DrawLine(rect->x + 1,
                 rect->y + 1,
                 rect->x + rect->width - 3,
                 rect->y + 1);

    GFX_DrawLine(rect->x + 1,
                 rect->y + 1,
                 rect->x + 1,
                 rect->y + rect->height - 3);

    // bottom inner lines
    GFX_Set(GFXF_DRAW_COLOR, bottomInnerColor);
    GFX_DrawLine(rect->x + 1,
                 rect->y + rect->height - 2,
                 rect->x + rect->width - 2,
                 rect->y + rect->height - 2);
    
    GFX_DrawLine(rect->x + rect->width - 2,
                 rect->y + 1,
                 rect->x + rect->width - 2,
                 rect->y + rect->height - 3);

    // bottom outer lines
    GFX_Set(GFXF_DRAW_COLOR, bottomOuterColor);
    GFX_DrawLine(rect->x,
                 rect->y + rect->height - 1,
                 rect->x + rect->width - 1,
                 rect->y + rect->height - 1);
    
    GFX_DrawLine(rect->x + rect->width - 1,
                 rect->y,
                 rect->x + rect->width - 1,
                 rect->y + rect->height - 2);
}