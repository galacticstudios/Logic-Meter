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
#include "gfx/hal/inc/gfx_pixel_buffer.h"
#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_util.h"

typedef GFX_Color (*color_convert_fn_ptr)(GFX_Color);

static GFX_Color no_conv(GFX_Color color);

/* color conversion function definitions */
static GFX_Color gs8_rgb332(GFX_Color color);
static GFX_Color gs8_rgb565(GFX_Color color);
static GFX_Color gs8_rgba5551(GFX_Color color);
static GFX_Color gs8_rgb888(GFX_Color color);
static GFX_Color gs8_rgba8888(GFX_Color color);
static GFX_Color gs8_argb8888(GFX_Color color);
static GFX_Color gs8_yuv(GFX_Color color);

static GFX_Color rgb332_gs8(GFX_Color color);
static GFX_Color rgb332_rgb565(GFX_Color color);
static GFX_Color rgb332_rgba5551(GFX_Color color);
static GFX_Color rgb332_rgb888(GFX_Color color);
static GFX_Color rgb332_rgba8888(GFX_Color color);
static GFX_Color rgb332_argb8888(GFX_Color color);
static GFX_Color rgb332_yuv(GFX_Color color);

static GFX_Color rgb565_gs8(GFX_Color color);
static GFX_Color rgb565_rgb332(GFX_Color color);
static GFX_Color rgb565_rgba5551(GFX_Color color);
static GFX_Color rgb565_rgb888(GFX_Color color);
static GFX_Color rgb565_rgba8888(GFX_Color color);
static GFX_Color rgb565_argb8888(GFX_Color color);
static GFX_Color rgb565_yuv(GFX_Color color);

static GFX_Color rgba5551_gs8(GFX_Color color);
static GFX_Color rgba5551_rgb332(GFX_Color color);
static GFX_Color rgba5551_rgb565(GFX_Color color);
static GFX_Color rgba5551_rgb888(GFX_Color color);
static GFX_Color rgba5551_rgba8888(GFX_Color color);
static GFX_Color rgba5551_argb8888(GFX_Color color);
static GFX_Color rgba5551_yuv(GFX_Color color);

static GFX_Color rgb888_gs8(GFX_Color color);
static GFX_Color rgb888_rgb332(GFX_Color color);
static GFX_Color rgb888_rgb565(GFX_Color color);
static GFX_Color rgb888_rgba5551(GFX_Color color);
static GFX_Color rgb888_rgba8888(GFX_Color color);
static GFX_Color rgb888_argb8888(GFX_Color color);
static GFX_Color rgb888_yuv(GFX_Color color);

static GFX_Color rgba8888_gs8(GFX_Color color);
static GFX_Color rgba8888_rgb332(GFX_Color color);
static GFX_Color rgba8888_rgb565(GFX_Color color);
static GFX_Color rgba8888_rgba5551(GFX_Color color);
static GFX_Color rgba8888_rgb888(GFX_Color color);
static GFX_Color rgba8888_argb8888(GFX_Color color);
static GFX_Color rgba8888_yuv(GFX_Color color);

static GFX_Color argb8888_rgba5551(GFX_Color color);
static GFX_Color argb8888_rgb888(GFX_Color color);
static GFX_Color argb8888_rgba8888(GFX_Color color);
//static GFX_Color argb8888_yuv(GFX_Color color);

static GFX_Color yuv_gs8(GFX_Color color);
static GFX_Color yuv_rgb332(GFX_Color color);
static GFX_Color yuv_rgb565(GFX_Color color);
static GFX_Color yuv_rgba5551(GFX_Color color);
static GFX_Color yuv_rgb888(GFX_Color color);
static GFX_Color yuv_rgba8888(GFX_Color color);
static GFX_Color yuv_argb8888(GFX_Color color);

static GFX_Color lut_gs8(GFX_Color color);
static GFX_Color lut_rgb332(GFX_Color color);
static GFX_Color lut_rgb565(GFX_Color color);
static GFX_Color lut_rgba5551(GFX_Color color);
static GFX_Color lut_rgb888(GFX_Color color);
static GFX_Color lut_rgba8888(GFX_Color color);
static GFX_Color lut_argb8888(GFX_Color color);
static GFX_Color lut_yuv(GFX_Color color);

/* color conversion function matrix */
color_convert_fn_ptr color_convert_table[GFX_COLOR_MODE_COUNT][GFX_COLOR_MODE_COUNT] =
{
//                GS8            RGB332            RGB565            RGBA_5551           RGB_888           RGBA_8888           ARGB_8888           YUV            LUT1      LUT4      LUT8
/* GS8       */ { &no_conv,      &gs8_rgb332,      &gs8_rgb565,      &gs8_rgba5551,      &gs8_rgb888,      &gs8_rgba8888,      &gs8_argb8888,      &gs8_yuv,      &no_conv, &no_conv, &no_conv },
/* RGB332    */ { &rgb332_gs8,   &no_conv,         &rgb332_rgb565,   &rgb332_rgba5551,   &rgb332_rgb888,   &rgb332_rgba8888,   &rgb332_argb8888,   &rgb332_yuv,   &no_conv, &no_conv, &no_conv },
/* RGB565    */ { &rgb565_gs8,   &rgb565_rgb332,   &no_conv,         &rgb565_rgba5551,   &rgb565_rgb888,   &rgb565_rgba8888,   &rgb565_argb8888,   &rgb565_yuv,   &no_conv, &no_conv, &no_conv },
/* RGBA_5551 */ { &rgba5551_gs8, &rgba5551_rgb332, &rgba5551_rgb565, &no_conv,           &rgba5551_rgb888, &rgba5551_rgba8888, &rgba5551_argb8888, &rgba5551_yuv, &no_conv, &no_conv, &no_conv },
/* RGB_888   */ { &rgb888_gs8,   &rgb888_rgb332,   &rgb888_rgb565,   &rgb888_rgba5551,   &no_conv,         &rgb888_rgba8888,   &rgb888_argb8888,   &rgb888_yuv,   &no_conv, &no_conv, &no_conv },
/* RGBA_8888 */ { &rgba8888_gs8, &rgba8888_rgb332, &rgba8888_rgb565, &rgba8888_rgba5551, &rgba8888_rgb888, &no_conv,           &rgba8888_argb8888, &rgba8888_yuv, &no_conv, &no_conv, &no_conv },
/* ARGB_8888 */ { &rgb888_gs8,   &rgb888_rgb332,   &rgb888_rgb565,   &argb8888_rgba5551, &argb8888_rgb888, &argb8888_rgba8888, &no_conv,           &rgb888_yuv,   &no_conv, &no_conv, &no_conv },
/* YUV       */ { &yuv_gs8,      &yuv_rgb332,      &yuv_rgb565,      &yuv_rgba5551,      &yuv_rgb888,      &yuv_rgba8888,      &yuv_argb8888,      &no_conv,      &no_conv, &no_conv, &no_conv },
/* LUT1      */ { &lut_gs8,      &lut_rgb332,      &lut_rgb565,      &lut_rgba5551,      &lut_rgb888,      &lut_rgba8888,      &lut_argb8888,      &lut_yuv,      &no_conv, &no_conv, &no_conv },
/* LUT4      */ { &lut_gs8,      &lut_rgb332,      &lut_rgb565,      &lut_rgba5551,      &lut_rgb888,      &lut_rgba8888,      &lut_argb8888,      &lut_yuv,      &no_conv, &no_conv, &no_conv },
/* LUT8      */ { &lut_gs8,      &lut_rgb332,      &lut_rgb565,      &lut_rgba5551,      &lut_rgb888,      &lut_rgba8888,      &lut_argb8888,      &lut_yuv,      &no_conv, &no_conv, &no_conv },
};

#if 0
static GFX_Color rgb_yuv(uint8_t red, uint8_t green, uint8_t blue)
{
    return 0;
}

static GFX_Color yuv_rgb(uint8_t y, uint8_t u, uint8_t v)
{
    return 0;
}
#endif

/*
  From RGB to YUV

   Y = 0.299R + 0.587G + 0.114B
   U = 0.492 (B-Y)
   V = 0.877 (R-Y)

   It can also be represented as:

   Y =  0.299R + 0.587G + 0.114B
   U = -0.147R - 0.289G + 0.436B
   V =  0.615R - 0.515G - 0.100B


   From YUV to RGB

   R = Y + 1.140V
   G = Y - 0.395U - 0.581V
   B = Y + 2.032U
*/

static GFX_Color no_conv(GFX_Color color)
{
    return color;
}

/*
grayscale conversion formula
0.21 R + 0.72 G + 0.07 B
*/

static GFX_Color gs8_rgb332(GFX_Color color)
{
    GFX_Color res;
    
    // red
    res = color & RGB_332_RED_MASK;
    
    // green
    res |= res >> 3;
    
    // blue
    res |= res >> 6;
    
    return res;
}

static GFX_Color gs8_rgb565(GFX_Color color)
{
    GFX_Color res;
    
    // red
    res = (color & 0xF8) << 8;
    
    // green
    res |= (color & 0xFC) << 3;
    
    // blue
    res |= (color & 0xF8) >> 3;
    
    return res;
}

static GFX_Color gs8_rgba5551(GFX_Color color)
{
    GFX_Color res;
    
    // red
    res = (color & 0xF8) << 8;
    
    // green
    res |= (color & 0xF8) << 3;
    
    // blue
    res |= (color & 0xF8) >> 2;
    
    // alpha
    res |= RGBA_5551_ALPHA_MASK;
    
    return res;
}

static GFX_Color gs8_rgb888(GFX_Color color)
{
    // blue
    GFX_Color res = color;
    
    // red
    res = color << 16;
    
    // green
    res |= color << 8;
    
    // green
    res |= color;
    
    return res;
}

static GFX_Color gs8_rgba8888(GFX_Color color)
{
    GFX_Color res;
    
    // red
    res = color << 24;
    
    // green
    res |= color << 16;
    
    // blue
    res |= color << 8;
    
    // alpha
    res |= 0xFF;
    
    return res;
}

static GFX_Color gs8_argb8888(GFX_Color color)
{
    GFX_Color res;
    
    // red
    res = color << 16;
    
    // green
    res |= color << 8;
    
    // blue
    res |= color;
    
    // alpha
    res |= 0xFF000000;
    
    return res;
}

static GFX_Color gs8_yuv(GFX_Color color)
{
    color = 0;
    
    return 0x0;
}

static GFX_Color rgb332_gs8(GFX_Color color)
{
    //0.21 R + 0.72 G + 0.07 B
    uint32_t red, green, blue;
    
    red = (color & RGB_332_RED_MASK) >> 5;
    red = GFX_ScaleInteger(red, RGB_3_BITS, 55);
    
    green = (color & RGB_332_GREEN_MASK) >> 2;
    green = GFX_ScaleInteger(red, RGB_3_BITS, 183);
    
    blue = (color & RGB_332_BLUE_MASK);
    blue = GFX_ScaleInteger(red, RGB_2_BITS, 17);
    
    return red + green + blue;
}

static GFX_Color rgb332_rgb565(GFX_Color color)
{
    uint32_t red, green, blue;
        
    red = ((color & RGB_332_RED_MASK) << 8) | 0x1800;
    green = ((color & RGB_332_GREEN_MASK) << 6) | 0xE0;
    blue = ((color & RGB_332_BLUE_MASK) << 3) | 0x3;
    
    return red | green | blue;
}

static GFX_Color rgb332_rgba5551(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = ((color & RGB_332_RED_MASK) << 8) | 0x1800;
    green = ((color & RGB_332_GREEN_MASK) << 6) | 0xC0;
    blue = ((color & RGB_332_BLUE_MASK) << 4) | 0x6;
    
    return red | green | blue;
}

static GFX_Color rgb332_rgb888(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = ((color & RGB_332_RED_MASK) << 16) | 0x1F0000;
    green = ((color & RGB_332_GREEN_MASK) << 11) | 0x1F00;
    blue = ((color & RGB_332_BLUE_MASK) << 6) | 0x1F;
    
    return red | green | blue;
}

static GFX_Color rgb332_rgba8888(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = ((color & RGB_332_RED_MASK) << 24) | 0x1F000000;
    green = ((color & RGB_332_GREEN_MASK) << 19) | 0x1F0000;
    blue = ((color & RGB_332_BLUE_MASK) << 14) | 0x1F00;
    
    return red | green | blue | 0xFF;
}

static GFX_Color rgb332_argb8888(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = ((color & RGB_332_RED_MASK) << 16) | 0x1F0000;
    green = ((color & RGB_332_GREEN_MASK) << 11) | 0x1F00;
    blue = ((color & RGB_332_BLUE_MASK) << 6) | 0x1F;
    
    return 0xFF000000 | red | green | blue;
}

static GFX_Color rgb332_yuv(GFX_Color color)
{
    

    return color;
}

static GFX_Color rgb565_gs8(GFX_Color color)
{
    //0.21 R + 0.72 G + 0.07 B
    uint32_t red, green, blue;
    
    red = (color & RGB_565_RED_MASK) >> 11;
    red = GFX_ScaleInteger(red, 7, 55);
    
    green = (color & RGB_565_GREEN_MASK) >> 5;
    green = GFX_ScaleInteger(red, 7, 183);
    
    blue = (color & RGB_565_BLUE_MASK);
    blue = GFX_ScaleInteger(red, 7, 17);
    
    return red + green + blue;
}

static GFX_Color rgb565_rgb332(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xE000) >> 8;
    green = (color & 0x700) >> 6;
    blue = (color & 0x18) >> 3;
    
    return red | green | blue;
}

static GFX_Color rgb565_rgba5551(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & RGB_565_RED_MASK);
    green = (color & RGBA_5551_GREEN_MASK);
    blue = (color & RGB_565_BLUE_MASK) << 1;
    
    return red | green | blue | RGBA_5551_ALPHA_MASK;
}

static GFX_Color rgb565_rgb888(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & RGB_565_RED_MASK) << 8;
    green = (color & RGB_565_GREEN_MASK) << 5;
    blue = (color & RGB_565_BLUE_MASK) << 3;
    
    return red | green | blue;
}

static GFX_Color rgb565_rgba8888(GFX_Color color)
{
    uint32_t red, green, blue;
 
    red = (color & RGB_565_RED_MASK) << 16;
    green = (color & RGB_565_GREEN_MASK) << 13;
    blue = (color & RGB_565_BLUE_MASK) << 11;
    
    return red | green | blue | 0xFF;
}

static GFX_Color rgb565_argb8888(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & RGB_565_RED_MASK) << 8;
    green = (color & RGB_565_GREEN_MASK) << 5;
    blue = (color & RGB_565_BLUE_MASK) << 3;
    
    return 0xFF000000 | red | green | blue;
}

static GFX_Color rgb565_yuv(GFX_Color color)
{
    return color;
}

static GFX_Color rgba5551_gs8(GFX_Color color)
{
    return color;
}

static GFX_Color rgba5551_rgb332(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xE000) >> 8;
    green = (color & 0x700) >> 6;
    blue = (color & 0x30) >> 4;
    
    return red | green | blue;
}

static GFX_Color rgba5551_rgb565(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & RGBA_5551_RED_MASK);
    green = (color & RGBA_5551_GREEN_MASK) | 0x20;
    blue = (color & RGBA_5551_BLUE_MASK) >> 1;
    
    return red | green | blue;
}

static GFX_Color rgba5551_rgb888(GFX_Color color)
{    
    uint32_t red, green, blue;
    
    red = ((color & RGBA_5551_RED_MASK) << 8) | 0x70000;
    green = ((color & RGBA_5551_GREEN_MASK) << 5) | 0x700;
    blue = ((color & RGBA_5551_BLUE_MASK) << 2) | 0x7;
    
    return red | green | blue;
}

static GFX_Color rgba5551_rgba8888(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = ((color & RGBA_5551_RED_MASK) << 16) | 0x7000000;
    green = ((color & RGBA_5551_GREEN_MASK) << 13) | 0x70000;
    blue = ((color & RGBA_5551_BLUE_MASK) << 10) | 0x700;
    
    return red | green | blue | ((color & RGBA_5551_ALPHA_MASK) > 0 ? 0xFF : 0x0);
}

static GFX_Color rgba5551_argb8888(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = ((color & RGBA_5551_RED_MASK) << 8) | 0x70000;
    green = ((color & RGBA_5551_GREEN_MASK) << 5) | 0x700;
    blue = ((color & RGBA_5551_BLUE_MASK) << 2) | 0x7;
    
    return ((color & RGBA_5551_ALPHA_MASK) > 0 ? 0xFF000000 : 0x0) | red | green | blue;
}

static GFX_Color rgba5551_yuv(GFX_Color color)
{
    return color;
}

static GFX_Color rgb888_gs8(GFX_Color color)
{
    return color;
}

static GFX_Color rgb888_rgb332(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xE00000) >> 17;
    green = (color & 0xE000) >> 8;
    blue = (color & 0xC0) >> 4;
    
    return red | green | blue;
}

static GFX_Color rgb888_rgb565(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xF80000) >> 8;
    green = (color & 0xFC00) >> 5;
    blue = (color & 0xF8) >> 3;
    
    return red | green | blue;
}

static GFX_Color rgb888_rgba5551(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xF80000) >> 8;
    green = (color & 0xF800) >> 5;
    blue = (color & 0xF8) >> 2;
    
    return red | green | blue | RGBA_5551_ALPHA_MASK;
}

static GFX_Color rgb888_rgba8888(GFX_Color color)
{
    return (color << 8) | 0xFF;
}

static GFX_Color rgb888_argb8888(GFX_Color color)
{
    return color | 0xFF000000;
}

static GFX_Color rgb888_yuv(GFX_Color color)
{
    return color;
}

static GFX_Color rgba8888_gs8(GFX_Color color)
{
    return color;
}

static GFX_Color rgba8888_rgb332(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xE0000000) >> 24;
    green = (color & 0xE00000) >> 19;
    blue = (color & 0xC000) >> 14;
    
    return red | green | blue;
}

static GFX_Color rgba8888_rgb565(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xF8000000) >> 16;
    green = (color & 0xFC0000) >> 13;
    blue = (color & 0xF800) >> 11;
    
    return red | green | blue;
}

static GFX_Color rgba8888_rgba5551(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xF8000000) >> 16;
    green = (color & 0xF80000) >> 13;
    blue = (color & 0xF800) >> 10;
    
    return red | green | blue | ((color & RGBA_8888_ALPHA_MASK) > 0 ? RGBA_5551_ALPHA_MASK : 0);
}

static GFX_Color rgba8888_rgb888(GFX_Color color)
{
    GFX_Color res;
    
    // red
    res  = (color & 0xFF000000) >> 8;
    
    // green
    res |= (color & 0x00FF0000) >> 8;
    
    // blue
    res |= (color & 0x0000FF00) >> 8;

    return res;
}

static GFX_Color rgba8888_argb8888(GFX_Color color)
{
    return (color >> 8) | (((color & 0xFF) << 24));
}

static GFX_Color rgba8888_yuv(GFX_Color color)
{
    return color;
}

static GFX_Color argb8888_rgba5551(GFX_Color color)
{
    uint32_t red, green, blue;
    
    red = (color & 0xF80000) >> 8;
    green = (color & 0xF800) >> 5;
    blue = (color & 0xF8) >> 2;
    
    return red | green | blue | ((color & RGBA_8888_ALPHA_MASK) > 0 ? RGBA_5551_ALPHA_MASK : 0);
}

static GFX_Color argb8888_rgb888(GFX_Color color)
{
    return color & 0xFFFFFF;
}

static GFX_Color argb8888_rgba8888(GFX_Color color)
{
    return (color << 8) | ((color & 0xFF000000) >> 24);
}

/*static GFX_Color argb8888_yuv(GFX_Color color)
{
    return color;
}*/

static GFX_Color yuv_gs8(GFX_Color color)
{
    return color;
}

static GFX_Color yuv_rgb332(GFX_Color color)
{
    return color;
}

static GFX_Color yuv_rgb565(GFX_Color color)
{
    return color;
}

static GFX_Color yuv_rgba5551(GFX_Color color)
{
    return color;
}

static GFX_Color yuv_rgb888(GFX_Color color)
{
    return color;
}

static GFX_Color yuv_rgba8888(GFX_Color color)
{
    return color;
}

static GFX_Color yuv_argb8888(GFX_Color color)
{
    return color;
}

static GFX_Color lut_gs8(GFX_Color color)
{
    GFX_Color res;
    GFX_Context* context = GFX_ActiveContext();
    
    if(context->draw.palette.pixels == GFX_NULL)
        return 0;
        
    res = GFX_PixelBufferGetIndex(&context->draw.palette, color);
                       
    return GFX_ColorConvert(context->draw.palette.mode,
                            GFX_COLOR_MODE_GS_8,
                            res);
}

static GFX_Color lut_rgb332(GFX_Color color)
{
    GFX_Color res;
    GFX_Context* context = GFX_ActiveContext();
    
    if(context->draw.palette.pixels == GFX_NULL)
        return 0;
        
    res = GFX_PixelBufferGetIndex(&context->draw.palette, color);
                       
    return GFX_ColorConvert(context->draw.palette.mode,
                            GFX_COLOR_MODE_RGB_332,
                            res);
}

static GFX_Color lut_rgb565(GFX_Color color)
{
    GFX_Color res;
    GFX_Context* context = GFX_ActiveContext();
    
    if(context->draw.palette.pixels == GFX_NULL)
        return 0;
        
    res = GFX_PixelBufferGetIndex(&context->draw.palette, color);
                       
    return GFX_ColorConvert(context->draw.palette.mode,
                            GFX_COLOR_MODE_RGB_565,
                            res);
}

static GFX_Color lut_rgba5551(GFX_Color color)
{
    GFX_Color res;
    GFX_Context* context = GFX_ActiveContext();
    
    if(context->draw.palette.pixels == GFX_NULL)
        return 0;
        
    res = GFX_PixelBufferGetIndex(&context->draw.palette, color);
                       
    return GFX_ColorConvert(context->draw.palette.mode,
                            GFX_COLOR_MODE_RGBA_5551,
                            res);
}

static GFX_Color lut_rgb888(GFX_Color color)
{
    GFX_Color res;
    GFX_Context* context = GFX_ActiveContext();
    
    if(context->draw.palette.pixels == GFX_NULL)
        return 0;
        
    res = GFX_PixelBufferGetIndex(&context->draw.palette, color);
                       
    return GFX_ColorConvert(context->draw.palette.mode,
                            GFX_COLOR_MODE_RGB_888,
                            res);
}

static GFX_Color lut_rgba8888(GFX_Color color)
{
    GFX_Color res;
    GFX_Context* context = GFX_ActiveContext();
    
    if(context->draw.palette.pixels == GFX_NULL)
        return 0;
        
    res = GFX_PixelBufferGetIndex(&context->draw.palette, color);
                       
    return GFX_ColorConvert(context->draw.palette.mode,
                            GFX_COLOR_MODE_RGBA_8888,
                            res);
}

static GFX_Color lut_argb8888(GFX_Color color)
{
    GFX_Color res;
    GFX_Context* context = GFX_ActiveContext();
    
    if(context->draw.palette.pixels == GFX_NULL)
        return 0;
        
    res = GFX_PixelBufferGetIndex(&context->draw.palette, color);
                       
    return GFX_ColorConvert(context->draw.palette.mode,
                            GFX_COLOR_MODE_ARGB_8888,
                            res);
}

static GFX_Color lut_yuv(GFX_Color color)
{
    return color;
}

GFX_Color GFX_ColorConvert(GFX_ColorMode mode_in,
                           GFX_ColorMode mode_out,
                           GFX_Color color)
{
    return color_convert_table[mode_in][mode_out](color);
}

#endif // GFX_DRAW_PIPELINE_ENABLED