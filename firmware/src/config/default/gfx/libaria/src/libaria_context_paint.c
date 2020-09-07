/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#include "gfx/libaria/inc/libaria_context.h"

#include "gfx/hal/gfx.h"

#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_screen.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

static void updateDirtyFlags(laWidget* widget)
{
    laWidget* child;
    GFX_Rect childRect;
    uint32_t i;

    _laWidget_ClearDirtyState(widget);
    
    for(i = 0; i < widget->children.size; i++)
    {
        child = widget->children.values[i];
        
        childRect = child->rect;
        childRect.x += widget->rect.x;
        childRect.y += widget->rect.y;

        // pre-cull based on visibility and rectangle intersection
        // anything not visible or completely outside parent can be ignored.
        if(child->visible == LA_TRUE &&
           child->dirtyState != LA_WIDGET_DIRTY_STATE_CLEAN &&
           GFX_RectIntersects(&widget->rect, &childRect) == LA_TRUE)
        {
            _laWidget_IncreaseDirtyState(widget, LA_WIDGET_DIRTY_STATE_CHILD);
        }
        else
        {            
            // clear all dirty flags for this child and its descendents
            _laWidget_ValidateChildren(child);
        }
    }
}

GFX_Result _laContext_PaintWidget(laWidget* widget)
{
    laWidget* child;
    GFX_Rect widgetRect, parentRect, clipRect, cacheRect;
    laBool alphaEnable;
    laBool shouldPaint = LA_FALSE;
    laBool painted = LA_FALSE;
    laLayer* layer;
    uint32_t i;
    
    layer = laUtils_GetLayer(widget);
    
    // fill widget cache if needed
    if(widget->backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        if(widget->cache == NULL)
        {
            // allocate struct
            widget->cache = laContext_GetActive()->memIntf.heap.malloc(sizeof(GFX_PixelBuffer));
            
            laContext_GetActive()->memIntf.heap.memset(widget->cache, 0, sizeof(GFX_PixelBuffer));
            
            widget->cacheInvalid = LA_TRUE;
        } 
        else if(layer->frameDrawCount > 0)
        {
            widget->cacheInvalid = LA_TRUE;
        }
    
        if(widget->cacheInvalid == GFX_TRUE)
        {
            cacheRect = laUtils_WidgetLocalRect((laWidget*)widget);
            
            laUtils_RectToLayerSpace(widget, &cacheRect);
            
            // fill the cache from the in-progress write buffer
            GFX_ReadPixelArray(GFX_BUFFER_WRITE,
                               &cacheRect,
                               widget->cache);
            
            widget->cacheInvalid = LA_FALSE;
        }
    }

    // skip any child that isn't dirty or that does not have a dirty descendent
    if(widget->dirtyState == LA_WIDGET_DIRTY_STATE_CLEAN)
        return LA_SUCCESS;
        
    // if widget is completely transparent just mark clean and return
    if(widget->alphaEnabled == GFX_TRUE && widget->alphaAmount == 0)
    {
        _laWidget_ValidateChildren(widget);
        
        return LA_SUCCESS;
    }
    
    if(widget->dirtyState == LA_WIDGET_DIRTY_STATE_DIRTY)
        shouldPaint = LA_TRUE;
    
    if((widget->optimizationFlags & LA_WIDGET_OPT_DRAW_ONCE) > 0 &&
       widget->drawCount >= laUtils_GetLayer(widget)->bufferCount)
        shouldPaint = LA_FALSE;
    
    // paint this widget and clear dirty flag
    if(shouldPaint == LA_TRUE)
    {
        // manage alpha blending
        alphaEnable = laWidget_GetCumulativeAlphaEnable(widget);
        
        if(alphaEnable == GFX_TRUE)
        {
            GFX_Set(GFXF_DRAW_BLEND_MODE, GFX_BLEND_GLOBAL);
            GFX_Set(GFXF_DRAW_ALPHA_VALUE, laWidget_GetCumulativeAlphaAmount(widget));
        }
        else
        {
            GFX_Set(GFXF_DRAW_BLEND_MODE, GFX_BLEND_NONE);
        }
        
        // clip the damage rectangle to the child's parent
        if(widget->parent != NULL)
        {
            widgetRect = laUtils_WidgetLayerRect(widget);
            parentRect = laUtils_WidgetLayerRect(widget->parent);
            
            // child does not intersect parent at all, do not draw
            if(GFX_RectIntersects(&widgetRect, &parentRect) == GFX_FALSE)
            {
                _laWidget_ValidateChildren(widget);
        
                return LA_SUCCESS;
            }
            
            // get the delta area between the parent and child
            GFX_RectClip(&widgetRect, &parentRect, &clipRect);
            
            // widget visible area does not intersect dirty area at all
            // do not draw
            if(GFX_RectIntersects(&clipRect,
                                  &layer->frameRectList.rects[layer->frameRectIdx]) == GFX_FALSE)
            {
                _laWidget_ValidateChildren(widget);
        
                return LA_SUCCESS;
            }
            
            // get the delta area between the dirty area and the child/parent
            // delta area
            GFX_RectClip(&layer->frameRectList.rects[layer->frameRectIdx],
                         &clipRect,
                         &layer->clippedDrawingRect);
        }
        else
        {
            //layer->dmgRectPtr = &layer->currentRect;
            layer->clippedDrawingRect = layer->frameRectList.rects[layer->frameRectIdx];
        }
        
        // turn off clipping by default
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_FALSE);
        
        widget->paint(widget);
        widget->drawCount++;
        
        painted = LA_TRUE;
        
        //printf("painting widget - %i, %i\n", widget->type, widget->id);
        
        if(widget->drawState != LA_WIDGET_DRAW_STATE_DONE)
            return LA_FAILURE;
            
        layer->frameDrawCount++;
    }
    
    _laWidget_ClearDirtyState(widget);
    
    // update widget dirty flags
    updateDirtyFlags(widget);
    
    // preempt if necessary
    if(laContext_GetActive()->preemptLevel >= LA_PREEMPTION_LEVEL_1 &&
       painted == LA_TRUE)
        return LA_FAILURE;
        
    if(widget->children.size == 0)
        return LA_SUCCESS;
    
    // draw children
    for(i = 0; i < widget->children.size; i++)
    {
        child = widget->children.values[i];

        if(child->dirtyState != LA_WIDGET_DIRTY_STATE_CLEAN)
        {
            if(_laContext_PaintWidget(child) == LA_FAILURE)
                return LA_FAILURE;
        }
    }
    
    // update widget dirty flags (should come out clean)
    updateDirtyFlags(widget);
    
    return LA_SUCCESS;
}

static laBool _paintLayer(laLayer* layer)
{
    // notify the draw layer that the library wants to start drawing to the active layer
    if(GFX_Begin() != GFX_SUCCESS)
        return LA_FAILURE;

    // paint root node
    _laContext_PaintWidget((laWidget*)layer);

    // notify the draw layer that drawing is over
    GFX_End();

    return layer->widget.dirtyState == LA_WIDGET_DIRTY_STATE_CLEAN;
}

static void invalidateLayers()
{
    GFX_Rect screenRect;
    laScreen* activeScreen = laContext_GetActive()->activeScreen;
    laBool needsDraw;
    int32_t i;
    
    screenRect = laContext_GetScreenRect();
    
    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        if(activeScreen->layers[i] == NULL)
            continue;
            
        needsDraw = _laLayer_NeedsDraw(activeScreen->layers[i], &screenRect);

        if(needsDraw == LA_TRUE)
        {
            GFX_Set(GFXF_LAYER_ACTIVE, i);
            GFX_Set(GFXF_LAYER_INVALID, LA_TRUE);
        }
    }
}

static laResult findNextLayer()
{
    GFX_Rect screenRect;
    laContext* context = laContext_GetActive();
    laScreen* activeScreen = laContext_GetActive()->activeScreen;
    laBool needsDraw;
    
    screenRect = laContext_GetScreenRect();
    
    while(context->currentLayer < LA_MAX_LAYERS)
    {
        needsDraw = _laLayer_NeedsDraw(activeScreen->layers[context->currentLayer], &screenRect);

        if(needsDraw == LA_TRUE)
            break;

        context->currentLayer++;
    }

    if(context->currentLayer == LA_MAX_LAYERS)
    {
        context->frameState = LA_CONTEXT_FRAME_READY;

        return LA_FAILURE;
    }
    
    return LA_SUCCESS;
}

void _laContext_Paint()
{   
    uint32_t i;
    
    laContext* context = laContext_GetActive();
    laScreen* activeScreen = laContext_GetActive()->activeScreen;

    switch(laContext_GetActive()->frameState)
    {
        case LA_CONTEXT_FRAME_PREFRAME:
        {
            context->currentLayer = 0;
            
            invalidateLayers();
            
            if(findNextLayer() == LA_FAILURE)
            {
                context->frameState = LA_CONTEXT_FRAME_READY;
                
                return;
            }
            
            context->frameState = LA_CONTEXT_FRAME_PRELAYER;
        }
        case LA_CONTEXT_FRAME_PRELAYER:
        {
            _laLayer_Preframe(activeScreen->layers[context->currentLayer]);
            
            context->frameState = LA_CONTEXT_FRAME_DRAWING;
        }
        case LA_CONTEXT_FRAME_DRAWING:
        {
            GFX_Set(GFXF_LAYER_ACTIVE, context->currentLayer);
            GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_FALSE);
            
            if(_paintLayer(activeScreen->layers[context->currentLayer]) == LA_FALSE)
                return;
                
            _laLayer_Postframe(activeScreen->layers[context->currentLayer]);
            
            if(activeScreen->layers[context->currentLayer]->frameState != LA_LAYER_FRAME_COMPLETE)
                break;
            
            context->frameState = LA_CONTEXT_FRAME_POSTLAYER;
        }
        case LA_CONTEXT_FRAME_POSTLAYER:
        {
            GFX_Set(GFXF_LAYER_SWAP, GFX_TRUE);
            
            _laLayer_FinishFrame(activeScreen->layers[context->currentLayer]);
            
            // advance layer index
            context->currentLayer++;
            
            findNextLayer();
            
            // check to see if a new frame needs to be drawn immediately
            if(context->currentLayer == LA_MAX_LAYERS)
            {
                context->frameState = LA_CONTEXT_FRAME_READY;
                
                for(i = 0; i < LA_MAX_LAYERS; i++)
                {
                    if(activeScreen->layers[i] != NULL &&
                       activeScreen->layers[i]->frameState != LA_LAYER_FRAME_READY)
                    {
                        context->frameState = LA_CONTEXT_FRAME_PREFRAME;
                        
                        break;
                    }
                }
            }
            else
            {
                context->frameState = LA_CONTEXT_FRAME_PRELAYER;
                
                _laContext_Paint();
            }
            
            break;
        }
        default:
        {
            break;
        }
    }

    
}