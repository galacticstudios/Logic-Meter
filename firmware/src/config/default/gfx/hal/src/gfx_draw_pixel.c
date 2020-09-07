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


#include "gfx/hal/inc/gfx_draw_pixel.h"

#if GFX_DRAW_PIPELINE_ENABLED

#include "gfx/hal/inc/gfx_color.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_pixel_buffer.h"
#include "gfx/hal/inc/gfx_rect.h"
#include "gfx/hal/inc/gfx_util.h"

static GFX_Result rejectPixel(const GFX_Point* p, 
                              const GFX_DrawState* state)
{       
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

#if GFX_LAYER_CLIPPING_ENABLED
    // clip point against layer rect
    if(GFX_RectContainsPoint(&state->targetClipRect, p) == GFX_FALSE)
        return GFX_FAILURE;
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip point against global clipping rect
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectContainsPoint(&state->clipRect, p) == GFX_FALSE)
        return GFX_FAILURE;
#endif

    return GFX_SUCCESS;
}

GFX_Result GFX_DrawPixel(int32_t x, int32_t y)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point p;
    GFX_Result res;
    GFX_DrawState state;
    
    GFX_ASSERT(context != NULL);

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

    p.x = x;
    p.y = y;
    
    if(rejectPixel(&p, &state) == GFX_FAILURE)
        return GFX_FAILURE;
    
    res = state.pipeline->drawPixel[context->draw.antialias](&p, &state);
    
    return res;
}

GFX_Result GFX_DrawPixelByDrawState(int32_t x, int32_t y, GFX_DrawState state)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point p;
    GFX_Result res;

    GFX_ASSERT(context != NULL);

    if(state.target == NULL)
    {
        if(context->layer.active == NULL ||
           context->layer.active->locked == GFX_FALSE)
            return GFX_FAILURE;

        state.target = &context->layer.active->buffers[context->layer.active->buffer_write_idx].pb;
        state.targetClipRect = context->layer.active->rect.local;
    }

    p.x = x;
    p.y = y;

    if(rejectPixel(&p, &state) == GFX_FAILURE)
        return GFX_FAILURE;

    res = state.pipeline->drawPixel[context->draw.antialias](&p, &state);

    return res;
}

GFX_Result cpuDrawPixel(const GFX_Point* pnt, const GFX_DrawState* state)
{
#if (GFX_ALPHA_BLENDING_ENABLED) || (GFX_ORIENTATION_ENABLED)
    GFX_Context* context = GFX_ActiveContext();
#endif
    GFX_Point tf_pnt = *pnt;
    GFX_Color result;
    
#if GFX_ALPHA_BLENDING_ENABLED
    GFX_Color dest;
#endif
    
#if GFX_ORIENTATION_ENABLED
    GFX_Rect targetRect = {0};
#endif  

#if GFX_ORIENTATION_ENABLED
    targetRect.width = state->target->size.width;
    targetRect.height = state->target->size.height;
    
    // reorient point to layer
    tf_pnt = context->hal.orientPoint(&tf_pnt, &targetRect, context->orientation);
    
    // mirror point to layer
    tf_pnt = context->hal.mirrorPoint(&tf_pnt, &targetRect, context->orientation);
#endif
    
    result = state->color;
    
#if GFX_ALPHA_BLENDING_ENABLED   
    if (state->alphaEnable == GFX_TRUE)
    {
        dest = context->hal.alphaGetPoint(state->target, &tf_pnt, state);

        dest = GFX_ColorConvert(state->target->mode, state->colorMode, dest);
        result = context->hal.alphaChannelBlend(result, dest, state->colorMode);

        result = context->hal.globalAlphaBlend(result, dest, state->colorMode);
    }
#endif

#if GFX_COLOR_CONVERSION_ENABLED   
    // convert color to destination buffer format
    result = GFX_ColorConvert(state->colorMode, state->target->mode, result);
#endif
    
    // write color
    return state->pipeline->pixelSet(state->target, &tf_pnt, result);
}

#endif // GFX_DRAW_PIPELINE_ENABLED