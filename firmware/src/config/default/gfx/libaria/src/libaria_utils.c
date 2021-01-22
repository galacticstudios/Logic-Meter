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


#include "gfx/libaria/inc/libaria_utils.h"

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_list.h"
#include "gfx/libaria/inc/libaria_screen.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/hal/inc/gfx_rect.h"

static laWidget* pickChild(laWidget* parent, int32_t x, int32_t y)
{
    laWidget* child;
    laWidget* result;
    GFX_Point pnt;
    uint32_t i;

    pnt.x = x;
    pnt.y = y;

    result = parent;

    for(i = 0; i < parent->children.size; i++)
    {
        child = parent->children.values[i];

        // widget must be enabled and visible
        if(child->enabled == LA_TRUE &&
           child->visible == LA_TRUE &&
           GFX_RectContainsPoint(&child->rect, &pnt) == GFX_TRUE)
        {
            result = pickChild(child, x - child->rect.x, y - child->rect.y);
        }
    }

    return result;
}

laWidget* laUtils_Pick(int32_t x, int32_t y)
{
    laContext* context = laContext_GetActive();
    laScreen* screen = NULL;
    laLayer* layer = NULL;
    laWidget* widget = NULL;
    GFX_Point pnt;
    GFX_Rect rect;
    int32_t i;

    if(context == NULL)
        return NULL;

    pnt.x = x;
    pnt.y = y;

    // get the active screen
    screen = context->activeScreen;

    rect = laContext_GetScreenRect();

    // screen must contain point
    if(GFX_RectContainsPoint(&rect, &pnt) == GFX_FALSE)
        return NULL;

    // find the topmost visible layer that contains the point
    for(i = LA_MAX_LAYERS - 1; i >= 0; i--)
    {
        layer = screen->layers[i];

        // layer must exist and be enabled
        if(layer == NULL || layer->widget.enabled == LA_FALSE)
           continue;

        // layer zero has the lowest z order, always favor higher layers
        if(GFX_RectContainsPoint(&layer->widget.rect, &pnt) == GFX_TRUE)
        {
            widget = pickChild((laWidget*)layer,
                                x - layer->widget.rect.x,
                                y - layer->widget.rect.y);
                                
            // no layer qualified
            if(widget != NULL && widget != (laWidget*)layer)
                return widget;
        }
    }

    // no widget qualified
    return NULL;
}

laWidget* laUtils_PickFromLayer(const laLayer* layer, int32_t x, int32_t y)
{
    laContext* context = laContext_GetActive();
    laWidget* widget = NULL;
    GFX_Point pnt;

    if(context == NULL || layer == NULL)
        return NULL;

    pnt.x = x;
    pnt.y = y;

    // layer must contain point
    if(GFX_RectContainsPoint(&layer->inputRect, &pnt) == GFX_TRUE)
    {
        widget = pickChild((laWidget*)layer,
                            x - layer->widget.rect.x,
                            y - layer->widget.rect.y); // coords changed to layer space
                            
        // no widget qualified
        if(widget != NULL)
            return widget;
    }
    else
        return NULL;

    // no widget qualified
    return (laWidget*)layer;
}

/*static void pickChildRect(laWidget* parent, GFX_Rect rect, laList* list)
{
    laWidget* child;
    GFX_Rect childRect;
    uint32_t i;

    for(i = 0; i < parent->children.size; i++)
    {
        child = parent->children.values[i];
        
        childRect = laWidget_RectToLayerSpace(child);
        
        if(GFX_RectIntersects(&rect, &childRect) == LA_TRUE)
            laList_PushBack(list, child);
            
        pickChildRect(child, rect, list);
    }
}*/

/*void laUtils_PickRect(laLayer* layer, GFX_Rect rect, laList* list)
{
    laWidget* child;
    GFX_Rect childRect;
    uint32_t i;

    if(layer == NULL ||
       layer->widget.enabled == LA_FALSE ||
       list == NULL)
        return;
        
    laList_Clear(list);

    // layer must contain rect
    if(GFX_RectIntersects(&layer->widget.rect, &rect) == GFX_FALSE)
        return;
        
    //GFX_RectClip(&rect, &layer->widget.rect, &clippedRect);

    for(i = 0; i < layer->widget.children.size; i++)
    {
        child = layer->widget.children.values[i];
        
        childRect = laWidget_RectToLayerSpace(child);
        
        if(GFX_RectIntersects(&rect, &childRect) == LA_TRUE)
            laList_PushBack(list, child);
            
        pickChildRect(child, rect, list);
    }
}*/

laLayer* laUtils_GetLayer(laWidget* widget)
{
    laWidget* parent;
    
    if(widget == NULL)
        return NULL;
   
    if(widget->type == LA_WIDGET_LAYER)
        return (laLayer*)widget;
        
    parent = widget->parent;
    
    while(parent != NULL)
    {
        if(parent->parent == NULL)
            break;
            
        parent = parent->parent;
    }
    
    if(parent == NULL || parent->type != LA_WIDGET_LAYER)
        return NULL;
    
    return (laLayer*)parent;
}

void laUtils_PointToLayerSpace(laWidget* widget, GFX_Point* pnt)
{
    if(widget == NULL || pnt == NULL)
        return;
        
    while(widget != NULL &&
          widget->type != LA_WIDGET_LAYER)
    {
        pnt->x += widget->rect.x;
        pnt->y += widget->rect.y;
        
        widget = widget->parent;
    }
}

void laUtils_PointScreenToLocalSpace(laWidget* widget, GFX_Point* pnt)
{
    laWidget* parent;

    if(widget == NULL || pnt == NULL)
        return;
        
    pnt->x -= widget->rect.x;
    pnt->y -= widget->rect.y;

    parent = widget->parent;

    while(parent != NULL)
    {
        pnt->x -= parent->rect.x;
        pnt->y -= parent->rect.y;

        parent = parent->parent;
    }
}

void laUtils_ClipRectToParent(laWidget* parent, GFX_Rect* rect)
{
    GFX_Rect result;

    if(parent == NULL || rect == NULL)
        return;

    rect->x += parent->rect.x;
    rect->y += parent->rect.y;

    GFX_RectClip(rect, &parent->rect, &result);

    result.x -= parent->rect.x;
    result.y -= parent->rect.y;

    *rect = result;
}

GFX_Rect laUtils_ClipWidgetToAncestors(laWidget* wgt)
{
    GFX_Rect result = GFX_Rect_Zero;
    
    if(wgt == NULL)
        return result;
    
    result = wgt->rect;

    while(wgt->parent != NULL)
    {
        result.x += wgt->parent->rect.x;
        result.y += wgt->parent->rect.y;
        
        GFX_RectClip(&result, &wgt->parent->rect, &result);
        
        wgt = wgt->parent;
    }

    return result;
}

GFX_Rect laUtils_WidgetLocalRect(laWidget* widget)
{
    GFX_Rect res;
    
	res.x = 0;
	res.y = 0;
	res.width = 0;
	res.height = 0;

	if (widget != NULL)
    {
        res = widget->rect;
        res.x = 0;
        res.y = 0;
    }
    
    return res;
}

GFX_Rect laUtils_WidgetLayerRect(laWidget* widget)
{
    GFX_Rect res;

	res.x = 0;
	res.y = 0;
	res.width = 0;
	res.height = 0;
    
    if(widget != NULL)
    {
        res = widget->rect;
        res.x = 0;
        res.y = 0;
        
        laUtils_RectToLayerSpace(widget, &res);
    }
    
    return res;
}

void laUtils_RectToParentSpace(laWidget* widget, GFX_Rect* rect)
{
    if(widget == NULL || rect == NULL || widget->parent == NULL)
        return;

    rect->x += widget->parent->rect.x;
    rect->y += widget->parent->rect.y;
}

void laUtils_RectFromParentSpace(laWidget* widget, GFX_Rect* rect)
{
    if(widget == NULL || rect == NULL || widget->parent == NULL)
        return;

    rect->x -= widget->parent->rect.x;
    rect->y -= widget->parent->rect.y;
}

void laUtils_RectFromLayerSpace(laWidget* widget, GFX_Rect* rect)
{
    if(widget == NULL || rect == NULL)
        return;

    rect->x -= widget->rect.x;
    rect->y -= widget->rect.y;

    widget = widget->parent;

    while(widget != NULL)
    {
        rect->x -= widget->rect.x;
        rect->y -= widget->rect.y;

        widget = widget->parent;
    }
}

void laUtils_RectToLayerSpace(laWidget* widget, GFX_Rect* rect)
{
    if(widget == NULL || rect == NULL)
        return;
        
    while(widget != NULL && widget->type != LA_WIDGET_LAYER)
    {
        rect->x += widget->rect.x;
        rect->y += widget->rect.y;
        
        widget = widget->parent;
    }
}

void laUtils_RectToScreenSpace(laWidget* widget, GFX_Rect* rect)
{
    if(widget == NULL || rect == NULL)
        return;
        
    while(widget != NULL)
    {
        rect->x += widget->rect.x;
        rect->y += widget->rect.y;

        widget = widget->parent;
    }
}

laBool laUtils_ChildIntersectsParent(laWidget* parent, laWidget* child)
{
    GFX_Rect childRect;
    
    if(parent == NULL || child == NULL)
        return LA_FALSE;
        
    childRect = child->rect;
    
    childRect.x += parent->rect.x;
    childRect.y += parent->rect.y;

    return GFX_RectIntersects(&childRect, &parent->rect);
}

laWidget* laUtils_GetNextHighestWidget(laWidget* wgt)
{
    int32_t idx;
    laWidget *target, *res;
    
    // return first child
    if(wgt->children.size != 0)
        return wgt->children.values[0];
    
    if(wgt->parent == NULL)
        return NULL;
    
    target = wgt->parent;
    
    // return next viable sibling
    while(target != NULL)
    {
        idx = laWidget_GetIndexOfChild(target, wgt);
    
        res = laWidget_GetChildAtIndex(target, ++idx);
        
        if(res != NULL)
            return res;
            
        wgt = target;
        target = wgt->parent;
    }
    
    return NULL;
}

void laUtils_ArrangeRectangle(GFX_Rect* sub,
                              GFX_Rect obj,
                              GFX_Rect bounds,
                              laHAlignment hAlignment,
                              laVAlignment vAlignment,
                              laRelativePosition position,
                              uint8_t leftMargin,
                              uint8_t topMargin,
                              uint8_t rightMargin,
                              uint8_t bottomMargin,
                              uint16_t rectMargin)
{
    sub->x = 0;
    sub->y = 0;
    
    if(hAlignment == LA_HALIGN_LEFT)
    {
        sub->x = bounds.x + leftMargin;
        
        if(obj.width > 0 && position == LA_RELATIVE_POSITION_RIGHTOF)
            sub->x += obj.width + rectMargin;
    }
    else if(hAlignment == LA_HALIGN_CENTER)
    {
        sub->x = bounds.x + (bounds.width / 2) - (sub->width / 2);
        
        if(obj.width > 0)
        {
            if(position == LA_RELATIVE_POSITION_LEFTOF)
                sub->x -= (obj.width / 2) + (rectMargin / 2);
            else if(position == LA_RELATIVE_POSITION_RIGHTOF)
                sub->x += (obj.width / 2) + (rectMargin / 2);
        }
    }
    else
    {
        sub->x = bounds.x + bounds.width - sub->width - rightMargin;
        
        if(obj.width > 0 && position == LA_RELATIVE_POSITION_LEFTOF)
            sub->x -= obj.width + rectMargin;
    }
    
    if(vAlignment == LA_VALIGN_TOP)
    {
        sub->y = bounds.y + topMargin + 1;
        
        if(obj.height > 0)
        {
            if(position == LA_RELATIVE_POSITION_BELOW)
                sub->y += obj.height + rectMargin;
        }
    }
    else if(vAlignment == LA_VALIGN_MIDDLE)
    {
        sub->y = bounds.y + (bounds.height / 2) - (sub->height / 2);
        
        if(obj.height > 0)
        {
            if(position == LA_RELATIVE_POSITION_ABOVE)
                sub->y -= (obj.height / 2) + (rectMargin / 2);
            else if(position == LA_RELATIVE_POSITION_BELOW)
                sub->y += (obj.height / 2) + (rectMargin / 2);
        }
    }
    else
    {
        sub->y = bounds.y + bounds.height - sub->height - bottomMargin ;
        
        if(obj.height > 0 && position == LA_RELATIVE_POSITION_ABOVE)
            sub->y -= obj.height + rectMargin;
    }
}

void laUtils_ArrangeRectangleRelative(GFX_Rect* sub,
                                      GFX_Rect obj,
                                      GFX_Rect bounds,
                                      laHAlignment hAlignment,
                                      laVAlignment vAlignment,
                                      laRelativePosition position,
                                      uint8_t leftMargin,
                                      uint8_t topMargin,
                                      uint8_t rightMargin,
                                      uint8_t bottomMargin,
                                      uint16_t rectMargin)
{
    sub->x = 0;
    sub->y = 0;
    
    if(hAlignment == LA_HALIGN_LEFT)
    {
        sub->x = bounds.x + leftMargin;
        
        if(obj.width > 0)
        {
            if(position == LA_RELATIVE_POSITION_LEFTOF)
                sub->x += obj.width + rectMargin;
            //else
            //    sub->x += (obj.width / 2) - (sub->width / 2);
        }
    }
    else if(hAlignment == LA_HALIGN_CENTER)
    {
        sub->x = bounds.x + (bounds.width / 2) - (sub->width / 2);
        
        if(obj.width > 0)
        {
            if(position == LA_RELATIVE_POSITION_LEFTOF)
                sub->x += (obj.width / 2) + (rectMargin / 2);
            else if(position == LA_RELATIVE_POSITION_RIGHTOF)
                sub->x -= (obj.width / 2) + (rectMargin / 2);
        }
    }
    else
    {
        sub->x = bounds.x + bounds.width - sub->width - rightMargin;
        
        if(obj.width > 0)
        {   
            if(position == LA_RELATIVE_POSITION_RIGHTOF)
                sub->x -= obj.width + rectMargin;
            //else
            //    sub->x -= (obj.width / 2) - (sub->width / 2);
        }
    }
    
    if(vAlignment == LA_VALIGN_TOP)
    {
        sub->y = bounds.y + topMargin + 1;
        
        if(obj.height > 0 && position != LA_RELATIVE_POSITION_BELOW)
        {
            if(position == LA_RELATIVE_POSITION_ABOVE)
                sub->y += obj.height + rectMargin;
            else
                sub->y += (obj.height / 2) - (sub->height / 2);
            
            if (sub->y < (bounds.y + topMargin + 1))
                sub->y = bounds.y + topMargin + 1;
        }
    }
    else if(vAlignment == LA_VALIGN_MIDDLE)
    {
        sub->y = bounds.y + (bounds.height / 2) - (sub->height / 2);
        
        if(obj.height > 0)
        {
            if(position == LA_RELATIVE_POSITION_ABOVE)
                sub->y += (obj.height / 2) + (rectMargin / 2);
            else if(position == LA_RELATIVE_POSITION_BELOW)
                sub->y -= (obj.height / 2) + (rectMargin / 2);
        }
    }
    else
    {
        sub->y = bounds.y + bounds.height - sub->height - bottomMargin;
        
        if(obj.height > 0 && position != LA_RELATIVE_POSITION_ABOVE)
        {
            if(position == LA_RELATIVE_POSITION_BELOW)
                sub->y -= obj.height + rectMargin;
            else
                sub->y -= (obj.height / 2) - (sub->height / 2);
            
            if ((sub->y + sub->height) > (bounds.y + bounds.height - bottomMargin))
                sub->y = bounds.y + bounds.height - sub->height - bottomMargin;
        }
    }
}

laBool _occlusionCullTestAscending(laWidget* widget, GFX_Rect* bounds)
{
    uint32_t i, idx;
    laWidget *parent, *child;
    GFX_Rect childRect;
    
    // root node of tree, can't possibly occlude
    if(widget->parent == NULL)
        return LA_FALSE;
        
    parent = widget->parent;
        
    bounds->x += parent->rect.x;
    bounds->y += parent->rect.y;
    
    idx = laArray_Find(&parent->children, widget);
    
    if(idx < parent->children.size - 1)
    {    
        idx += 1;
        
        // test all rightside siblings, leftside can't occlude
        for(i = idx; i < parent->children.size; i++)
        {
            child = parent->children.values[i];
            
            childRect = child->rect;
            
            // move child rect into parent space
            //childRect.x += widget->rect.x;
            //childRect.y += widget->rect.y;
            
            if(GFX_RectContainsRect(&childRect, bounds) == LA_TRUE)
                return LA_TRUE;
        }
    }
    
    return _occlusionCullTestAscending(parent, bounds);
}

laBool laUtils_OcclusionCullTest(laWidget* widget)
{
    uint32_t i;
    laWidget* child;
    GFX_Rect childRect;
    
    if(widget == NULL)
        return LA_FALSE;
    
    // test children first
    for(i = 0; i < widget->children.size; i++)
    {
        child = widget->children.values[i];
        
        childRect = child->rect;
        
        // move child rect into parent space
        //childRect.x += widget->rect.x;
        //childRect.y += widget->rect.y;
        
        if(GFX_RectContainsRect(&childRect, &widget->rect) == LA_TRUE)
            return LA_TRUE;
    }
    
    // test rightmost ancestors
    childRect = widget->rect;
    
    return _occlusionCullTestAscending(widget, &childRect);
}

void laUtils_ListOcclusionCullTest(laList* list, GFX_Rect rect)
{
    uint32_t i, j;
    laWidget *widget, *higherWidget;
    GFX_Rect widgetRect;//, lclippedRect, rclippedRect;
    laList cullList;
    laBool cull;
    
    laList_Create(&cullList);
    laList_Copy(&cullList, list);
    laList_Clear(list);
    
    if(cullList.size == 0)
        return;

    for(i = 0; i <= cullList.size - 1; i++)
    {
        cull = LA_FALSE;
        
        widget = (laWidget*)laList_Get(&cullList, i);
        
        // topmost widget is always a pass
        if(i == cullList.size - 1)
        {
            laList_PushBack(list, widget);
        
            continue;
        }
        
        widgetRect = laWidget_RectToLayerSpace(widget);
        
        //GFX_RectClip(&widgetRect, &rect, &lclippedRect);
        
        for(j = i + 1; j < cullList.size; j++)
        {
            higherWidget = (laWidget*)laList_Get(&cullList, j);
            
            //GFX_RectClip(&higherWidget->rect, &widgetRect, &rclippedRect);
            
            if(GFX_RectContainsRect(&higherWidget->rect, &widgetRect) == LA_TRUE &&
               laWidget_isOpaque(higherWidget) == LA_TRUE)
            {
                cull = LA_TRUE;
                
                break;
            }
        }
        
        if(cull == LA_FALSE)
            laList_PushBack(list, widget);
    }
    
    laList_Clear(&cullList);
}

laBool laUtils_WidgetIsOccluded(laWidget* wgt, const GFX_Rect* rect)
{
    laWidget* target;
    GFX_Rect targetRect;
    
    if(laWidget_GetCumulativeAlphaEnable(wgt) == LA_TRUE)
        return LA_FALSE;
    
    target = laUtils_GetNextHighestWidget(wgt);
    
    while(target != NULL)
    {
        targetRect = laUtils_WidgetLayerRect(target);
        
// BA added this code because Aria wasn't checking whether a widget's parent(s)
// are visible when determining if a widget is visible
        laBool  visible = target->visible;
        laWidget *targetParent = target->parent;
        while (targetParent && visible)
        {
            visible = targetParent->visible;
            targetParent = targetParent->parent;
        }
        
        if(visible == GFX_TRUE &&
           target->alphaEnabled == GFX_FALSE &&
           (target->backgroundType != LA_WIDGET_BACKGROUND_NONE || (target->optimizationFlags & LA_WIDGET_OPT_OPAQUE) > 0) &&
           GFX_RectContainsRect(&targetRect, rect) == GFX_TRUE)
            return LA_TRUE;
            
        target = laUtils_GetNextHighestWidget(target);
    }
    
    return LA_FALSE;
}

typedef GFX_Point (*laOrient_FnPtr)(const GFX_Point*, const GFX_Rect*);

static GFX_Point orient_0(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point orient_90(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point orient_180(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point orient_270(const GFX_Point* pnt, const GFX_Rect* rect);

static laOrient_FnPtr orient_ptrs[] =
{
    &orient_0,
    &orient_90,
    &orient_180,
    &orient_270
};

static GFX_Point orient_0(const GFX_Point* pnt, const GFX_Rect* rect)
{ 
    GFX_Point res;
    
    res = *pnt;
    
    return res;
}

static GFX_Point orient_90(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;

    res.y = rect->width - 1 - pnt->x;
    res.x = pnt->y;
    
    return res;
}

static GFX_Point orient_180(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;
    
    res.x = rect->width - 1 - pnt->x;
    res.y = rect->height - 1 - pnt->y;
    
    return res;
}

static GFX_Point orient_270(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;
    
    res.x = rect->height - 1 - pnt->y;
    res.y = pnt->x;
    
    return res;
}

GFX_Point laUtils_ScreenToOrientedSpace(const GFX_Point* pnt,
                                        const GFX_Rect* rect,
                                        GFX_Orientation ori)
{
    return orient_ptrs[ori](pnt, rect);    
}

typedef GFX_Point (*laMirror_FnPtr)(const GFX_Point*, const GFX_Rect*);

static GFX_Point mirror_0(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point mirror_90(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point mirror_180(const GFX_Point* pnt, const GFX_Rect* rect);
static GFX_Point mirror_270(const GFX_Point* pnt, const GFX_Rect* rect);

static laMirror_FnPtr mirror_ptrs[] =
{
    &mirror_0,
    &mirror_90,
    &mirror_180,
    &mirror_270
};

static GFX_Point mirror_0(const GFX_Point* pnt, const GFX_Rect* rect)
{ 
    GFX_Point res;
    
    res.x = rect->width - 1 - pnt->x;
    res.y = pnt->y;
    
    return res;
}

static GFX_Point mirror_90(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;

    res.x = pnt->x;
    res.y = rect->height - 1 - pnt->y;
    
    return res;
}

static GFX_Point mirror_180(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;
    
    res.x = rect->width - 1 - pnt->x;
    res.y = pnt->y;

    return res;
}

static GFX_Point mirror_270(const GFX_Point* pnt, const GFX_Rect* rect)
{
    GFX_Point res;

    res.x = pnt->x;
    res.y = rect->height - 1 - pnt->y;
    
    return res;
}

GFX_Point laUtils_ScreenToMirroredSpace(const GFX_Point* pnt,
                                        const GFX_Rect* rect,
                                        GFX_Orientation ori)
{
    return mirror_ptrs[ori](pnt, rect);    
}

laResult laUtils_PreprocessImage(GFXU_ImageAsset* img,
                                uint32_t destAddress,
                                GFX_ColorMode destMode,
                                GFX_Bool padBuffer, 
                                GFXU_MemoryIntf * memIntf)
{
    GFX_PixelBuffer buf;
    uint32_t width = img->width;
    uint32_t height = img->height;
    uint32_t p;
    GFX_Rect rect;
    GFX_Bool alphaEnable;
    
    if(padBuffer == GFX_TRUE)
    {
        if(width && !(width & (width - 1)) == 0)
        {
            p = 1;
            
            while(width >= p)
                p <<= 1;
            
            width = p;
        }
        
        if(height && !(height & (height - 1)) == 0)
        {
            p = 1;
            
            while(height >= p)
                p <<= 1;
            
            height = p;
        }
    }   
    
    GFX_PixelBufferCreate(width,
                          height,
                          destMode,
                          (void*)destAddress,
                          &buf);
    
    if((img->flags & GFXU_IMAGE_USE_MASK) > 0)
    {
        rect.x = 0;
        rect.y = 0;
        rect.width = buf.size.width;
        rect.height = buf.size.height;
        
        GFX_PixelBufferAreaFill(&buf, &rect, GFX_ColorConvert(img->colorMode, destMode, img->mask));
    }
    else
    {
        memset(buf.pixels, 0, buf.buffer_length);
    }
    
    GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
    GFX_Set(GFXF_DRAW_ALPHA_ENABLE, GFX_FALSE);
    GFX_Set(GFXF_DRAW_TARGET, &buf);
    
    GFX_Begin();
    
    if (img->header.dataLocation == GFXU_ASSET_LOCATION_INTERNAL)
    {
        GFXU_DrawImage(img,
                       0,
                       0,
                       img->width,
                       img->height,
                       0,
                       0,
                       NULL,
                       NULL);        
    }
    else
    {
        GFXU_ExternalAssetReader* reader = NULL;
        GFX_Result drawResult = GFX_SUCCESS;
        
        do
        {
            drawResult = GFXU_DrawImage(img,
                                        0,
                                        0,
                                        img->width,
                                        img->height,
                                        0,
                                        0,
                                        memIntf,
                                        &reader);
            
            if(reader != NULL)
            {
                while (reader->status != GFXU_READER_STATUS_FINISHED)
                {
                    reader->run(reader);
                }
                
                if (reader != NULL)
                {
                    memIntf->heap.free(reader);
                    reader = NULL;
                }

            }
        } while (drawResult == GFX_SUCCESS && reader != NULL);
    }
    
    GFX_End();
    
    GFX_Set(GFXF_DRAW_TARGET, NULL);
    GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);
    
    img->header.dataAddress = (void*)destAddress;
    img->header.dataLocation = 0;
    img->header.dataSize = buf.buffer_length;
    img->bufferWidth = buf.size.width;
    img->bufferHeight = buf.size.height;
    img->format = GFXU_IMAGE_FORMAT_RAW;
    img->compType = GFXU_IMAGE_COMPRESSION_NONE;
    
    if((img->flags & GFXU_IMAGE_USE_MASK) > 0)
        img->mask = GFX_ColorConvert(img->colorMode, destMode, img->mask);
    else
        img->mask = 0;
    
    img->palette = NULL;
    img->colorMode = destMode;
    
    return LA_SUCCESS;
}

