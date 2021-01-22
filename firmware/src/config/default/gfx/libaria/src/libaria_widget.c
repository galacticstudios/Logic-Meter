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


#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_utils.h"

#include "gfx/hal/inc/gfx_math.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

static laWidgetUpdateState update(laWidget* wgt, uint32_t ms)
{
    //printf("updating %u\n", wgt->id);
    return LA_WIDGET_UPDATE_STATE_DONE;
}

static void moved(laWidget* wgt) { }
static void resized(laWidget* wgt) { }
static void focusGained(laWidget* wgt) { }
static void focusLost(laWidget* wgt) { }

static void touchDownEvent(laWidget* wgt, laInput_TouchDownEvent* evt) { }
static void touchUpEvent(laWidget* wgt, laInput_TouchUpEvent* evt) { }
static void touchMovedEvent(laWidget* wgt, laInput_TouchMovedEvent* evt) { }

static void languageChangingEvent(laWidget* wgt) { }

void _laWidget_Constructor(laWidget* wgt)
{
    wgt->id = laContext_GetActive()->widgetIDs++;

    wgt->type = LA_WIDGET_WIDGET;
    
    wgt->visible = LA_TRUE;
    wgt->enabled = LA_TRUE;
    wgt->dirtyState = LA_WIDGET_DIRTY_STATE_DIRTY;
    wgt->drawState = LA_WIDGET_DRAW_STATE_READY;

    wgt->rect.x = 0;
    wgt->rect.y = 0;
    wgt->rect.width = 1;
    wgt->rect.height = 1;
    wgt->cornerRadius = 0;

    wgt->borderType = LA_WIDGET_BORDER_NONE;
    wgt->backgroundType = LA_WIDGET_BACKGROUND_FILL;
            
    wgt->alphaEnabled = LA_FALSE;
    wgt->alphaAmount = 255;
            
    wgt->margin.left = DEFAULT_BORDER_MARGIN;
    wgt->margin.top = DEFAULT_BORDER_MARGIN;
    wgt->margin.right = DEFAULT_BORDER_MARGIN;
    wgt->margin.bottom = DEFAULT_BORDER_MARGIN;
    
    wgt->scheme = laContext_GetDefaultScheme();

    wgt->parent = NULL;
    laArray_Create(&wgt->children);

    wgt->destructor = &_laWidget_Destructor;

    wgt->moved = &moved;
    wgt->resized = &resized;
    
    wgt->focusGained = &focusGained;
    wgt->focusLost = &focusLost;

    wgt->update = &update;
    wgt->paint = &_laWidget_Paint;

    wgt->touchDown = &touchDownEvent;
    wgt->touchUp = &touchUpEvent;
    wgt->touchMoved = &touchMovedEvent;
    wgt->destructor = &_laWidget_Destructor;
    
    wgt->languageChangeEvent = &languageChangingEvent;
	wgt->invalidateBorderAreas = &laWidget_SkinClassic_InvalidateBorderAreas;
}

laWidget* laWidget_New()
{
    laWidget* wgt = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    wgt = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laWidget));

    if(wgt == NULL)
        return NULL;
    
    _laWidget_Constructor(wgt);

    return wgt;
}

void laWidget_Delete(laWidget* wgt)
{
    if(wgt == NULL || laContext_GetActive() == NULL)
        return;
        
    wgt->destructor(wgt);
    
    laContext_GetActive()->memIntf.heap.free(wgt);
}

void _laWidget_Destructor(laWidget* wgt)
{
    laWidget* child;
    uint32_t i;
    
    if(wgt == NULL)
        return;
        
    // destroy local cache memory
    if(wgt->cache != NULL)
    {
        GFX_PixelBufferDestroy(wgt->cache,
                               &laContext_GetActive()->memIntf.heap);
        
        laContext_GetActive()->memIntf.heap.free(wgt->cache);
        
        wgt->cache = NULL;
    }
    
    for(i = 0; i < wgt->children.size; i++)
    {
        child = wgt->children.values[i];
        child->destructor(child);
    }

    laArray_Destroy(&wgt->children);
}

void _laWidget_Update(laWidget* wgt, uint32_t ms)
{ }

laBool laWidget_GetEnabled(laWidget* wgt)
{
    if(wgt == NULL)
        return LA_FALSE;
        
    return wgt->enabled;
}

laResult laWidget_SetEnabled(laWidget* wgt, laBool enable)
{
    if(wgt == NULL ||
       wgt->enabled == enable ||
       wgt->type == LA_WIDGET_LAYER) // layer enabled must be set through layer API)
        return LA_FAILURE;
        
    wgt->enabled = enable;
    
    laWidget_Invalidate(wgt);
    
    return LA_SUCCESS;
}

laBool laWidget_GetVisible(laWidget* wgt)
{
    if(wgt == NULL)
        return LA_FALSE;
        
    return wgt->visible;
}

laResult laWidget_SetVisible(laWidget* wgt, laBool visible)
{
    if(wgt == NULL || wgt->visible == visible)
        return LA_FAILURE;
        
    wgt->visible = visible;
    
    laWidget_Invalidate(wgt);
        
    return LA_SUCCESS;
}

int32_t laWidget_GetX(laWidget* wgt)
{
    if(wgt == NULL)
        return 0;

    return wgt->rect.x;
}

laResult laWidget_SetX(laWidget* wgt, int32_t x)
{
    laLayer* layer;
    GFX_Rect area;

    if(wgt == NULL)
        return LA_FAILURE;
        
    if(wgt->rect.x == x)
        return LA_SUCCESS;

    layer = laUtils_GetLayer(wgt);

    // invalidate old area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
        
    wgt->rect.x = x;

    // invalidate new area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
        
    return LA_SUCCESS;
}

int32_t laWidget_GetY(laWidget* wgt)
{
    if(wgt == NULL)
        return 0;

    return wgt->rect.y;
}

laResult laWidget_SetY(laWidget* wgt, int32_t y)
{
    laLayer* layer;
    GFX_Rect area;

    if(wgt == NULL)
        return LA_FAILURE;
        
    if(wgt->rect.y == y)
        return LA_SUCCESS;

    layer = laUtils_GetLayer(wgt);

    // invalidate old area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
        
    wgt->rect.y = y;
    
    // invalidate new area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
        
    return LA_SUCCESS;
}

laResult laWidget_SetPosition(laWidget* wgt, int32_t x, int32_t y)
{   
    GFX_Rect area;
    laLayer* layer;
    
    if(wgt == NULL ||
       (wgt->rect.x == x && wgt->rect.y == y))
        return LA_FAILURE;

    layer = laUtils_GetLayer(wgt);

    // invalidate old area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);

    wgt->rect.x = x;
    wgt->rect.y = y;

    // invalidate new area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);

    wgt->moved(wgt);
    
    return LA_SUCCESS;
}

laResult laWidget_Translate(laWidget* wgt, int32_t x, int32_t y)
{
    GFX_Rect area;
    laLayer* layer;
    
    if(wgt == NULL || (x == 0 && y == 0))
        return LA_FAILURE;

    layer = laUtils_GetLayer(wgt);

    // invalidate old area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
    
    wgt->rect.x += x;
    wgt->rect.y += y;

    // invalidate new area 
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
      
    wgt->moved(wgt);
    
    return LA_SUCCESS;
}

int32_t laWidget_GetWidth(laWidget* wgt)
{
    if(wgt == NULL)
        return 0;

    return wgt->rect.width;
}

laResult laWidget_SetWidth(laWidget* wgt, int32_t width)
{
    GFX_Rect area;
    laLayer* layer;

    if(wgt == NULL)
        return LA_FAILURE;
        
    if(wgt->rect.width == width)
        return LA_SUCCESS;

    layer = laUtils_GetLayer(wgt);

    // invalidate old area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
        
    wgt->rect.width = width;
    
    // invalidate new area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
        
    return LA_SUCCESS;
}

int32_t laWidget_GetHeight(laWidget* wgt)
{
    if(wgt == NULL)
        return 0;

    return wgt->rect.height;
}

laResult laWidget_SetHeight(laWidget* wgt, int32_t height)
{
    GFX_Rect area;
    laLayer* layer;

    if(wgt == NULL)
        return LA_FAILURE;
        
    if(wgt->rect.height == height)
        return LA_SUCCESS;

    layer = laUtils_GetLayer(wgt);

    // invalidate old area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
        
    wgt->rect.height = height;
    
    // invalidate new area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
        
    return LA_SUCCESS;
}

laBool laWidget_GetAlphaEnable(laWidget* wgt)
{
    if(wgt == NULL)
        return 0;
        
    return wgt->alphaEnabled;
}

laBool laWidget_GetCumulativeAlphaEnable(laWidget* wgt)
{
    if(wgt == NULL)
        return 0;
        
    while(wgt != NULL)
    {
        if(wgt->alphaEnabled == LA_TRUE)
            return LA_TRUE;
        
        wgt = wgt->parent; 
    }
    
    return LA_FALSE;
}

laResult laWidget_SetAlphaEnable(laWidget* wgt, laBool enable)
{
    if(wgt == NULL ||
       wgt->alphaEnabled == enable ||
       wgt->type == LA_WIDGET_LAYER) // layer alpha must be set through layer API
        return LA_FAILURE;
        
    wgt->alphaEnabled = enable;
    
    laWidget_Invalidate(wgt);
        
    return LA_SUCCESS;
}

uint32_t laWidget_GetAlphaAmount(laWidget* wgt)
{
    if(wgt == NULL)
        return 0;
        
    return wgt->alphaAmount;
}

uint32_t laWidget_GetCumulativeAlphaAmount(laWidget* wgt)
{
    int32_t amount = 0;
    int32_t localAmount = 0;
    int32_t actualAmount = 0;
    
    if(wgt == NULL || laWidget_GetCumulativeAlphaEnable(wgt) == GFX_FALSE)
        return 255;
        
    localAmount = GFX_PercentWholeRounded(wgt->alphaAmount, 255);
    actualAmount = localAmount;
        
    wgt = wgt->parent;
        
    while(wgt != NULL && laWidget_GetCumulativeAlphaEnable(wgt) == GFX_TRUE)
    {
        amount = GFX_PercentWholeRounded(wgt->alphaAmount, 255);
        
        actualAmount = GFX_PercentOf(actualAmount, amount);
        
        wgt = wgt->parent;
    }
    
    return GFX_PercentOf(255, actualAmount);
}

laResult laWidget_SetAlphaAmount(laWidget* wgt, uint32_t alpha)
{
    if(wgt == NULL || wgt->alphaAmount == alpha)
        return LA_FAILURE;
        
    if(alpha > 255)
        alpha = 255;
        
    wgt->alphaAmount = alpha;
    
    if(wgt->type != LA_WIDGET_LAYER)
        laWidget_Invalidate(wgt);
        
    return LA_SUCCESS;
}

laBool laWidget_isOpaque(laWidget* wgt)
{
    if (laWidget_GetCumulativeAlphaEnable(wgt) == LA_TRUE &&
        laWidget_GetCumulativeAlphaAmount(wgt) < 255)
         return LA_FALSE;
         
    if(wgt->backgroundType == LA_WIDGET_BACKGROUND_NONE &&
       (wgt->optimizationFlags & LA_WIDGET_OPT_OPAQUE) == 0)
       return LA_FALSE;
       
    return LA_TRUE;
}

GFX_Rect laWidget_RectToParentSpace(laWidget* wgt)
{
    GFX_Rect rect = {0};
    
    if(wgt != NULL && wgt->parent != NULL)
    {
        rect = wgt->rect;
        
        rect.x += wgt->parent->rect.x;
        rect.y += wgt->parent->rect.y;
    }
    
    return rect;
}

GFX_Rect laWidget_RectToLayerSpace(laWidget* wgt)
{
    GFX_Rect rect;
    
	rect.x = 0;
	rect.y = 0;
	rect.width = 0;
	rect.height = 0;

	if (wgt == NULL)
        return rect;
    
    rect = wgt->rect;
    
    if(wgt->parent != NULL)
    {
        laUtils_RectToLayerSpace(wgt->parent, &rect);
    }
    
    return rect;
}

GFX_Rect laWidget_RectToScreenSpace(laWidget* wgt)
{
    GFX_Rect rect = {0};
    
	rect.x = 0;
	rect.y = 0;
	rect.width = 0;
	rect.height = 0;

	if (wgt != NULL && wgt->parent != NULL)
    {
        rect = wgt->rect;
        
        laUtils_RectToScreenSpace(wgt->parent, &rect);
    }
    
    return rect;
}

laResult laWidget_SetSize(laWidget* wgt, uint32_t width, uint32_t height)
{
    GFX_Rect area;
    laLayer* layer;
    
    if(wgt == NULL ||
       (wgt->rect.width == width && wgt->rect.height == height))
        return LA_FAILURE;

    layer = laUtils_GetLayer(wgt);

    if(width == 0)
        width = 1;

    if(height == 0)
        height = 1;
        
    // invalidate old area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);

    wgt->rect.width = width;
    wgt->rect.height = height;

    // invalidate new area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);

    wgt->resized(wgt);
    
    return LA_SUCCESS;
}

laResult laWidget_Resize(laWidget* wgt, int32_t width, int32_t height)
{
    GFX_Rect area;
    laLayer* layer;
    
    if(wgt == NULL || (width == 0 && height == 0))
        return LA_FAILURE;

    layer = laUtils_GetLayer(wgt);

    // invalidate old area    
    area = wgt->rect;
    laUtils_RectToLayerSpace(wgt, &area);
    
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
    
    wgt->rect.width += width;
    wgt->rect.height += height;

    if(wgt->rect.width < 1)
        wgt->rect.width = 1;

    if(wgt->rect.height < 1)
        wgt->rect.height = 1;

    // invalidate new area    
    area = wgt->rect;
    laUtils_RectToLayerSpace(wgt, &area);
    
    laLayer_AddDamageRect(layer, &area, LA_FALSE);

    wgt->resized(wgt);
    
    return LA_SUCCESS;
}

laBorderType laWidget_GetBorderType(laWidget* wgt)
{
    if(wgt == NULL)
        return LA_WIDGET_BORDER_NONE;

    return wgt->borderType;
}

laResult laWidget_SetBorderType(laWidget* wgt, laBorderType type)
{
    if(wgt == NULL)
        return LA_FAILURE;

    if(wgt->borderType == type)
        return LA_SUCCESS;
        
    wgt->borderType = type;
    
	wgt->invalidateBorderAreas(wgt);
    
    return LA_SUCCESS;
}

laBackgroundType laWidget_GetBackgroundType(laWidget* wgt)
{
    if(wgt == NULL)
        return LA_WIDGET_BACKGROUND_NONE;

    return wgt->backgroundType;
}

laResult laWidget_SetBackgroundType(laWidget* wgt, laBackgroundType type)
{
    if(wgt == NULL)
        return LA_FAILURE;
        
    if(wgt->backgroundType == type)
        return LA_SUCCESS;
        
    // layers can't use cache mode
    if(wgt->type == LA_WIDGET_LAYER && type == LA_WIDGET_BACKGROUND_CACHE)
        return LA_FAILURE;
        
    // turning cache off
    if(wgt->backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        if(wgt->cache != NULL)
        {
            GFX_PixelBufferDestroy(wgt->cache,
                                   &laContext_GetActive()->memIntf.heap);
                                   
            laContext_GetActive()->memIntf.heap.free(wgt->cache);
            
            wgt->cache = NULL;
            
            wgt->cacheInvalid = LA_FALSE;
        }
    }

    
    
    // turning cache on
    if(type == LA_WIDGET_BACKGROUND_CACHE)
        wgt->cacheInvalid = LA_TRUE;
    
    wgt->backgroundType = type;
    
    laWidget_Invalidate(wgt);
    
    return LA_SUCCESS;
}

laBool laWidget_GetOptimizationFlags(laWidget* wgt)
{
    if(wgt == NULL)
        return LA_FALSE;

    return wgt->optimizationFlags;
}

laResult laWidget_SetOptimizationFlags(laWidget* wgt, uint32_t val)
{
    if(wgt == NULL)
        return LA_FAILURE;

    wgt->optimizationFlags = val;
    
    return LA_SUCCESS;
}

// BA fixed the function name to match the documentation
laResult laWidget_GetMargin(laWidget* wgt, laMargin* mg)
{
    if(wgt == NULL)
        return LA_FAILURE;
        
    *mg = wgt->margin;
    
    return LA_SUCCESS;
}

laResult laWidget_SetMargins(laWidget* wgt,
                             uint32_t left,
                             uint32_t top,
                             uint32_t right,
                             uint32_t bottom)
{
    if(wgt == NULL)
        return LA_FAILURE;
        
    if(wgt->margin.left == left &&
       wgt->margin.top == top &&
       wgt->margin.right == right &&
       wgt->margin.bottom == bottom)
       return LA_FAILURE;
        
    wgt->margin.left = left;
    wgt->margin.top = top;
    wgt->margin.right = right;
    wgt->margin.bottom = bottom;
    
    laWidget_Invalidate(wgt);
    
    return LA_SUCCESS;
}

uint32_t laWidget_GetCornerRadius(laWidget* wgt)
{
    if(wgt == NULL)
        return 0;
        
    return wgt->cornerRadius;
}

laResult laWidget_SetCornerRadius(laWidget* wgt,
                                  uint32_t radius)
{
    if(wgt == NULL)
        return LA_FAILURE;
    
    if (wgt->borderType != LA_WIDGET_BORDER_NONE &&
        wgt->borderType != LA_WIDGET_BORDER_LINE)
        return LA_FAILURE;
        
    if(wgt->cornerRadius == radius)
       return LA_FAILURE;
        
    wgt->cornerRadius = radius;
    
    laWidget_Invalidate(wgt);
    
    return LA_SUCCESS;
}

laBool laWidget_HasFocus(laWidget* wgt)
{
    if(laContext_GetActive() == NULL)
        return LA_FALSE;
        
    return laContext_GetActive()->focus == wgt;
}

laResult laWidget_SetFocus(laWidget* wgt)
{
    return laContext_SetFocusWidget(wgt);
}

laResult laWidget_OverrideTouchDownEvent(laWidget* wgt,
                                         laWidget_TouchDownEvent_FnPtr ptr)
{
    if(wgt == NULL || ptr == NULL)
        return LA_FAILURE;
        
    wgt->touchDown = ptr;
        
    return LA_SUCCESS;
}

laResult laWidget_OverrideTouchUpEvent(laWidget* wgt,
                                       laWidget_TouchUpEvent_FnPtr ptr)
{
    if(wgt == NULL || ptr == NULL)
        return LA_FAILURE;
        
    wgt->touchUp = ptr;
        
    return LA_SUCCESS;
}

laResult laWidget_OverrideTouchMovedEvent(laWidget* wgt,
                                          laWidget_TouchMovedEvent_FnPtr ptr)
{
    if(wgt == NULL || ptr == NULL)
        return LA_FAILURE;
        
    wgt->touchMoved = ptr;
        
    return LA_SUCCESS;
}

static laBool isAncestorOf(laWidget* sub, laWidget* obj)
{
    if(sub == NULL || obj == NULL)
        return LA_FALSE;
        
    if(obj->parent == NULL)
        return LA_FALSE;
        
    if(sub == obj->parent)
        return LA_TRUE;
        
    return isAncestorOf(sub, obj->parent);
}

laResult laWidget_AddChild(laWidget* parent, laWidget* child)
{
    if(parent == NULL || child == NULL || child->root == LA_TRUE ||
       isAncestorOf(child, parent) == LA_TRUE)
        return LA_FAILURE;
        
    if(child->parent != NULL)
        laWidget_RemoveChild(child->parent, child);

    laArray_PushBack(&parent->children, child);
    child->parent = parent;

    laWidget_Invalidate(child);

    return LA_SUCCESS;
}

laResult laWidget_RemoveChild(laWidget* parent, laWidget* child)
{
    if(child == NULL || parent == NULL || child->parent != parent)
        return LA_FAILURE;
        
    laArray_Remove(&parent->children, child);
    child->parent = NULL;
        
    laWidget_Invalidate(parent);
    
    return LA_SUCCESS;
}

laResult laWidget_SetParent(laWidget* wgt, laWidget* parent)
{
    if(wgt == NULL || parent == NULL)
        return LA_FAILURE;
        
    if(wgt->parent != NULL)
        laWidget_RemoveChild(wgt->parent, wgt);

    if(parent == NULL)
        return LA_SUCCESS;
        
    return laWidget_AddChild(parent, wgt);
}

uint32_t laWidget_GetChildCount(laWidget* parent)
{
    if(parent == NULL)
        return 0;
        
    return parent->children.size;
}

laWidget* laWidget_GetChildAtIndex(laWidget* parent, uint32_t idx)
{
    if(parent == NULL || idx >= parent->children.size)
        return NULL;
        
    return parent->children.values[idx];
}

int32_t laWidget_GetIndexOfChild(laWidget* parent, laWidget* child)
{
    if(parent == NULL || child == NULL)
        return 0;
        
    return laArray_Find(&parent->children, child);
}

void laWidget_DeleteAllDescendants(laWidget* wgt)
{
    laWidget* child;
    uint32_t i;
    
    if(wgt == NULL)
        return;
        
    for(i = 0; i < wgt->children.size; i++)
    {
        child = wgt->children.values[i];
        
        if (child != NULL && child->children.size > 0)
        {
            laWidget_DeleteAllDescendants(child);
        }
        
        child->destructor(child);
    }
    
    laArray_Free(&wgt->children);
}

laScheme* laWidget_GetScheme(laWidget* wgt)
{
    if(wgt == NULL)
        return NULL;

    return wgt->scheme;
}

laResult laWidget_SetScheme(laWidget* wgt, laScheme* scheme)
{
    if(wgt == NULL || wgt->scheme == scheme)
        return LA_FAILURE;

    wgt->scheme = scheme;

    laWidget_Invalidate(wgt);
    
    return LA_SUCCESS;
}

void laWidget_Invalidate(laWidget* wgt)
{
    GFX_Rect area;
    laLayer* layer;
    
    if(wgt == NULL)
        return;

    layer = laUtils_GetLayer(wgt);

    // invalidate entire area
    area = laWidget_RectToLayerSpace(wgt);
    laLayer_AddDamageRect(layer, &area, LA_FALSE);
}

void _laWidget_Invalidate(laWidget* wgt,  const GFX_Rect* rect)
{
    uint32_t i;
    GFX_Rect widgetRect;
    laWidget* parent;

    // if widget is completely outside of parent's bounds
    // then nothing needs to be done
    if(wgt->type != LA_WIDGET_LAYER &&  
       laUtils_ChildIntersectsParent(wgt->parent, wgt) == GFX_FALSE)
    {
        // mark this widget and all children as clean
        //_laWidget_ValidateChildren(wgt);
        
        return;
    }
    
    widgetRect = laUtils_WidgetLayerRect(wgt);
    
    if(GFX_RectIntersects(&widgetRect, rect) == GFX_FALSE)
        return;
        
    if(laUtils_WidgetIsOccluded(wgt, rect) == GFX_FALSE)
    {
        _laWidget_IncreaseDirtyState(wgt, LA_WIDGET_DIRTY_STATE_DIRTY);
        
        parent = wgt->parent;
        
        if(parent != NULL)
        {
            if(parent->dirtyState != LA_WIDGET_DIRTY_STATE_CHILD)
            {
                parent = wgt->parent;
                
                while(parent != NULL)
                {
                    _laWidget_IncreaseDirtyState(parent, LA_WIDGET_DIRTY_STATE_CHILD);
                    
                    parent = parent->parent;
                }
            }
        }
    }
    
    // attempt to invalidate children
    for(i = 0; i < wgt->children.size; i++)
        _laWidget_Invalidate(wgt->children.values[i], rect);
}

void _laWidget_ValidateChildren(laWidget* wgt)
{
    uint32_t i;
    laWidget* child;

    _laWidget_ClearDirtyState(wgt);
    wgt->drawState = LA_WIDGET_DRAW_STATE_READY;

    for(i = 0; i < wgt->children.size; i++)
    {
        child = wgt->children.values[i];

        _laWidget_ValidateChildren(child);
    }
}

void _laWidget_IncreaseDirtyState(laWidget* wgt, uint32_t state)
{
    if(state > wgt->dirtyState)
        wgt->dirtyState = state;
}

void _laWidget_SetDirtyState(laWidget* wgt, uint32_t state)
{
    wgt->dirtyState = state;
}

void _laWidget_ClearDirtyState(laWidget* wgt)
{
    wgt->dirtyState = LA_WIDGET_DIRTY_STATE_CLEAN;
}
