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


#include "gfx/hal/inc/gfx_draw.h"

#if GFX_DRAW_PIPELINE_ENABLED

#include <math.h>

#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_rect.h"
#include "gfx/hal/inc/gfx_util.h"

static GFX_Result rejectCircle(int32_t x,
                               int32_t y,
                               int32_t radius,
                               const GFX_DrawState* state)
{
    GFX_Rect rect;

#if GFX_COLOR_MASKING_ENABLED
    // mask color
    if(state->maskEnable == GFX_TRUE && state->color == state->maskValue)
       return GFX_FAILURE;
#endif

#if GFX_ALPHA_BLENDING_ENABLED
    // alpha is zero
    if((state->blendMode & GFX_BLEND_GLOBAL) > 0 &&
        state->globalAlphaValue == 0)
       return GFX_FAILURE;
#endif
    
    rect.x = x;
    rect.width = radius + radius;
    rect.y = y;
    rect.height = rect.width;    

#if GFX_LAYER_CLIPPING_ENABLED
    // clip rect against layer rect
    if(GFX_RectIntersects(&state->targetClipRect, &rect) == GFX_FALSE)
        return GFX_FAILURE;
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip rect against global clipping rect
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, &rect) == GFX_FALSE)
        return GFX_FAILURE;
#endif

    return GFX_SUCCESS;
}

GFX_Result GFX_DrawCircle(int32_t x,
                          int32_t y,
                          int32_t radius)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point pt;
    GFX_Result res;
    GFX_DrawState state;
   
    GFX_ASSERT(context != NULL);

    if(context == NULL)
        return GFX_FAILURE;
        
    state = context->draw;
        
    if(state.target == NULL)
    {
        if(context->layer.active == NULL ||
           context->layer.active->locked == GFX_FALSE)
            return GFX_FAILURE;
            
        state.target = &context->layer.active->buffers[context->layer.active->buffer_write_idx].pb;
        state.targetClipRect = context->layer.active->rect.local;
    }
    
    state.colorMode = state.target->mode;

    pt.x = x;
    pt.y = y;
    
    if(rejectCircle(x, y, radius, &state) == GFX_FAILURE)
        return GFX_FAILURE;
    
    res = state.pipeline->drawCircle[state.mode]
                                    [state.antialias]
                                    (&pt, radius, &state);

    return res;
}

GFX_Result cpuDrawCircle_Line(const GFX_Point* pt,
                              int32_t r,
                              const GFX_DrawState* state)
{
    int x = -r, y = 0, err = 2-2*r;
    int xm = pt->x;
    int ym = pt->y;   
    
    GFX_Point dp;
    
    do
    {
        dp.x = xm-x;
        dp.y = ym+y;
        state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&dp, state);        
        dp.x = xm-y;
        dp.y = ym-x;
        state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&dp, state);        
        dp.x = xm+x;
        dp.y = ym-y;
        state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&dp, state);
        
        dp.x = xm+y;
        dp.y = ym+x;
        state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&dp, state);
        
        r = err;
        if (r <= y) 
            err += ++y*2+1;           /* e_xy+e_y < 0 */
            
        if (r > x || err > y)
             err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
             
    } 
    while (x < 0);
    
    return GFX_SUCCESS;
}

#endif // GFX_DRAW_PIPELINE_ENABLED