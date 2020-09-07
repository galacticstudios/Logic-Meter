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


#include "gfx/hal/inc/gfx_color.h"

#if GFX_DRAW_PIPELINE_ENABLED

#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_util.h"

typedef GFX_Color (*color_lerp_fn_ptr)(GFX_Color, GFX_Color, uint32_t);

/* color conversion function definitions */
static GFX_Color lerp_gs8(GFX_Color l, GFX_Color r, uint32_t percent);
static GFX_Color lerp_rgb332(GFX_Color l, GFX_Color r, uint32_t percent);
static GFX_Color lerp_rgb565(GFX_Color l, GFX_Color r, uint32_t percent);
static GFX_Color lerp_rgba5551(GFX_Color l, GFX_Color r, uint32_t percent);
static GFX_Color lerp_rgb888(GFX_Color l, GFX_Color r, uint32_t percent);
static GFX_Color lerp_rgba8888(GFX_Color l, GFX_Color r, uint32_t percent);
static GFX_Color lerp_argb8888(GFX_Color l, GFX_Color r, uint32_t percent);
//static GFX_Color lerp_yuv(GFX_Color l, GFX_Color r, uint32_t percent);

/* color lerp function matrix */
color_lerp_fn_ptr color_lerp_table[GFX_COLOR_MODE_COUNT] =
{
    &lerp_gs8,      /* GS8       */
    &lerp_rgb332,   /* RGB332    */
    &lerp_rgb565,   /* RGB565    */
    &lerp_rgba5551, /* RGBA5551  */
    &lerp_rgb888,   /* RGB888    */
    &lerp_rgba8888, /* RGBA8888  */
    &lerp_argb8888, /* ARGB8888  */
};

GFX_Color lerp_gs8(GFX_Color l, GFX_Color r, uint32_t percent)
{
    return GFX_Lerp((int32_t)l, (int32_t)r, percent);
}

GFX_Color lerp_rgb332(GFX_Color l, GFX_Color r, uint32_t percent)
{
    uint32_t lchan, rchan, lerp, res;
    
    res = 0;
    
    // red
    lchan = (l & 0xE0) >> 5;
    rchan = (r & 0xE0) >> 5;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 5;
    
    // green
    lchan = (l & 0x1C) >> 2;
    rchan = (r & 0x1C) >> 2;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 2;
    
    // blue
    lchan = (l & 0x3);
    rchan = (r & 0x3);
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp;
    
    return res;
}

GFX_Color lerp_rgb565(GFX_Color l, GFX_Color r, uint32_t percent)
{
    uint32_t lchan, rchan, lerp, res;
    
    res = 0;
    
    // red
    lchan = (l & 0xF800) >> 11;
    rchan = (r & 0xF800) >> 11;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 11;
    
    // green
    lchan = (l & 0x7E0) >> 5;
    rchan = (r & 0x7E0) >> 5;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 5;
    
    // blue
    lchan = (l & 0x1F);
    rchan = (r & 0x1F);
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp;
    
    return res;
}

GFX_Color lerp_rgba5551(GFX_Color l, GFX_Color r, uint32_t percent)
{
    uint32_t lchan, rchan, lerp, res;
    
    res = 0;
    
    // red
    lchan = (l & 0xF800) >> 11;
    rchan = (r & 0xF800) >> 11;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 11;
    
    // green
    lchan = (l & 0x7C0) >> 6;
    rchan = (r & 0x7C0) >> 6;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 6;
    
    // blue
    lchan = (l & 0x3E) >> 1;
    rchan = (r & 0x3E) >> 1;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 1;
    
    // interpolate binary alpha channel, if either is on then on
    if((l & 0x1) > 0 || (r & 0x1) > 0)
        res |= 0x1;
    
    return res;
}

GFX_Color lerp_rgb888(GFX_Color l, GFX_Color r, uint32_t percent)
{
    uint32_t lchan, rchan, lerp, res;
    
    res = 0;
    
    // red
    lchan = (l & 0xFF0000) >> 16;
    rchan = (r & 0xFF0000) >> 16;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 16;
    
    // green
    lchan = (l & 0xFF00) >> 8;
    rchan = (r & 0xFF00) >> 8;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 8;
    
    // blue
    lchan = (l & 0xFF);
    rchan = (r & 0xFF);
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp;
    
    return res;
}

GFX_Color lerp_rgba8888(GFX_Color l, GFX_Color r, uint32_t percent)
{
    uint32_t lchan, rchan, lerp, res;
    
    res = 0;
    
    // red
    lchan = (l & 0xFF000000) >> 24;
    rchan = (r & 0xFF000000) >> 24;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 24;
    
    // green
    lchan = (l & 0xFF0000) >> 16;
    rchan = (r & 0xFF0000) >> 16;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 16;
    
    // blue
    lchan = (l & 0xFF00) >> 8;
    rchan = (r & 0xFF00) >> 8;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 8;
    
    // alpha
    lchan = (l & 0xFF);
    rchan = (r & 0xFF);
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp;
    
    return res;
}

GFX_Color lerp_argb8888(GFX_Color l, GFX_Color r, uint32_t percent)
{
    uint32_t lchan, rchan, lerp, res;
    
    res = 0;
    
    // alpha
    lchan = (l & 0xFF000000) >> 24;
    rchan = (r & 0xFF000000) >> 24;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 24;
    
    // red
    lchan = (l & 0xFF0000) >> 16;
    rchan = (r & 0xFF0000) >> 16;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 16;
    
    // green
    lchan = (l & 0xFF00) >> 8;
    rchan = (r & 0xFF00) >> 8;
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp << 8;
    
    // blue
    lchan = (l & 0xFF);
    rchan = (r & 0xFF);
    
    lerp = GFX_Lerp((int32_t)lchan, (int32_t)rchan, percent);
    
    res |= lerp;
    
    return res;
}

/*GFX_Color lerp_yuv(GFX_Color l, GFX_Color r, uint32_t percent)
{
    // not implemented yet
    return 0;
}*/

GFX_Color GFX_ColorLerp(GFX_Color l,
                        GFX_Color r,
                        uint32_t percent,
                        GFX_ColorMode mode)
{
    if(percent == 0)
        return l;
        
    if(percent == 100)
        return r;
        
    if(l == r)
        return l;
        
    return color_lerp_table[mode](l, r, percent);
}

GFX_Color GFX_ColorBilerp(GFX_Color c00,
                          GFX_Color c01,
                          GFX_Color c10,
                          GFX_Color c11,
                          uint32_t xper,
                          uint32_t yper,
                          GFX_ColorMode mode)
{
    GFX_Color r1, r2, p;
    
    if(xper == 0 && yper == 0)
        return c00;
    
    r1 = GFX_ColorLerp(c00, c01, xper, mode);
    r2 = GFX_ColorLerp(c10, c11, xper, mode);
    
    p = GFX_ColorLerp(r1, r2, yper, mode);
    
    return p;
}

#endif // GFX_DRAW_PIPELINE_ENABLED