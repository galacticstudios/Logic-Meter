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


#include "gfx/libaria/inc/libaria_layer.h"

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_screen.h"
#include "gfx/libaria/inc/libaria_utils.h"

#include "gfx/hal/inc/gfx_rect.h"

#define DEFAULT_WIDTH   100
#define DEFAULT_HEIGHT  100

laLayer* laLayer_New()
{
    laLayer* lyr = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    lyr = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laLayer));
    
    _laWidget_Constructor((laWidget*)lyr);

    lyr->widget.type = LA_WIDGET_LAYER;
    lyr->widget.root = LA_TRUE;
    
    lyr->bufferCount = 1;

    lyr->widget.rect.x = 0;
    lyr->widget.rect.y = 0;
    lyr->widget.rect.width = DEFAULT_WIDTH;
    lyr->widget.rect.height = DEFAULT_HEIGHT;
    
    lyr->inputRect.x = 0;
    lyr->inputRect.y = 0;
    lyr->inputRect.width = DEFAULT_WIDTH;
    lyr->inputRect.height = DEFAULT_HEIGHT;
    
    lyr->alphaEnable = LA_FALSE;

    lyr->widget.moved = (laWidget_Moved_FnPtr)&_laLayer_Moved;
    lyr->widget.resized = (laWidget_Resized_FnPtr)&_laLayer_Resized;
    lyr->widget.paint = (laWidget_Paint_FnPtr)&_laLayer_Paint;
    
    laRectArray_Create(&lyr->prevDamageRects);
    laRectArray_Create(&lyr->currentDamageRects);
    laRectArray_Create(&lyr->pendingDamageRects);
    laRectArray_Create(&lyr->frameRectList);
    laRectArray_Create(&lyr->scratchRectList);
    
    lyr->vsync = LA_TRUE;
    
    lyr->inputRect = lyr->widget.rect;
    lyr->inputRectLocked = LA_TRUE;
    lyr->allowInputPassThrough = LA_TRUE;
    
    lyr->frameState = LA_LAYER_FRAME_READY;

    laLayer_AddDamageRect(lyr, &lyr->widget.rect, LA_FALSE);

    return lyr;
}

void laLayer_Delete(laLayer* layer)
{
    if(layer == NULL)
        return;

    laRectArray_Destroy(&layer->prevDamageRects);
    laRectArray_Destroy(&layer->currentDamageRects);
    laRectArray_Destroy(&layer->pendingDamageRects);
    laRectArray_Destroy(&layer->frameRectList);
    laRectArray_Destroy(&layer->scratchRectList);
    
    layer->deleting = LA_TRUE;
    
    layer->widget.destructor((laWidget*)layer);    
}

laBool laLayer_GetEnabled(const laLayer* layer)
{
    if(layer == NULL)
        return LA_FAILURE;
        
    return layer->widget.enabled;
}

laResult laLayer_SetEnabled(laLayer* layer, laBool enable)
{
    if(layer == NULL)
        return LA_FAILURE;
    
    layer->widget.enabled = enable;
    
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return GFX_SUCCESS;
    
    GFX_Set(GFXF_LAYER_ACTIVE, laScreen_GetLayerIndex(layer->screen, layer));
    GFX_Set(GFXF_LAYER_ENABLED, layer->widget.enabled);
    
    return GFX_SUCCESS;
}

laBool laLayer_GetAlphaEnable(const laLayer* layer)
{
    if(layer == NULL)
        return LA_FAILURE;
        
    return layer->alphaEnable;
}

laResult laLayer_SetAlphaEnable(laLayer* layer, laBool enable)
{
    if(layer == NULL)
        return LA_FAILURE;
    
    layer->alphaEnable = enable;
    
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return GFX_SUCCESS;
    
    GFX_Set(GFXF_LAYER_ACTIVE, laScreen_GetLayerIndex(layer->screen, layer));
    GFX_Set(GFXF_LAYER_ALPHA_ENABLE, layer->alphaEnable, GFX_FALSE);
    
    return GFX_SUCCESS;
}

laResult laLayer_SetAlphaAmount(laLayer* layer, uint32_t amount)
{
    if(laWidget_SetAlphaAmount((laWidget*)layer, amount) == GFX_FAILURE)
        return GFX_FAILURE;
        
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return GFX_SUCCESS;
    
    GFX_Set(GFXF_LAYER_ACTIVE, laScreen_GetLayerIndex(layer->screen, layer));
    GFX_Set(GFXF_LAYER_ALPHA_AMOUNT, layer->widget.alphaAmount, GFX_FALSE);
    
    return GFX_SUCCESS;
}

laBool laLayer_GetMaskEnable(const laLayer* layer)
{
    if(layer == NULL)
        return 0;
        
    return layer->maskEnable;
}

laResult laLayer_SetMaskEnable(laLayer* layer, laBool enable)
{
    if(layer == NULL || layer->maskEnable == enable)
        return LA_FAILURE;
        
    layer->maskEnable = enable;
        
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return LA_SUCCESS;
    
    GFX_Set(GFXF_LAYER_ACTIVE, laScreen_GetLayerIndex(layer->screen, layer));
    GFX_Set(GFXF_LAYER_MASK_ENABLE, layer->maskEnable, GFX_FALSE);

    return LA_SUCCESS;
}

GFX_Color laLayer_GetMaskColor(const laLayer* layer)
{
    if(layer == NULL)
        return 0;
        
    return layer->maskColor;
}

laResult laLayer_SetMaskColor(laLayer* layer, GFX_Color color)
{
    if(layer == NULL || layer->maskColor == color)
        return LA_FAILURE;
        
    layer->maskColor = color;
        
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return LA_SUCCESS;
    
    GFX_Set(GFXF_LAYER_ACTIVE, laScreen_GetLayerIndex(layer->screen, layer));
    GFX_Set(GFXF_LAYER_MASK_COLOR, layer->maskColor, GFX_FALSE);
    
    return LA_SUCCESS;
}

uint32_t laLayer_GetBufferCount(const laLayer* layer)
{
    if(layer == NULL)
        return 0;

    return layer->bufferCount;
}

laResult laLayer_SetBufferCount(laLayer* layer, uint32_t count)
{
    if(layer == NULL)
        return LA_FAILURE;

    layer->bufferCount = count;
    
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return LA_SUCCESS;
    
    GFX_Set(GFXF_LAYER_BUFFER_COUNT, count);
    
    return LA_SUCCESS;
}

laBool laLayer_GetVSync(const laLayer* layer)
{
    if(layer == NULL)
        return 0;

    return layer->vsync;
}

laResult laLayer_SetVSync(laLayer* layer, laBool enable)
{
    if(layer == NULL)
        return LA_FAILURE;
    
    layer->vsync = enable;
    
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return LA_SUCCESS;
    
    GFX_Set(GFXF_LAYER_VSYNC, enable);
    
    return LA_SUCCESS;
}

GFX_Rect laLayer_GetInputRect(const laLayer* layer)
{
    if(layer == NULL)
        return GFX_Rect_Zero;
        
    return layer->inputRect;
}

laResult laLayer_SetInputRect(laLayer* layer,
                              int32_t x,
                              int32_t y,
                              int32_t width,
                              int32_t height)
{
    if(layer == NULL || layer->inputRectLocked == LA_TRUE)
        return LA_FAILURE;
        
    layer->inputRect.x = x;
    layer->inputRect.y = y;
    layer->inputRect.width = width;
    layer->inputRect.height = height;
        
    return LA_SUCCESS;
}

laBool laLayer_GetInputRectLocked(const laLayer* layer)
{
    if(layer == NULL)
        return LA_FAILURE;
        
    return layer->inputRectLocked;
}

laResult laLayer_SetInputRectLocked(laLayer* layer, laBool locked)
{
    if(layer == NULL)
        return LA_FAILURE;
        
    layer->inputRectLocked = locked;
    
    if(locked == LA_TRUE)
        layer->inputRect = layer->widget.rect;
        
    return LA_SUCCESS;
}

laBool laLayer_GetAllowInputPassThrough(const laLayer* layer)
{
    if(layer == NULL)
        return 0;

    return layer->allowInputPassThrough;
}

laResult laLayer_SetAllowInputPassthrough(laLayer* layer, laBool enable)
{
    if(layer == NULL)
        return LA_FAILURE;
    
    layer->allowInputPassThrough = enable;
    
    return LA_SUCCESS;
}

laResult laLayer_AddDamageRect(laLayer* layer,
                               const GFX_Rect* rect,
							   laBool noCombine)
{
    GFX_Rect clipRect;

    if(layer == NULL || layer->deleting == LA_TRUE)
        return LA_FAILURE;

    // make sure rect is inside the layer
    GFX_RectClip(&layer->widget.rect, rect, &clipRect);

    if(laContext_GetActive()->frameState == LA_CONTEXT_FRAME_READY)
        laContext_GetActive()->frameState = LA_CONTEXT_FRAME_PREFRAME;

    if(layer->frameState == LA_LAYER_FRAME_READY)
    {        
        //printf("toggling preframe\n");
        layer->frameState = LA_LAYER_FRAME_PREFRAME;
        
        // get ready for a new frame
        _laWidget_IncreaseDirtyState((laWidget*)layer, LA_WIDGET_DIRTY_STATE_DIRTY);
    }

    if(layer->frameState < LA_LAYER_FRAME_IN_PROGRESS)
        _laLayer_AddDamageRectToList(&layer->currentDamageRects, &clipRect, noCombine);
    else
    {
        //printf("added pending rect\n");
        _laLayer_AddDamageRectToList(&layer->pendingDamageRects, &clipRect, noCombine);
    }
    
    return LA_SUCCESS;
}

laBool laLayer_IsDrawing(laLayer* layer)
{
    if(layer == NULL)
        return LA_FALSE;
        
    return layer->frameState >= LA_LAYER_FRAME_IN_PROGRESS;
}

void _laLayer_Reset(laLayer* layer)
{
    laRectArray_Clear(&layer->prevDamageRects);
    laRectArray_Clear(&layer->currentDamageRects);
    laRectArray_Clear(&layer->pendingDamageRects);
    laRectArray_Clear(&layer->frameRectList);
    
    layer->frameState = LA_LAYER_FRAME_READY;
    layer->frameDrawCount = 0;
    layer->deltaTime = 0;
    
    layer->drawingPrev = LA_FALSE;
}

//int32_t dump = 0;

laBool _laLayer_NeedsDraw(laLayer* layer, GFX_Rect* rect)
{
    if(layer == NULL)
        return LA_FALSE;
        
    return (layer->widget.enabled == LA_FALSE ||
            GFX_RectIntersects(rect, &layer->widget.rect) == LA_FALSE ||
            layer->widget.dirtyState == LA_WIDGET_DIRTY_STATE_CLEAN) == LA_FALSE;
}

static void addRectToFrameList(laLayer* lyr, GFX_Rect* rect)
{
    uint32_t i, j, cnt;
    GFX_Rect rects[4];
    
    if(lyr->frameRectList.size > 0)
    {
        for(i = 0; i < lyr->frameRectList.size; i++)
        {
            // area is already covered by an existing rect
            if(GFX_RectContainsRect(&lyr->frameRectList.rects[i], rect) == GFX_TRUE)
            {
                return;
            }
            // two rectangles are touching, split the incoming rectangle and
            // add pieces to list
            else if(GFX_RectIntersects(rect, &lyr->frameRectList.rects[i]) == GFX_TRUE)
            {
                cnt = GFX_RectSplit(rect, &lyr->frameRectList.rects[i], rects);

                for(j = 0; j < cnt; j++)
                    laRectArray_PushBack(&lyr->scratchRectList, &rects[j]);
                
                return;
            }
        }
    }
    
    laRectArray_PushBack(&lyr->frameRectList, rect);
}

void _laLayer_Preframe(laLayer* layer)
{
    uint32_t i;
    GFX_Rect rect;
    
    layer->frameState = LA_LAYER_FRAME_IN_PROGRESS;
    
    laRectArray_Clear(&layer->scratchRectList);
    laRectArray_Clear(&layer->frameRectList);
    
    //printf("dump: %i\n", dump++);
    
    // merge rectangle lists
    if(layer->bufferCount > 1)
    {
        for(i = 0; i < layer->prevDamageRects.size; i++)
            laRectArray_PushBack(&layer->scratchRectList, &layer->prevDamageRects.rects[i]);
    }
    
    for(i = 0; i < layer->currentDamageRects.size; i++)
        laRectArray_PushBack(&layer->scratchRectList, &layer->currentDamageRects.rects[i]);
    
    // remove duplicate rectangles from combined list
    laRectArray_RemoveDuplicates(&layer->scratchRectList);
    
    // combine any adjacent rectangles
    laRectArray_MergeSimilar(&layer->scratchRectList);
    
    // remove overlapping space
    laRectArray_RemoveOverlapping(&layer->scratchRectList);
    
    while(layer->scratchRectList.size != 0)
    {
        rect = layer->scratchRectList.rects[0];
        
        laRectArray_RemoveAt(&layer->scratchRectList, 0);
        
        addRectToFrameList(layer, &rect);
    }
    
    layer->frameRectIdx = 0;
    layer->frameDrawCount = 0;
    
    _laLayer_InvalidateChildren(layer);
    
    
    
    /*GFX_DisableDebugRects();
    
    GFX_SetDebugRect(layer->currentDrawingRect.x,
                     layer->currentDrawingRect.y,
                     layer->currentDrawingRect.width,
                     layer->currentDrawingRect.height);
                     
    GFX_SetDebugRectColor(0x0000FF);
    GFX_ShowDebugRect(GFX_TRUE);
    GFX_NextDebugRect();*/
}

void _laLayer_Postframe(laLayer* layer)
{
    layer->frameDrawCount = 0;
    
    layer->frameRectIdx++;
    
    // done drawing all rects?
    if(layer->frameRectIdx >= layer->frameRectList.size)
    {
        // move current rects to previous list
        //if(layer->bufferCount > 1)
        //    laRectArray_Copy(&layer->currentDamageRects, &layer->prevDamageRects);
        
        layer->clippedDrawingRect = GFX_Rect_Zero;
        
        layer->frameState = LA_LAYER_FRAME_COMPLETE;
    }
    else
    {
        _laLayer_InvalidateChildren(layer);
    }
    
    /*GFX_SetDebugRect(layer->currentDrawingRect.x,
                     layer->currentDrawingRect.y,
                     layer->currentDrawingRect.width,
                     layer->currentDrawingRect.height);
                     
    GFX_SetDebugRectColor(0x0000FF);
    GFX_ShowDebugRect(GFX_TRUE);
    GFX_NextDebugRect();*/
}

void _laLayer_FinishFrame(laLayer* layer)
{
    if(layer->frameRectIdx >= layer->frameRectList.size)
    {
        // move current rects to previous list
        if(layer->bufferCount > 1)
            laRectArray_Copy(&layer->currentDamageRects, &layer->prevDamageRects);
        
        // need to automatically fill the back buffer the first time this layer
        // draws
        if(layer->layerDrawCount == 0 && layer->bufferCount > 1)
            laRectArray_PushBack(&layer->pendingDamageRects, &layer->widget.rect);
        
        //After the back buffer has been tagged for fill, clear the prevDamaged 
        //rects. This avoids a full redraw of the frame later
        if (layer->layerDrawCount == 1 &&  layer->bufferCount > 1)
            laRectArray_Clear(&layer->prevDamageRects);
        
        layer->layerDrawCount++;
        
        // manage the layer's rectangle arrays            
        if(layer->pendingDamageRects.size > 0)
        {
            laRectArray_Copy(&layer->pendingDamageRects, 
                             &layer->currentDamageRects);

            laRectArray_Clear(&layer->pendingDamageRects);

            _laWidget_IncreaseDirtyState((laWidget*)layer,
                                         LA_WIDGET_DIRTY_STATE_DIRTY);

            layer->frameState = LA_LAYER_FRAME_PREFRAME;
        }
        else
        {
            laRectArray_Clear(&layer->currentDamageRects);
            
            layer->frameState = LA_LAYER_FRAME_READY;
        }
    }
}

void _laLayer_Moved(laLayer* layer)
{
    // update input rectangle
    if(layer->inputRectLocked == LA_TRUE)
        layer->inputRect = layer->widget.rect;
        
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return;
    
    GFX_Set(GFXF_LAYER_ACTIVE, laScreen_GetLayerIndex(layer->screen, layer));
    GFX_Set(GFXF_LAYER_POSITION, layer->widget.rect.x, layer->widget.rect.y);
}

void _laLayer_Resized(laLayer* layer)
{
    // update input rectangle
    if(layer->inputRectLocked == LA_TRUE)
        layer->inputRect = layer->widget.rect;
        
    if(layer->screen == NULL || laContext_GetActive()->activeScreen != layer->screen)
        return;

    GFX_Set(GFXF_LAYER_ACTIVE, laScreen_GetLayerIndex(layer->screen, layer));
    GFX_Set(GFXF_LAYER_SIZE, layer->widget.rect.width, layer->widget.rect.height); 
}

void _laLayer_Paint(laLayer* layer)
{
    if(layer->widget.backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        if(laUtils_WidgetIsOccluded((laWidget*)layer, 
                                    &layer->clippedDrawingRect) == LA_FALSE)
        {
            GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
            GFX_Set(GFXF_DRAW_COLOR, layer->widget.scheme->background);
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);

            GFX_DrawRect(layer->clippedDrawingRect.x,
                         layer->clippedDrawingRect.y,
                         layer->clippedDrawingRect.width,
                         layer->clippedDrawingRect.height);
                         
            /*printf("layer drew %i, %i, %i, %i\n", layer->dmgRectPtr->x,
                         layer->dmgRectPtr->y,
                         layer->dmgRectPtr->width,
                         layer->dmgRectPtr->height);*/
        }
    }
    
    layer->widget.drawState = LA_WIDGET_DRAW_STATE_DONE;
}

void _laLayer_AddDamageRectToList(laRectArray* arr,
								  const GFX_Rect* rect,
								  laBool noCombine)
{
    uint32_t i;
    
    for(i = 0; i < arr->size; i++)
    {
        // nothing to do, damaged area is already covered by an existing rect
        if(GFX_RectContainsRect(&arr->rects[i], rect) == GFX_TRUE)
        {
            return;
        }
        // new rect completely envelopes old rect, just replace
        else if(GFX_RectContainsRect(rect, &arr->rects[i]) == GFX_TRUE)
        {
            arr->rects[i] = *rect;

            return;
        }
        // two rectangles are touching, combine the areas
        else if(noCombine == LA_FALSE && GFX_RectIntersects(rect, &arr->rects[i]) == GFX_TRUE)
        {
            arr->rects[i] = GFX_RectCombine(rect, &arr->rects[i]);

            return;
        }
    }

    laRectArray_PushBack(arr, rect);
}

void _laLayer_InvalidateChildren(laLayer* layer)
{
    uint32_t i;
    
    _laWidget_ValidateChildren((laWidget*)layer);
    
    if(laUtils_WidgetIsOccluded((laWidget*)layer,
       &layer->frameRectList.rects[layer->frameRectIdx]) == GFX_FALSE)
    {
        _laWidget_SetDirtyState((laWidget*)layer, LA_WIDGET_DIRTY_STATE_DIRTY);
    }
    else
    {
        _laWidget_IncreaseDirtyState((laWidget*)layer, LA_WIDGET_DIRTY_STATE_CHILD);
    }
        
    for(i = 0; i < layer->widget.children.size; i++)
        _laWidget_Invalidate(layer->widget.children.values[i],
                             &layer->frameRectList.rects[layer->frameRectIdx]);
}