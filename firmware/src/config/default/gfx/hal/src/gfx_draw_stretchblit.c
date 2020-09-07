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


#include "gfx/hal/inc/gfx_draw_stretchblit.h"

#if GFX_DRAW_PIPELINE_ENABLED

#include "gfx/hal/inc/gfx_color.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_pixel_buffer.h"
#include "gfx/hal/inc/gfx_rect.h"

static GFX_Result rejectBlit(GFX_Rect* rect,
                             const GFX_DrawState* state)
{
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
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, rect) == GFX_FALSE)
        return GFX_FAILURE;
#endif

    return GFX_SUCCESS;
}

GFX_Result GFX_DrawStretchBlit(GFX_PixelBuffer* buffer,
                               int32_t src_x,
                               int32_t src_y,
                               int32_t src_width,
                               int32_t src_height,
                               int32_t dest_x,
                               int32_t dest_y,
                               int32_t dest_width,
                               int32_t dest_height)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Rect src_rect;
    GFX_Rect dest_rect;
    GFX_Result res;
    GFX_DrawState state;
    
    GFX_ASSERT(context != NULL && buffer != NULL);

    if(context == NULL)
        return GFX_FAILURE;

    if (dest_width == 0 || dest_height == 0)
        return GFX_FAILURE;
        
    state = context->draw;
        
    if(state.target == NULL)
    {
        if(context->layer.active == NULL ||
           context->layer.active->locked == GFX_FALSE)
            return GFX_FAILURE;
            
        state.target = &context->layer.active->buffers[context->layer.active->buffer_write_idx].pb;
        state.targetClipRect = context->layer.active->rect.local;
        state.colorMode = buffer->mode;
    }

    src_rect.x = src_x;
    src_rect.y = src_y;
    src_rect.width = src_width;
    src_rect.height = src_height;

    dest_rect.x = dest_x;
    dest_rect.y = dest_y;
    dest_rect.width = dest_width;
    dest_rect.height = dest_height;
    
    if(rejectBlit(&dest_rect, &state) == GFX_FAILURE)
        return GFX_FAILURE;
    
    // todo clip rectangles here

    res = state.pipeline->drawStretchBlit[state.resizeMode]
                                         (buffer,
                                         &src_rect,
                                         &dest_rect,
                                         &state);
    
    return res;
}

GFX_Result cpuDrawStretchBlit_NearestNeighbor(const GFX_PixelBuffer* source,
                                              const GFX_Rect* srcRect,
                                              const GFX_Rect* destRect,
                                              const GFX_DrawState* state)
{
#if GFX_COLOR_MASKING_ENABLED
    GFX_Context* context = GFX_ActiveContext();
#endif
    GFX_ColorMode sourceMode;
    int32_t row, col, index;
    GFX_Point pnt, refPnt, drawPoint;
    GFX_DrawState blitState = *state;
    GFX_Rect clipRect;
    GFX_Rect srcClipRect;
    GFX_Rect destClipRect = *destRect;
    
    int32_t x_ratio, y_ratio;
    
    // get source color mode
    sourceMode = source->mode;
       
    if(GFX_COLOR_MODE_IS_INDEX(source->mode) == GFX_TRUE)
        sourceMode = state->palette.mode;
        
    // clip buffer source rect against buffer size rect
    if(GFX_PixelBufferClipRect(source,
                               srcRect,
                               &srcClipRect) == GFX_FAILURE)
        return GFX_FAILURE;
    
#if GFX_LAYER_CLIPPING_ENABLED
    // clip image rect against target buffer rect
    if(GFX_PixelBufferClipRect(state->target,
                               &destClipRect,
                               &clipRect) == GFX_FAILURE)
        return GFX_FAILURE;
    
    pnt.x = destClipRect.x;
    pnt.y = destClipRect.y;
    
    destClipRect = clipRect;
#endif

#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip image rect against draw clipping rect
    if(state->clipEnable == GFX_TRUE)
    {
        GFX_RectClip(&destClipRect, &state->clipRect, &clipRect);
        
        pnt.x = destClipRect.x;
        pnt.y = destClipRect.y;
        
        destClipRect = clipRect;
    }
#endif
    
    
    // calculate stretching ratios
    x_ratio = GFX_Percent(srcClipRect.width, destClipRect.width);
    y_ratio = GFX_Percent(srcClipRect.height, destClipRect.height);
    
    for(row = 0; row < destClipRect.height; row++)
    {
        drawPoint.y = pnt.y + row;
    
        for(col = 0; col < destClipRect.width; col++)
        {
            drawPoint.x = pnt.x + col;
            
            // calculate nearest neighbor index
            refPnt.x = (GFX_PercentOf(col, x_ratio) + 50) / 100;
            
            if(refPnt.x >= srcClipRect.width)
                refPnt.x = srcClipRect.width - 1;
                
            refPnt.y = (GFX_PercentOf(row, y_ratio) + 50) / 100;
            
            if(refPnt.y >= srcClipRect.height)
                refPnt.y = srcClipRect.height - 1;
        
            // calculate source index
            index = (srcClipRect.x + refPnt.x) +
                    ((srcClipRect.y + refPnt.y) * source->size.width);

            // get color value/index
            blitState.color = GFX_PixelBufferGetIndex(source, index);
            
            // look up source color from buffer or palette
            blitState.color = GFX_ColorConvert(source->mode, sourceMode, blitState.color);
            
#if GFX_COLOR_MASKING_ENABLED
            // mask color
            if(context->hal.maskColor(&blitState, blitState.color) == GFX_TRUE)
                continue;
#endif  
            
            blitState.colorMode = source->mode;         

            // write color
            state->pipeline->drawPixel[state->antialias](&drawPoint, &blitState);
        }
    }
    
    return GFX_SUCCESS;
}

GFX_Result cpuDrawStretchBlit_Bilinear(const GFX_PixelBuffer* source,
                                       const GFX_Rect* srcRect,
                                       const GFX_Rect* destRect,
                                       const GFX_DrawState* state)
{
#if GFX_COLOR_MASKING_ENABLED
    GFX_Context* context = GFX_ActiveContext();
#endif
    GFX_ColorMode sourceMode;
    int32_t row, col;
    GFX_DrawState blitState = *state;
    GFX_Rect clipRect;
    GFX_Rect srcClipRect;
    GFX_Rect destClipRect = *destRect;
    GFX_Color c00, c01, c10, c11;
    GFX_Point bufferPoint;
    GFX_Point drawPoint;
    
    int32_t x_ratio, y_ratio;
    int32_t px, pxi, py, pyi;
    
    // get source color mode
    sourceMode = source->mode;
       
    if(GFX_COLOR_MODE_IS_INDEX(source->mode) == GFX_TRUE)
        sourceMode = state->palette.mode;
        
    // clip buffer source rect against buffer size rect
    if(GFX_PixelBufferClipRect(source,
                               srcRect,
                               &srcClipRect) == GFX_FAILURE)
        return GFX_FAILURE;
    
#if GFX_LAYER_CLIPPING_ENABLED
    // clip image rect against target buffer rect
    if(GFX_PixelBufferClipRect(state->target,
                               &destClipRect,
                               &clipRect) == GFX_FAILURE)
        return GFX_FAILURE;
    
    destClipRect = clipRect;
#endif

#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip image rect against draw clipping rect
    if(state->clipEnable == GFX_TRUE)
    {
        GFX_RectClip(&destClipRect, &state->clipRect, &clipRect);
        
        destClipRect = clipRect;
    }
#endif
    
    
    // calculate stretching ratios
    x_ratio = GFX_Percent(srcClipRect.width, destClipRect.width);
    y_ratio = GFX_Percent(srcClipRect.height, destClipRect.height);
    
    for(row = 0; row < destClipRect.height; row++)
    {
        drawPoint.y = destClipRect.y + row;
    
        for(col = 0; col < destClipRect.width; col++)
        {
            // calculate dest offset
            drawPoint.x = destClipRect.x + col;
            
            // calculate percentages and indices
            px = GFX_PercentOf(col, x_ratio);
            pxi = px / 100;
            
            if(pxi >= source->size.width)
                pxi = source->size.width - 1;
            
            py = GFX_PercentOf(row, y_ratio);
            pyi = py / 100;
            
            if(pyi >= source->size.height)
                pyi = source->size.height - 1;   
                
            // get top left color
            bufferPoint.x = pxi;
            bufferPoint.y = pyi;
                     
            c00 = GFX_PixelBufferGet(source, &bufferPoint);
            c00 = GFX_ColorConvert(source->mode, sourceMode, c00);
            
            // get top right color
            bufferPoint.x = pxi;
            bufferPoint.y = pyi;
            
            if(pxi < source->size.width - 1)
                bufferPoint.x++;
                
            c01 = GFX_PixelBufferGet(source, &bufferPoint);
            c01 = GFX_ColorConvert(source->mode, sourceMode, c01);
                
            // get bottom left color
            bufferPoint.x = pxi;
            bufferPoint.y = pyi;
            
            if(pyi < source->size.height - 1)
                bufferPoint.y++;
                
            c10 = GFX_PixelBufferGet(source, &bufferPoint);
            c10 = GFX_ColorConvert(source->mode, sourceMode, c10);
            
            // get bottom right color
            bufferPoint.x = pxi;
            bufferPoint.y = pyi;
            
            if(pxi < source->size.width - 1)
                bufferPoint.x++;
                
            if(pyi < source->size.height - 1)
                bufferPoint.y++;
                
            c11 = GFX_PixelBufferGet(source, &bufferPoint);
            c11 = GFX_ColorConvert(source->mode, sourceMode, c11);

            // perform bilinear interpoation
            blitState.color = GFX_ColorBilerp(c00, c01, c10, c11,
                                              px % 100,
                                              py % 100,
                                              sourceMode);
            
#if GFX_COLOR_MASKING_ENABLED
            // mask color
            if(context->hal.maskColor(&blitState, blitState.color) == GFX_TRUE)
                continue;
#endif  
            
            blitState.colorMode = sourceMode;

            // write color
            state->pipeline->drawPixel[state->antialias]
                                      (&drawPoint, &blitState);
        }
    }
    
    return GFX_SUCCESS;
}

#endif // GFX_DRAW_PIPELINE_ENABLED
