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


#include "gfx/libaria/inc/libaria_widget_listwheel.h"

#if LA_LISTWHEEL_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/hal/inc/gfx_math.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

#define DEFAULT_MARGIN          10
#define DEFAULT_VISIBLE_COUNT   5

#define DEFAULT_INDICATOR_AREA  20

#define DEFAULT_DRAG_DELTA        20
#define MAX_MOMENTUM              100
#define DEFAULT_MOMENTUM_FALLOFF  2
#define DEFAULT_ROTATION_TICK     60

static laWidgetUpdateState update(laListWheelWidget* whl, uint32_t ms);
static void resized(laListWheelWidget* whl);
static void touchDown(laListWheelWidget* whl, laInput_TouchDownEvent* evt);
static void touchUp(laListWheelWidget* whl, laInput_TouchUpEvent* evt);
static void touchMoved(laListWheelWidget* whl, laInput_TouchMovedEvent* evt);

static int32_t previousItem(int32_t size, int32_t idx)
{
    int32_t prevIdx = idx;
    
    if(prevIdx == 0)
        prevIdx = size - 1;
    else
        prevIdx--;
    
    return prevIdx;
}

static int32_t nextItem(int32_t size, int32_t idx)
{
    int32_t nextIdx = idx;
    
    if(nextIdx + 1 == size)
        nextIdx = 0;
    else
        nextIdx++;
    
    return nextIdx;
}

static void calculateTopItem(laListWheelWidget* whl)
{
    int32_t idx;
    int32_t item = whl->selectedItem;
    int32_t count = (whl->visibleItems / 2) + 1;
    
    whl->topItem = 0;
    
    if(whl->items.size == 0)
        return;
    
    for(idx = 0; idx < count; idx++)
        item = previousItem(whl->items.size, item);
        
    whl->topItem = item;
}

static int32_t calculateSelectedItem(laListWheelWidget* whl)
{
    int32_t idx;
    int32_t item = whl->topItem;
    int32_t count = (whl->visibleItems / 2) + 1;
    
    if(whl->items.size == 0)
        return item;
    
    for(idx = 0; idx < count; idx++)
        item = nextItem(whl->items.size, item);
        
    // account for spinning close to another value but not over it
    if(whl->rotation > 0 && whl->rotation > whl->cycleDistance / 2)
        item = previousItem(whl->items.size, item);
    else if(whl->rotation < 0 && whl->rotation < -whl->cycleDistance / 2)
        item = nextItem(whl->items.size, item);
        
    return item;
}

//int32_t last;

#if 0
static void invalidateContents(laListWheelWidget* whl)
{
    uint32_t idx;
    int32_t y = -1;
    GFX_Rect itemRect, widgetRect, clipRect;
    laLayer* layer;
    laListWheelItem* item;
    
    if(whl->items.size == 0)
        return;
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)whl);
    layer = laUtils_GetLayer((laWidget*)whl);
    idx = whl->topItem;
    
    for(y = -1; y < whl->visibleItems + 1; y++)
    {
        //if(y != 2)
        //    continue;
            
        item = whl->items.values[idx];
        
        if(item->icon != NULL)
        {
            _laListWheelWidget_GetItemIconRect(whl, idx, y, &itemRect, &clipRect);
            
            if(GFX_RectIntersects(&itemRect, &widgetRect) == GFX_TRUE)
            {
                GFX_RectClip(&itemRect, &widgetRect, &clipRect);
                
                laLayer_AddDamageRect(layer, &clipRect, GFX_FALSE);
                
                last = clipRect.y;
                
                if(last == clipRect.y)
                {
                    last = last;
                }
                
                /*printf("damaging %i, %i, %i, %i\n", clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);*/
            }
        }
        
        if(laString_IsEmpty(&item->string) == GFX_FALSE)
        {
            _laListWheelWidget_GetItemTextRect(whl, idx, y, &clipRect, &itemRect);
            
            if(GFX_RectIntersects(&itemRect, &widgetRect) == GFX_TRUE)
            {
                GFX_RectClip(&itemRect, &widgetRect, &clipRect);
                
                laLayer_AddDamageRect(layer, &clipRect, GFX_FALSE);
            }
        }
      
        idx++;
          
        if(idx >= whl->items.size)
            idx = 0;
    } 
}
#endif

static void languageChanging(laListWheelWidget* whl)
{
    int32_t i;
    uint32_t item;
    laString* str;
    item = whl->topItem;
    
    for(i = 0; i < whl->visibleItems; i++)
    {
        str = whl->items.values[item];
        
        if(laString_IsEmpty(str) == LA_FALSE)
        {
            laWidget_Invalidate((laWidget*)whl);
            
            return;
        }
        
        item = nextItem(whl->items.size, item);
    }
}

void _laListWheelWidget_Constructor(laListWheelWidget* whl)
{
    _laWidget_Constructor((laWidget*)whl);
    
    whl->widget.destructor = (laWidget_Destructor_FnPtr)&_laListWheelWidget_Destructor;

    whl->widget.languageChangeEvent = (laWidget_LanguageChangingEvent_FnPtr)&languageChanging;

    whl->widget.type = LA_WIDGET_LISTWHEEL;

    // override base class methods
    whl->widget.update = (laWidget_Update_FnPtr)&update;
    whl->widget.paint = (laWidget_Paint_FnPtr)&_laListWheelWidget_Paint;
    whl->widget.resized = (laWidget_Resized_FnPtr)&resized;

    whl->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&touchDown;
    whl->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&touchUp;
    whl->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&touchMoved;

    whl->widget.rect.width = DEFAULT_WIDTH;
    whl->widget.rect.height = DEFAULT_HEIGHT;

    whl->widget.borderType = LA_WIDGET_BORDER_BEVEL;
    whl->widget.backgroundType = LA_WIDGET_BACKGROUND_FILL;
    
    whl->borderTypeCache = whl->widget.borderType;
    whl->backgroundTypeCache = whl->widget.backgroundType;
    //whl->allowEmpty = LA_TRUE;
    

    //laString_Initialize(&whl->text);
    
    laArray_Create(&whl->items);
    
    whl->halign = LA_HALIGN_CENTER;
    whl->iconMargin = DEFAULT_MARGIN;
    whl->visibleItems = DEFAULT_VISIBLE_COUNT;
    
    calculateTopItem(whl);
    //whl->scrollBarWidth = DEFAULT_SCROLL_WIDTH;
    
    whl->showIndicators = LA_TRUE;
    whl->indicatorArea = DEFAULT_INDICATOR_AREA;
    whl->shaded = LA_TRUE;
    
    whl->cycleDistance = GFX_DivideRounding(whl->widget.rect.height, whl->visibleItems - 1);
    //whl->cycleSteps = DEFAULT_CYCLE_STEPS;
    
    whl->minFlickDelta = DEFAULT_DRAG_DELTA;
    whl->rotationTick = DEFAULT_ROTATION_TICK;
    whl->momentumFalloff = DEFAULT_MOMENTUM_FALLOFF;
    whl->stillTouching = LA_FALSE;
    whl->autoHideWheel = LA_FALSE;
    whl->hideWheel = LA_FALSE;
    
    whl->indicatorFill = LA_LISTWHEEL_INDICATOR_FILL_SOLID;
    whl->zoomEffects = LA_LISTWHEEL_ZOOM_EFFECT_NONE;
}

void _laListWheelWidget_Destructor(laListWheelWidget* whl)
{
    // destroy all items
    laListWheelWidget_RemoveAllItems(whl);
    
    // free any existing memory reader
    if(whl->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(whl->reader);
        
        whl->reader = NULL;
    }

    _laWidget_Destructor((laWidget*)whl);
}

laListWheelWidget* laListWheelWidget_New()
{
    laListWheelWidget* whl = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    whl = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laListWheelWidget));

    _laListWheelWidget_Constructor(whl);

    return whl;
}

#if 0
static void setRotation(laListWheelWidget* whl, int32_t rot)
{
    uint32_t rem, dist;
    
    whl->rotation = rot;
    
    //printf("y=%i, rot=%i\n", whl->touchY, whl->rotation);
    
    if(rot == 0)
        return;
    
    rem = GFX_AbsoluteValue(whl->rotation);
    dist = (uint32_t)whl->cycleDistance / 2;
    
    if(rem < dist)
        return;
       
    // adjust top item as necessary
    if(rot < 0)
    {
        //printf("old top item: %i\n", whl->topItem);
        whl->topItem = nextItem(whl->items.size, whl->topItem);
        //printf("new top item: %i\n", whl->topItem);
        
        whl->touchY -= whl->cycleDistance;
            
        whl->rotation += whl->cycleDistance;
        
        //printf("adj %i, %i\n", whl->rotation, whl->touchY);
    }
    else
    {
        //printf("old top item: %i\n", whl->topItem);
        whl->topItem = previousItem(whl->items.size, whl->topItem);
        //printf("new top item: %i\n", whl->topItem);
        
        whl->touchY += whl->cycleDistance;
            
        whl->rotation -= whl->cycleDistance;
        
        //printf("adj %i, %i\n", whl->rotation, whl->touchY);
    } 
}
#endif

static void adjustRotation(laListWheelWidget* whl, int32_t rot)
{
    uint32_t rem;
    uint32_t i;
    
    //printf("rot: %i\n", rot);
    
    whl->rotation += rot;
    
    rem = GFX_AbsoluteValue(whl->rotation) / whl->cycleDistance;
    
    if(rem == 0 || rot == 0)
        return;
    
    // adjust top item as necessary
    if(rot < 0)
    {
        for(i = 0; i < rem; i++)
        {
            //printf("before: %i, %i", whl->topItem, whl->rotation);
            
            whl->topItem = nextItem(whl->items.size, whl->topItem);
            //whl->touchY -= (whl->cycleDistance * 2);
            
            whl->rotation += (whl->cycleDistance);
            
            //printf(" after: %i, %i\n", whl->topItem, whl->rotation);
            
            //if(whl->rotation > 1000)
            //    whl->rotation = whl->rotation;
        }
    }
    else
    {
        for(i = 0; i < rem; i++)
        {
            whl->topItem = previousItem(whl->items.size, whl->topItem);
            //whl->touchY += rem * (whl->cycleDistance * 2);
            
            whl->rotation -= (whl->cycleDistance);
        }
    } 
}

static void snapRotation(laListWheelWidget* whl)
{
    int32_t selectedItem;
    
    selectedItem = calculateSelectedItem(whl);
        
    whl->rotation = 0;
    whl->momentum = 0;
           
    // set selected item and issue callback
    if(selectedItem != whl->selectedItem)
    {
        whl->selectedItem = selectedItem;
        
        calculateTopItem(whl);
        
        if(whl->cb != NULL)
            whl->cb(whl, whl->selectedItem);
    }
    
    // reset draw state on invalidate in case the wheel is partially redrawn
    //whl->widget.drawState = LA_WIDGET_DRAW_STATE_READY;
        
    //printf("snap, %i, %i, %i\n", whl->rotation, whl->momentum, whl->paintState.offs);
}

static void updateRotation(laListWheelWidget* whl, uint32_t ms)
{
    uint32_t absMomentum = GFX_AbsoluteValue(whl->momentum);
    uint32_t sig, dec;
    uint32_t fallOff;
    
    whl->rotationCounter += ms;
    
    //if(whl->rotationCounter >= whl->rotationTick)
    //{
        //invalidateContents(whl);
        
        whl->rotationCounter = 0;
        
		// if the user has a finger on the wheel, we should increase the friction
        if (whl->stillTouching == LA_TRUE)
        {
            fallOff = whl->momentumFalloff * 4;
        }
        else
        {
            fallOff = whl->momentumFalloff;
        }
        
        if (fallOff > 100)
        {
            fallOff = 100;
        }
        
        GFX_PercentOfDec(absMomentum,
                         100 - fallOff,
                         &sig,
                         &dec);
                         
        //printf("mom: %i\n", whl->momentum);
                         
        if(whl->momentum < 0)
        {
            whl->momentum = -(int32_t)sig;
            
            adjustRotation(whl, whl->momentum);
        }
        else
        {
            whl->momentum = sig;
            
            adjustRotation(whl, whl->momentum);
        }
        
        // below reasonable threshold, stop rotation
        if(whl->momentum == 0
           && whl->stillTouching == LA_FALSE)
        {
            whl->snapPending = LA_TRUE;
            
            return;
        }
            
        //printf("mom: %i.%i\n", whl->momentum, whl->momentumDec);
        
        //invalidateContents(whl);
        
        laWidget_Invalidate((laWidget*)whl);
    //}
    
    
    
    //tickPer = tickPer;
}

static laWidgetUpdateState update(laListWheelWidget* whl, uint32_t ms)
{
    laWidgetUpdateState retval = LA_WIDGET_UPDATE_STATE_DONE;
    
    if(whl->momentum != 0)
    {
        retval = LA_WIDGET_UPDATE_STATE_PENDING;
        updateRotation(whl, ms);
        
        if (whl->autoHideWheel == LA_TRUE &&
            whl->hideWheel == LA_TRUE)
        {
            //show wheel
            whl->hideWheel = LA_FALSE;
        
            laWidget_SetBorderType((laWidget*) whl, whl->borderTypeCache);
            laWidget_SetBackgroundType((laWidget*) whl, whl->backgroundTypeCache);                
        }
    }
        
    if(whl->snapPending == LA_TRUE)
    {
        //invalidateContents(whl);
        
        snapRotation(whl);
        
        //invalidateContents(whl);
        
        laWidget_Invalidate((laWidget*)whl);
        
        whl->snapPending = LA_FALSE;
        
        retval = LA_WIDGET_UPDATE_STATE_DONE;
        
        if (whl->autoHideWheel == LA_TRUE &&
            whl->hideWheel == LA_FALSE &&
            whl->stillTouching == LA_FALSE)
        {
            //Hide wheel
            whl->hideWheel = LA_TRUE;

            whl->borderTypeCache = laWidget_GetBorderType((laWidget*) whl);
            whl->backgroundTypeCache = laWidget_GetBackgroundType((laWidget*) whl);

            laWidget_SetBorderType((laWidget*) whl, LA_WIDGET_BORDER_NONE);
            laWidget_SetBackgroundType((laWidget*) whl, LA_WIDGET_BACKGROUND_NONE);           
        }
    }
    
    return retval;
}

static void resized(laListWheelWidget* whl)
{
    whl->cycleDistance = GFX_DivideRounding(whl->widget.rect.height, whl->visibleItems - 1);
}

static void touchDown(laListWheelWidget* whl, laInput_TouchDownEvent* evt)
{
    whl->touchY = evt->y;
    whl->lastTouchY = evt->y;
    whl->firstTouchY = evt->y;
    
    //printf("down: %i\n", whl->touchY);
    
    whl->stillTouching = LA_TRUE;
    evt->event.accepted = LA_TRUE;
    
    if (whl->autoHideWheel == LA_TRUE &&
        whl->hideWheel == LA_TRUE)    
    {
        //show wheel
        whl->hideWheel = LA_FALSE;
        laWidget_SetBorderType((laWidget*) whl, whl->borderTypeCache);
        laWidget_SetBackgroundType((laWidget*) whl, whl->backgroundTypeCache);
    }
    
    laWidget_Invalidate((laWidget*)whl);
}

static void touchUp(laListWheelWidget* whl, laInput_TouchUpEvent* evt)
{
    if(whl->momentum == 0 || GFX_AbsoluteValue(whl->firstTouchY - whl->lastTouchY) < 10)
        whl->snapPending = LA_TRUE;
    
    whl->stillTouching = LA_FALSE;
    evt->event.accepted = LA_TRUE;
    
    if (whl->autoHideWheel == LA_TRUE
        && whl->hideWheel == LA_FALSE
        && whl->snapPending == LA_FALSE
       )
    {
        //Hide wheel
        whl->hideWheel = LA_TRUE;
        
        whl->borderTypeCache = laWidget_GetBorderType((laWidget*) whl);
        whl->backgroundTypeCache = laWidget_GetBackgroundType((laWidget*) whl);
        
        laWidget_SetBorderType((laWidget*) whl, LA_WIDGET_BORDER_NONE);
        laWidget_SetBackgroundType((laWidget*) whl, LA_WIDGET_BACKGROUND_NONE);
    }
    
    laWidget_Invalidate((laWidget*)whl);
}

static void touchMoved(laListWheelWidget* whl, laInput_TouchMovedEvent* evt)
{
    GFX_Rect rect;
    
    laUtils_RectToLayerSpace((laWidget*)whl, &rect);
    
    //printf("%i, %i, %i\n", evt->y, whl->touchY, evt->y - whl->touchY);
    //printf("%i\n", GFX_AbsoluteValue(evt->y - whl->lastTouchY));
    
    //invalidateContents(whl);
    
    if (evt->y < whl->lastTouchY)
    {
        if(whl->lastTouchY - evt->y > whl->minFlickDelta)
        {
            whl->momentum -= (whl->lastTouchY - evt->y) / 2;

            //printf("drag: %i\n", whl->lastTouchY - evt->y);

            if(whl->momentum < -MAX_MOMENTUM)
                whl->momentum = -MAX_MOMENTUM;

            //printf("plus %i\n", whl->momentum);
        }
        else
            whl->momentum = -(whl->lastTouchY - evt->y);
    }
    else if (evt->y > whl->lastTouchY)
    {
        if(evt->y - whl->lastTouchY > whl->minFlickDelta)
        {
            whl->momentum += (evt->y - whl->lastTouchY) / 2;

            //printf("drag: %i\n", evt->y - whl->lastTouchY);

            if(whl->momentum > MAX_MOMENTUM)
                whl->momentum = MAX_MOMENTUM;

            //printf("minus %i\n", whl->momentum);
        }
        else
            whl->momentum = evt->y - whl->lastTouchY;
    }    
    
    if (whl->autoHideWheel == LA_TRUE
        && whl->hideWheel == LA_TRUE)
    {
        whl->hideWheel = LA_FALSE;
        
        laWidget_SetBorderType((laWidget*) whl, whl->borderTypeCache);
        laWidget_SetBackgroundType((laWidget*) whl, whl->backgroundTypeCache);
    }
    
    laWidget_Invalidate((laWidget*)whl);
    
    whl->lastTouchY = evt->y;
    
	evt->event.accepted = LA_TRUE;
}

laHAlignment laListWheelWidget_GetAlignment(laListWheelWidget* whl)
{
    if(whl == NULL)
        return 0;

    return whl->halign;
}

laResult laListWheelWidget_SetAlignment(laListWheelWidget* whl, laHAlignment align)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    if(whl->halign == align)
        return LA_SUCCESS;

    //invalidateContents(whl);
    
    whl->halign = align;

    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

uint32_t laListWheelWidget_GetVisibleItemCount(laListWheelWidget* whl)
{
    if(whl == NULL)
        return 0;

    return whl->visibleItems;
}

laResult laListWheelWidget_SetVisibleItemCount(laListWheelWidget* whl, uint32_t cnt)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    if(whl->visibleItems == cnt)
        return LA_SUCCESS;
    
    //invalidateContents(whl);

    whl->visibleItems = cnt;
    
    whl->cycleDistance = GFX_DivideRounding(whl->widget.rect.height, whl->visibleItems - 1);

    calculateTopItem(whl);

    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

laRelativePosition laListWheelWidget_GetIconPosition(laListWheelWidget* whl)
{
    if(whl == NULL)
        return 0;

    return whl->iconPos;
}
                                               
laResult laListWheelWidget_SetIconPosition(laListWheelWidget* whl,
                                      laRelativePosition pos)
{
    if(whl == NULL ||
       (pos != LA_RELATIVE_POSITION_LEFTOF && pos != LA_RELATIVE_POSITION_RIGHTOF))
        return LA_FAILURE;
        
    if(whl->iconPos == pos)
        return LA_SUCCESS;

    //invalidateContents(whl);

    whl->iconPos = pos;

    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

uint32_t laListWheelWidget_GetIconMargin(laListWheelWidget* whl)
{
    if(whl == NULL)
        return 0;

    return whl->iconMargin;
}
                                               
laResult laListWheelWidget_SetIconMargin(laListWheelWidget* whl, uint32_t mg)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    if(whl->iconMargin == mg)
        return LA_SUCCESS;

    //invalidateContents(whl);

    whl->iconMargin = mg;

   // invalidateContents(whl);
   
   laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

laBool laListWheelWidget_GetShowIndicators(laListWheelWidget* whl)
{
    if(whl == NULL)
        return LA_FALSE;
        
    return whl->showIndicators;
}

laResult laListWheelWidget_SetShowIndicators(laListWheelWidget* whl, laBool b)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    if(whl->showIndicators == b)
        return LA_SUCCESS;

    //invalidateContents(whl);

    whl->showIndicators = b;

    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

uint32_t laListWheelWidget_GetIndicatorArea(laListWheelWidget* whl)
{
    if(whl == NULL)
        return LA_FALSE;
        
    return whl->indicatorArea;
}

laResult laListWheelWidget_SetIndicatorArea(laListWheelWidget* whl, uint32_t area)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    if(whl->indicatorArea == area)
        return LA_SUCCESS;

// todo
    //invalidateContents(whl);

    whl->indicatorArea = area;

    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

laBool laListWheelWidget_GetShaded(laListWheelWidget* whl)
{
    if(whl == NULL)
        return LA_FALSE;
        
    return whl->shaded;
}

laResult laListWheelWidget_SetShaded(laListWheelWidget* whl, laBool b)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    if(whl->shaded == b)
        return LA_SUCCESS;

    whl->shaded = b;

    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

uint32_t laListWheelWidget_GetFlickInitSpeed(laListWheelWidget* whl)
{
    if(whl == NULL)
        return LA_FALSE;
        
    return whl->minFlickDelta;
}

laResult laListWheelWidget_SetFlickInitSpeed(laListWheelWidget* whl, uint32_t speed)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    whl->minFlickDelta = speed;
    
    return LA_SUCCESS;
}

uint32_t laListWheelWidget_GetMaxMomentum(laListWheelWidget* whl)
{
    if(whl == NULL)
        return LA_FALSE;
        
    return whl->maxMomentum;
}

laResult laListWheelWidget_SetMaxMomentum(laListWheelWidget* whl, uint32_t max)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    whl->maxMomentum = max;
    
    return LA_SUCCESS;
}

uint32_t laListWheelWidget_GetMomentumFalloffRate(laListWheelWidget* whl)
{
    if(whl == NULL)
        return LA_FALSE;
        
    return whl->momentumFalloff;
}

laResult laListWheelWidget_SetMomentumFalloffRate(laListWheelWidget* whl, uint32_t percent)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    whl->momentumFalloff = percent;
    
    return LA_SUCCESS;
}

uint32_t laListWheelWidget_GetRotationUpdateRate(laListWheelWidget* whl)
{
    if(whl == NULL)
        return LA_FALSE;
        
    return whl->rotationTick;
}

laResult laListWheelWidget_SetRotationUpdateRate(laListWheelWidget* whl, uint32_t ms)
{
    if(whl == NULL)
        return LA_FAILURE;
        
    whl->rotationTick = ms;
    
    return LA_SUCCESS;
}
                                                        
uint32_t laListWheelWidget_GetItemCount(laListWheelWidget* whl)
{
    if(whl == NULL)
        return 0;

    return whl->items.size;
}

uint32_t laListWheelWidget_AppendItem(laListWheelWidget* whl)
{
    laListWheelItem* item;
    
    if(whl == NULL)
        return -1;
        
    item = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laListWheelItem));
    
    if(item == NULL)
        return -1;
        
    //invalidateContents(whl);
        
    laString_Initialize(&item->string);
    
    laArray_PushBack(&whl->items, item);
    
    if(whl->items.size == 1)
        whl->selectedItem = 0;
        
    calculateTopItem(whl);
    
    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return whl->items.size - 1;
}

uint32_t laListWheelWidget_InsertItem(laListWheelWidget* whl, uint32_t idx)
{
    laListWheelItem* item;
    
    if(whl == NULL)
        return -1;
        
    item = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laListWheelItem));
    
    if(item == NULL)
        return -1;
        
    laString_Initialize(&item->string);
    
    //invalidateContents(whl);
    
    laArray_InsertAt(&whl->items, idx, item);
    
    if(whl->items.size == 1)
        whl->selectedItem = 0;
        
    calculateTopItem(whl);
    
    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return idx;
}

laResult laListWheelWidget_RemoveItem(laListWheelWidget* whl, uint32_t idx)
{
    laResult res;
    laListWheelItem* item;
    
    if(whl == NULL)
        return LA_FAILURE;
        
    item = whl->items.values[idx];
    
    if(item == NULL)
        return LA_FAILURE;
        
    //if(idx < whl->items.size)        
    //    invalidateContents(whl);
        
    res = laArray_RemoveAt(&whl->items, idx);
    
    if(whl->items.size == 0)
        whl->selectedItem = -1;
    
    laString_Destroy(&item->string);
    
    laContext_GetActive()->memIntf.heap.free(item);
    
    calculateTopItem(whl);

    //if(res == LA_SUCCESS)
    //    invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return res;
}

laResult laListWheelWidget_RemoveAllItems(laListWheelWidget* whl)
{
    laListWheelItem* item;
    
    if(whl == NULL)
        return LA_FAILURE;
        
    while(whl->items.size > 0)
    {
        item = whl->items.values[0];
    
        if(item != NULL)
        {
            laString_Destroy(&item->string);
        
            laContext_GetActive()->memIntf.heap.free(item);
        }
        
        laArray_RemoveAt(&whl->items, 0);
    }
    
    laArray_Destroy(&whl->items);
    
    whl->selectedItem = -1;
    
    calculateTopItem(whl);
        
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

int32_t laListWheelWidget_GetSelectedItem(laListWheelWidget* whl)
{
    if(whl == NULL)
        return -1;
        
    return whl->selectedItem;
}

laResult laListWheelWidget_SetSelectedItem(laListWheelWidget* whl, uint32_t idx)
{
    if(whl == NULL || idx >= whl->items.size)
        return LA_FAILURE;
        
    if(whl->selectedItem == idx)
        return LA_SUCCESS;
        
    //invalidateContents(whl);
        
    whl->selectedItem = idx;
    
    if(whl->cb != NULL)
        whl->cb(whl, idx);

    calculateTopItem(whl);
    
    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
        
    return LA_SUCCESS;
}

laResult laListWheelWidget_SelectPreviousItem(laListWheelWidget* whl)
{
    uint32_t idx;
    
    if(whl == NULL || whl->items.size == 0)
        return LA_FAILURE;
        
    //invalidateContents(whl);
        
    idx = whl->selectedItem;
    
    if(idx == 0)
        idx = whl->items.size - 1;
    else
        idx -= 1; 
        
    if(whl->selectedItem == idx)
        return LA_SUCCESS;
        
    whl->selectedItem = idx;
    
    if(whl->cb != NULL)
        whl->cb(whl, idx);

    calculateTopItem(whl);
    
    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
        
    return LA_SUCCESS;
}

laResult laListWheelWidget_SelectNextItem(laListWheelWidget* whl)
{
    uint32_t idx;
    
    if(whl == NULL || whl->items.size == 0)
        return LA_FAILURE;
        
    //invalidateContents(whl);
        
    idx = whl->selectedItem;
    
    if(idx == whl->items.size - 1)
        idx = 0;
    else
        idx += 1;  
        
    if(whl->selectedItem == idx)
        return LA_SUCCESS;
        
    whl->selectedItem = idx;
    
    if(whl->cb != NULL)
        whl->cb(whl, idx);
    
    calculateTopItem(whl);
    
    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
        
    return LA_SUCCESS;
}
                                                 
laResult laListWheelWidget_GetItemText(laListWheelWidget* whl,
                                       uint32_t idx,
                                       laString* str)
{
    laListWheelItem* item;
    
    if(whl == NULL || idx >= whl->items.size)
        return LA_FAILURE;
        
    item = whl->items.values[idx];
    
    laString_Copy(str, &item->string);
    
    return LA_SUCCESS;
}
                                               
laResult laListWheelWidget_SetItemText(laListWheelWidget* whl,
                                       uint32_t idx,
                                       laString str)
{
    laListWheelItem* item;
    
    if(whl == NULL || idx >= whl->items.size)
        return LA_FAILURE;
        
    //invalidateContents(whl);
       
    item = whl->items.values[idx];
    
    laString_Copy(&item->string, &str);
    
    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}
                                               
GFXU_ImageAsset* laListWheelWidget_GetItemIcon(laListWheelWidget* whl,
                                               uint32_t idx)
{
    laListWheelItem* item;
    
    if(whl == NULL || idx >= whl->items.size)
        return NULL;
        
    item = whl->items.values[idx];
        
    return item->icon;
}
                                                              
laResult laListWheelWidget_SetItemIcon(laListWheelWidget* whl,
                                       uint32_t idx,
                                       GFXU_ImageAsset* img)
{
    laListWheelItem* item;
    
    if(whl == NULL || idx >= whl->items.size)
        return LA_FAILURE;
        
    //invalidateContents(whl);
        
    item = whl->items.values[idx];
        
    item->icon = img;
    
    //invalidateContents(whl);
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

laListWheelWidget_SelectedItemChangedEvent laListWheelWidget_GetSelectedItemChangedEventCallback(laListWheelWidget* whl)
{
    if(whl == NULL)
        return NULL;

    return whl->cb;
}

laResult laListWheelWidget_SetSelectedItemChangedEventCallback(laListWheelWidget* whl,
                                                               laListWheelWidget_SelectedItemChangedEvent cb)
{
    if(whl == NULL || whl->cb == cb)
        return LA_FAILURE;

    whl->cb = cb;
    
    return LA_SUCCESS;
}

laListWheelIndicatorFill laListWheelWidget_GetIndicatorFill(laListWheelWidget* whl)
{
    if (whl == NULL)
        return LA_LISTWHEEL_INDICATOR_FILL_NONE;
    
    return whl->indicatorFill;
}

laResult laListWheelWidget_SetIndicatorFill(laListWheelWidget* whl, laListWheelIndicatorFill fill)
{
    if (whl == NULL || fill > LA_LISTWHEEL_INDICATOR_FILL_GRADIENT)
        return LA_FAILURE;
    
    if (whl->indicatorFill == fill)
        return LA_SUCCESS;
    
    whl->indicatorFill = fill;
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

laListWheelZoomEffects laListWheelWidget_GetZoomEffects(laListWheelWidget* whl)
{
    if (whl == NULL)
        return LA_LISTWHEEL_INDICATOR_FILL_NONE;
    
    return whl->zoomEffects;
}

laResult laListWheelWidget_SetZoomEffects(laListWheelWidget* whl, laListWheelZoomEffects zoomEffects)
{
    if (whl == NULL || zoomEffects > LA_LISTWHEEL_ZOOM_EFFECT_FIXED_SCALE)
        return LA_FAILURE;
    
    if (whl->zoomEffects == zoomEffects)
        return LA_SUCCESS;
    
    whl->zoomEffects = zoomEffects;
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

laBool laListWheelWidget_GetAutoHideWheel(laListWheelWidget* whl)
{
    if (whl == NULL)
        return LA_LISTWHEEL_INDICATOR_FILL_NONE;
    
    return whl->autoHideWheel;
}

laResult laListWheelWidget_SetAutoHideWheel(laListWheelWidget* whl, laBool autoHide)
{
    if (whl == NULL)
        return LA_FAILURE;
    
    if (whl->autoHideWheel == autoHide)
        return LA_SUCCESS;
    
    whl->autoHideWheel = autoHide;
    
    whl->hideWheel = autoHide;
    
    whl->borderTypeCache = laWidget_GetBorderType((laWidget*) whl);
    whl->backgroundTypeCache = laWidget_GetBackgroundType((laWidget*) whl);
    
    if (whl->autoHideWheel == LA_TRUE)
    {
        laWidget_SetBorderType((laWidget*) whl, LA_WIDGET_BORDER_NONE);
        laWidget_SetBackgroundType((laWidget*) whl, LA_WIDGET_BACKGROUND_NONE);   
    }
    
    laWidget_Invalidate((laWidget*)whl);
    
    return LA_SUCCESS;
}

#endif // LA_LISTWHEEL_WIDGET_ENABLED
