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
#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_util.h"

#if GFX_DRAW_PIPELINE_ENABLED && GFX_ALPHA_BLENDING_ENABLED

static uint8_t calculateBlendColor(uint8_t lclr,
                                   uint32_t lper,
                                   uint8_t rclr,
                                   uint32_t rper,
                                   uint8_t max)
{
    uint8_t res = 0;

    /*lper *= (float)lclr;    // translate to color space
    lper += 0.5f;           // round
    lclr = (uint8_t)(lper); // truncate

    rper *= (float)rclr;    // translate to color space
    rper += 0.5f;           // round
    rclr = (uint8_t)(rper); // truncate
    
    res = lclr + rclr; // sum
    
    if(res > max) // clamp
        res = max;*/

    lper *= lclr;        // translate to color space
    lper += 50;          // round
    lclr = lper / 10000; // truncate

    rper *= rclr;        // translate to color space
    rper += 50;          // round
    rclr = rper / 10000; // truncate
    
    /*if(lclr != 0)
        lclr = GFX_PercentWholeRounded(lper, lclr);
    else
        lclr = 0;
        
    if(rclr != 0)
        rclr = GFX_PercentWholeRounded(rper, rclr);
    else
        rclr = 0;*/
        
    res = lclr + rclr;   // sum

    if(res > max)        // clamp
        res = max;

    return res;
}

GFX_Color GFX_ColorBlend_RGBA_8888(GFX_Color c1, GFX_Color c2)
{
    GFX_Color result;
    uint32_t ia1, /*ia2,*/ oneMinus_ia1;
    uint32_t c1a, c2a;

    result = 0;
        
    c1a = (c1 & RGBA_8888_ALPHA_MASK);
    ia1 = GFX_Percent(c1a, 255);
    
    c2a = (c2 & RGBA_8888_ALPHA_MASK);

    // calculate result alpha
    //ia2 = GFX_Percent(c2a, 255);
    oneMinus_ia1 = 10000 - ia1;

    // red channel
    result |= calculateBlendColor((c1 & RGBA_8888_RED_MASK) >> 24,
                                  ia1,
                                  (c2 & RGBA_8888_RED_MASK) >> 24,
                                  oneMinus_ia1,
                                  255) << 24;

    // green channel
    result |= calculateBlendColor((c1 & RGBA_8888_GREEN_MASK) >> 16,
                                  ia1,
                                  (c2 & RGBA_8888_GREEN_MASK) >> 16,
                                  oneMinus_ia1,
                                  255) << 16;

    // blue channel
    result |= calculateBlendColor((c1 & RGBA_8888_BLUE_MASK) >> 8,
                                  ia1,
                                  (c2 & RGBA_8888_BLUE_MASK) >> 8,
                                  oneMinus_ia1,
                                  255) << 8;
    
    // alpha channel
    result |= calculateBlendColor(c1a,
                                  ia1,
                                  c2a,
                                  oneMinus_ia1,
                                  255);
    
    return result;
}

#endif // GFX_DRAW_PIPELINE_ENABLED && GFX_ALPHA_BLENDING_ENABLED