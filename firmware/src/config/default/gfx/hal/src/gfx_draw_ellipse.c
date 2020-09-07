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

#if GFX_DRAW_PIPELINE_ENABLED && GFX_DRAW_ELLIPSE_ENABLED

#include <math.h>
#include <stdbool.h>

#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_rect.h"
#include "gfx/hal/inc/gfx_util.h"
#include "gfx/hal/inc/gfx_math.h"

static GFX_Result rejectEllipse(GFX_Rect* rect,
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
    // clip rect against target rect
    if(GFX_RectIntersects(&state->targetClipRect, rect) == GFX_FALSE)
        return GFX_FAILURE;
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip rect against global clipping rect
    if(state->clipEnable == GFX_TRUE)
    {
        if(GFX_RectIntersects(&state->clipRect, rect) == GFX_FALSE)
            return GFX_FAILURE;
    }
#endif

    return GFX_SUCCESS;
}

GFX_Result GFX_DrawEllipse(int32_t x,
                       int32_t y,
                       int32_t a,
                       int32_t b,
                       int32_t theta,
                       int32_t startAngle,
                       int32_t centerAngle,
                       int32_t width,
                       int32_t height)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point pt;
    GFX_Result res = GFX_FAILURE;
    GFX_DrawState state;
    GFX_Rect rect;
   
    GFX_ASSERT(context != NULL);

    if(context == NULL)
        return res;
        
    state = context->draw;
        
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    
    if(state.target == NULL)
    {
        if(context->layer.active == NULL ||
           context->layer.active->locked == GFX_FALSE)
            return res;
            
        state.target = &context->layer.active->buffers[context->layer.active->buffer_write_idx].pb;
        state.targetClipRect = context->layer.active->rect.local;
    }
    
    state.colorMode = state.target->mode;
    
    pt.x = x;
    pt.y = y;
    
    if(rejectEllipse(&rect, &state) == GFX_FAILURE)
        return res;
    
    res = state.pipeline->drawEllipse[state.mode]
                                 [state.antialias]
                                 (&pt, a, b, theta, startAngle, centerAngle, &state);

    return res;
}

GFX_Result cpuDrawEllipse_Line(const GFX_Point* pt,
                           int32_t a,
                           int32_t b,
                           int32_t theta,
                           int32_t startAngle,
                           int32_t centerAngle,
                           const GFX_DrawState* state)
{
    int32_t sa = startAngle;
    GFX_Point p;
    static GFX_Point lp = {0,0};
    GFX_Bool layerClip = GFX_TRUE;
    GFX_Bool rectClip = GFX_TRUE;

    while (sa != (startAngle + centerAngle))
    {
        GFX_EllipsePoint(sa, a, b, theta, &p);
        
        p.x += pt->x;
        p.y += pt->y;
        
#if GFX_LAYER_CLIPPING_ENABLED
        if(GFX_RectContainsPoint(&state->targetClipRect, &p) == GFX_FALSE)
        {
            layerClip = GFX_FALSE;
        }
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
        // clip rect against global clipping rect
        if((state->clipEnable == GFX_TRUE) && 
            GFX_RectContainsPoint(&state->clipRect, &p) == GFX_FALSE)
        {
            rectClip = GFX_FALSE;
        }
#endif
        if (layerClip == GFX_TRUE && rectClip == GFX_TRUE)
        {
            state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&p, state);

            if (sa != startAngle)
            {
                GFX_DrawLine(lp.x,lp.y,p.x,p.y);
            }            
        }

        lp.x = p.x;
        lp.y = p.y;
        
        //Clockwise
        if (centerAngle < 0)
        {
            sa--;
        }
        //Counter-clockwise
        else
        {
            sa++;
        }
    }

    return GFX_SUCCESS;
}

#endif // GFX_DRAW_PIPELINE_ENABLED