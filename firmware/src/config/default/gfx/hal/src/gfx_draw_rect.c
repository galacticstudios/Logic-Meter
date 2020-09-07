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

#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_draw_line.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_math.h"

static GFX_Result rejectRect(GFX_Rect* rect,
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

GFX_Result GFX_DrawRect(int32_t x,
                        int32_t y,
                        int32_t width,
                        int32_t height)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Rect rect;
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
    
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    
    if(rejectRect(&rect, &state) == GFX_FAILURE)
        return GFX_FAILURE;

    res = state.pipeline->drawRect[state.mode]
                                  [state.antialias]
                                  (&rect, &state);
                               
    return res;
}

GFX_Result cpuDrawRect_Line(const GFX_Rect* rect, const GFX_DrawState* state)
{
    GFX_Point pnt1, pnt2;
        
    // top line
    pnt1.x = rect->x;
    pnt1.y = rect->y;
    pnt2.x = rect->x + rect->width - 1;
    pnt2.y = rect->y;
    state->pipeline->drawLine[GFX_DRAW_LINE]
                             [state->antialias]
                             (&pnt1, &pnt2, state);
    // bottom line
    pnt1.y = rect->y + rect->height - 1;
    pnt2.y = pnt1.y;                            
    state->pipeline->drawLine[GFX_DRAW_LINE]
                             [state->antialias]
                             (&pnt1, &pnt2, state);
                
    // left line
    pnt1.x = rect->x;
    pnt1.y = rect->y;
    pnt2.x = rect->x;
    state->pipeline->drawLine[GFX_DRAW_LINE]
                             [state->antialias]
                             (&pnt1, &pnt2, state);
    
    // right line
    pnt1.x = rect->x + rect->width - 1;
    pnt2.x = pnt1.x;                            
    state->pipeline->drawLine[GFX_DRAW_LINE]
                             [state->antialias]
                             (&pnt1, &pnt2, state);
    
    return GFX_SUCCESS;
}

GFX_Result cpuDrawRect_Fill(const GFX_Rect* rect, const GFX_DrawState* state)
{
    int32_t row, row_max;
    GFX_Point pnt1, pnt2;
    
    GFX_Rect lrect;
#if GFX_LAYER_CLIPPING_ENABLED || GFX_BOUNDS_CLIPPING_ENABLED
    GFX_Rect clipRect;
#endif

    lrect = *rect;
    
#if GFX_LAYER_CLIPPING_ENABLED
    // clip rect against layer rect
    GFX_RectClip(&state->targetClipRect,
                 &lrect,
                 &clipRect);
                 
    lrect = clipRect;        
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, &lrect) == GFX_FALSE)
        return GFX_FAILURE;
        
    // clip rect against global clipping rect
    if(state->clipEnable == GFX_TRUE)
    {
        GFX_RectClip(&state->clipRect,
                     &lrect,
                     &clipRect);
                     
        lrect = clipRect; 
    }
#endif
    
    // calculate minimums
    row_max = lrect.height;
    
    pnt1.x = lrect.x;
    pnt2.x = lrect.x + lrect.width - 1;
    
    for(row = 0; row < row_max; row++)
    {
        pnt1.y = lrect.y + row;
        pnt2.y = lrect.y + row;

        cpuDrawLine_Horz(&pnt1, &pnt2, state);
    }
    
    return GFX_SUCCESS;
}

GFX_Result cpuDrawRect_Gradient_LeftRight(const GFX_Rect* rect,
                                          const GFX_DrawState* state)
{
    GFX_Point pnt1, pnt2;
    int32_t val;
    uint32_t percent;
    GFX_DrawState gradState;
    
    GFX_Rect lrect;
#if GFX_LAYER_CLIPPING_ENABLED || GFX_BOUNDS_CLIPPING_ENABLED
    GFX_Rect clipRect;
#endif

    lrect = *rect;
    
#if GFX_LAYER_CLIPPING_ENABLED
    // clip rect against layer rect
    GFX_RectClip(&state->targetClipRect,
                 &lrect,
                 &clipRect);
                 
    lrect = clipRect;        
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, &lrect) == GFX_FALSE)
        return GFX_FAILURE;
        
    // clip rect against global clipping rect
    if(state->clipEnable == GFX_TRUE)
    {
        GFX_RectClip(&state->clipRect,
                     &lrect,
                     &clipRect);
                     
        lrect = clipRect; 
    }
#endif
    
    // calculate minimums
    pnt1.y = lrect.y;
    pnt2.y = lrect.y + lrect.height - 1;   
    
    gradState = *state;
    
    for(pnt1.x = lrect.x; pnt1.x < lrect.x + lrect.width; pnt1.x++)
    {
        val = pnt1.x - rect->x;
        
        percent = GFX_PercentWholeRounded(val, rect->width);
        
        gradState.color = GFX_ColorLerp(gradState.gradient.c0,
                                        gradState.gradient.c1,
                                        percent,
                                        state->colorMode);
                                 
        state->pipeline->drawVertLine[GFX_DRAW_LINE]
                                     [state->antialias]
                                     (&pnt1, &pnt2, &gradState);
    }
    
    return GFX_SUCCESS;
}

GFX_Result cpuDrawRect_Gradient_TopBottom(const GFX_Rect* rect,
                                          const GFX_DrawState* state)
{
    GFX_Point pnt1, pnt2;
    int32_t val;
    uint32_t percent;
    GFX_DrawState gradState;
    
    GFX_Rect lrect;
#if GFX_LAYER_CLIPPING_ENABLED || GFX_BOUNDS_CLIPPING_ENABLED
    GFX_Rect clipRect;
#endif

    lrect = *rect;
    
#if GFX_LAYER_CLIPPING_ENABLED
    // clip rect against layer rect
    GFX_RectClip(&state->targetClipRect,
                 &lrect,
                 &clipRect);
                 
    lrect = clipRect;        
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, &lrect) == GFX_FALSE)
        return GFX_FAILURE;
        
    // clip rect against global clipping rect
    if(state->clipEnable == GFX_TRUE)
    {
        GFX_RectClip(&state->clipRect,
                     &lrect,
                     &clipRect);
                     
        lrect = clipRect; 
    }
#endif
    
    // calculate minimums
    pnt1.x = lrect.x;
    pnt2.x = lrect.x + lrect.width - 1;   
    
    gradState = *state;
    
    for(pnt1.y = lrect.y; pnt1.y < lrect.y + lrect.height; pnt1.y++)
    {
        val = pnt1.y - rect->y;
        
        percent = GFX_PercentWholeRounded(val, rect->height);
        
        gradState.color = GFX_ColorLerp(gradState.gradient.c0,
                                        gradState.gradient.c1,
                                        percent,
                                        state->colorMode);
                                 
        state->pipeline->drawHorzLine[GFX_DRAW_LINE]
                                     [state->antialias]
                                     (&pnt1, &pnt2, &gradState);
    }
    
    return GFX_SUCCESS;
}
                          
GFX_Result cpuDrawRect_Gradient_Quad(const GFX_Rect* rect, 
                                     const GFX_DrawState* state)
{
    return GFX_UNSUPPORTED;
}

GFX_Result cpuDrawRect_Line_AA(const GFX_Rect* rect, 
                               const GFX_DrawState* state)
{   
    return GFX_UNSUPPORTED;
}

GFX_Result cpuDrawRect_Fill_AA(const GFX_Rect* rect, 
                               const GFX_DrawState* state)
{
    return GFX_UNSUPPORTED;
}

GFX_Result cpuDrawRect_Gradient_Bi_AA(const GFX_Rect* rect, 
                                      const GFX_DrawState* state)
{
    return GFX_UNSUPPORTED;
}
                          
GFX_Result cpuDrawRect_Gradient_Quad_AA(const GFX_Rect* rect,
                                        const GFX_DrawState* state)
{
    return GFX_UNSUPPORTED;
}

#endif // GFX_DRAW_PIPELINE_ENABLED