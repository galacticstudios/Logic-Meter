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

#include "gfx/hal/inc/gfx_color.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_pixel_buffer.h"
#include "gfx/hal/inc/gfx_rect.h"

static GFX_Result rejectBlit(const GFX_Rect* rect,
                             const GFX_DrawState* state)
{
#if GFX_ALPHA_BLENDING_ENABLED
    // alpha is zero
    if((state->blendMode & GFX_BLEND_GLOBAL) > 0 &&
        state->globalAlphaValue == 0)
       return GFX_FAILURE;
#endif
    
#if GFX_LAYER_CLIPPING_ENABLED
    // clip rect against layer rect
    if(GFX_RectIntersects(&state->targetClipRect, rect) == GFX_FALSE)
        return GFX_FAILURE;
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip rect against global clipping rect
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, rect) == GFX_FALSE)
        return GFX_FAILURE;
#endif

    return GFX_SUCCESS;
}

GFX_Result GFX_DrawBlit(GFX_PixelBuffer* buffer,
                        int32_t src_x,
                        int32_t src_y,
                        int32_t src_width,
                        int32_t src_height,
                        int32_t dest_x,
                        int32_t dest_y)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Rect src_rect, dest_rect, clip_rect;
    GFX_Point pnt;
    GFX_Result res;
    GFX_DrawState state;

    GFX_ASSERT(context != NULL && buffer != NULL);

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

    src_rect.x = src_x;
    src_rect.y = src_y;
    src_rect.width = src_width;
    src_rect.height = src_height;
    
    // clip buffer source rect against buffer size rect
    if(GFX_PixelBufferClipRect(buffer,
                               &src_rect,
                               &clip_rect) == GFX_FAILURE)
        return GFX_FAILURE;
    
    src_rect = clip_rect;
    
    dest_rect.x = dest_x;
    dest_rect.y = dest_y;
    dest_rect.width = src_rect.width;
    dest_rect.height = src_rect.height;
    
    if(rejectBlit(&dest_rect, &state) == GFX_FAILURE)
        return GFX_FAILURE;
    
#if GFX_LAYER_CLIPPING_ENABLED
    // clip image rect against target buffer rect   
    clip_rect = GFX_RectClipAdj(&dest_rect, &state.targetClipRect, &src_rect);
    
    dest_rect = clip_rect;
#endif

#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip image rect against draw clipping rect
    if(state.clipEnable == GFX_TRUE)
        clip_rect = GFX_RectClipAdj(&dest_rect, &state.clipRect, &src_rect);
        
    dest_rect = clip_rect;
#endif
    
    pnt.x = dest_rect.x;
    pnt.y = dest_rect.y;

    res = state.pipeline->drawBlit(buffer, 
                                   &src_rect, 
                                   &pnt,
                                   &state);
    
    return res;
}

GFX_Result cpuDrawBlit(const GFX_PixelBuffer* source,
                       const GFX_Rect* srcRect,
                       const GFX_Point* drawPoint,
                       const GFX_DrawState* state)
{
#if GFX_COLOR_MASKING_ENABLED
    GFX_Context* context = GFX_ActiveContext();
#endif
    GFX_ColorMode sourceMode;
    int32_t row, col, index;
    GFX_Point pnt;
    GFX_DrawState blitState = *state;
    
    // get source color mode
    sourceMode = source->mode;
       
    if(GFX_COLOR_MODE_IS_INDEX(source->mode) == GFX_TRUE)
        sourceMode = state->palette.mode;
        
    for(row = 0; row < srcRect->height; row++)
    {
        pnt.y = drawPoint->y + row;
    
        for(col = 0; col < srcRect->width; col++)
        {
            // calculate dest offset
            pnt.x = drawPoint->x + col;
        
            // calculate source index
            index = (srcRect->x + col) +
                    ((srcRect->y + row) * source->size.width);

            // get color value/index
            blitState.color = GFX_PixelBufferGetIndex(source, index);
            
            // look up source color from buffer or palette
            blitState.color = GFX_ColorConvert(source->mode, sourceMode, blitState.color);
            
#if GFX_COLOR_MASKING_ENABLED
            // mask color
            if(context->hal.maskColor(&blitState, blitState.color) == GFX_TRUE)
                continue;
#endif  
            
            blitState.colorMode = sourceMode;         

            // write color
            state->pipeline->drawPixel[state->antialias](&pnt, &blitState);
        }
    }
    
    return GFX_SUCCESS;
}

GFX_Result GFX_DrawDirectBlit(GFX_PixelBuffer* buffer,
                              int32_t src_x,
                              int32_t src_y,
                              int32_t src_width,
                              int32_t src_height,
                              int32_t dest_x,
                              int32_t dest_y)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Rect src_rect, dest_rect, clip_rect;
    GFX_Point pnt;
    GFX_Result res;
    GFX_DrawState state;

    GFX_ASSERT(context != NULL && buffer != NULL);

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
    
    // color format must match
    if(GFX_COLOR_MODE_IS_INDEX(buffer->mode) == GFX_TRUE ||
       state.target->mode != buffer->mode)
        return GFX_FAILURE;

    src_rect.x = src_x;
    src_rect.y = src_y;
    src_rect.width = src_width;
    src_rect.height = src_height;
    
    // clip buffer source rect against buffer size rect
    if(GFX_PixelBufferClipRect(buffer,
                               &src_rect,
                               &clip_rect) == GFX_FAILURE)
        return GFX_FAILURE;
    
    src_rect = clip_rect;
    
    dest_rect.x = dest_x;
    dest_rect.y = dest_y;
    dest_rect.width = src_rect.width;
    dest_rect.height = src_rect.height;
    
    if(rejectBlit(&dest_rect, &state) == GFX_FAILURE)
        return GFX_FAILURE;
    
#if GFX_LAYER_CLIPPING_ENABLED
    // clip image rect against target buffer rect   
    clip_rect = GFX_RectClipAdj(&dest_rect, &state.targetClipRect, &src_rect);
    
    dest_rect = clip_rect;
#endif

#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip image rect against draw clipping rect
    if(state.clipEnable == GFX_TRUE)
        clip_rect = GFX_RectClipAdj(&dest_rect, &state.clipRect, &src_rect);
        
    dest_rect = clip_rect;
#endif
    
    pnt.x = dest_rect.x;
    pnt.y = dest_rect.y;

    res = state.pipeline->drawDirectBlit(buffer, 
                                         &src_rect, 
                                         &pnt,
                                         &state);
    
    return res;
}

GFX_Result cpuDrawDirectBlit(const GFX_PixelBuffer* source,
                             const GFX_Rect* srcRect,
                             const GFX_Point* drawPoint,
                             const GFX_DrawState* state)
{
    GFX_Context* context = GFX_ActiveContext();
    int32_t row;
    uint32_t size;
    GFX_Point srcPnt, destPnt;
    GFX_Buffer *src, *dest;
    
    srcPnt.x = srcRect->x;
    srcPnt.y = srcRect->y;
        
    destPnt.x = drawPoint->x;
    destPnt.y = drawPoint->y;
    
    // see if a bulk memcpy can be used
    // won't work if there are gaps at the start or end of a row
    if(srcRect->x == 0 && srcRect->width == state->target->size.width)
    {
        src = GFX_PixelBufferOffsetGet(source, &srcPnt);
        dest = GFX_PixelBufferOffsetGet(state->target, &destPnt); 
        size = (srcRect->width * srcRect->height) * GFX_ColorInfo[source->mode].size;
        
        context->memory.memcpy(dest, src, size);
        
        return GFX_SUCCESS;
    }
    
    // size for one row at a time
    size = srcRect->width * GFX_ColorInfo[source->mode].size;
    
    // can't do bulk copy, go row by row instead        
    for(row = 0; row < srcRect->height; row++)
    {
        src = GFX_PixelBufferOffsetGet(source, &srcPnt);
        dest = GFX_PixelBufferOffsetGet(state->target, &destPnt);
    
        context->memory.memcpy(dest, src, size);
        
        srcPnt.y++;
        destPnt.y++;
    }
    
    return GFX_SUCCESS;
}

#endif // GFX_DRAW_PIPELINE_ENABLED
