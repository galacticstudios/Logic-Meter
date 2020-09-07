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


#include "gfx/libaria/inc/libaria_widget_scrollbar.h"

#if LA_SCROLLBAR_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_utils.h"

#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_util.h"

#define DEFAULT_WIDTH           20
#define DEFAULT_HEIGHT          100

#define DEFAULT_MIN             0
#define DEFAULT_MAX             100
#define DEFAULT_EXTENT          10
#define DEFAULT_VALUE           0

#define DEFAULT_STEP            1

#define MIN_HANDLE_HEIGHT       16

static void invalidateUpLeftButton(laScrollBarWidget* bar)
{
    GFX_Rect rect;
    
    _laScrollBar_GetUpLeftButtonRect(bar, &rect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)bar),
                          &rect,
                          GFX_FALSE);
}

static void invalidateDownRightButton(laScrollBarWidget* bar)
{
    GFX_Rect rect;
    
    _laScrollBar_GetDownRightButtonRect(bar, &rect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)bar),
                          &rect,
                          GFX_FALSE);
}

static void invalidateScrollArea(laScrollBarWidget* bar)
{
    GFX_Rect rect;
    
    _laScrollBar_GetScrollAreaRect(bar, &rect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)bar),
                          &rect,
                          GFX_FALSE);
}

static void invalidateHandleRect(laScrollBarWidget* bar)
{
    GFX_Rect rect;
    
    _laScrollBar_GetHandleRect(bar, &rect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)bar),
                          &rect,
                          GFX_FALSE);
}

uint32_t _laScrollWidget_GetExtent(laScrollBarWidget* bar)
{
    uint32_t extent = bar->extent;
    
    if(extent >= bar->max)
        extent = bar->max - 1;
        
    return extent;
}

uint32_t _laScrollBar_GetPercentFromPoint(laScrollBarWidget* bar, GFX_Point* pnt)
{
    GFX_Rect scrollRect, handleRect;
    int32_t val;
    int32_t size;
    
    _laScrollBar_GetScrollAreaRect(bar, &scrollRect);
    _laScrollBar_GetHandleRect(bar, &handleRect);
    
    if(bar->alignment == LA_SCROLLBAR_ORIENT_VERTICAL)
    {
        size = scrollRect.height - handleRect.height;
                
        // translate rect and point in to rect space
        val = pnt->y - scrollRect.y;
        scrollRect.y = 0;
        
        if(val <= 0)
            return 0;
        else if(val >= size)
            return 100;
        
        return GFX_PercentWholeRounded(val, size);
    }
    else
    {
        size = scrollRect.width - handleRect.width;
        
        // translate rect and point in to rect space
        val = pnt->x - scrollRect.x;
        scrollRect.x = 0;
        
        //scrollRect.width -= _laScrollWidget_GetExtent(bar);
        
        if(val <= 0)
            return 0;
        else if(val >= size)
            return 100;
        
        return GFX_PercentWholeRounded(val, size);
    }
}

uint32_t _laScrollBar_GetValueFromPercent(laScrollBarWidget* bar, uint32_t per)
{
    int32_t max;
    
    if(per == 0)
        return DEFAULT_MIN;
    else if(per == 100)
        return bar->max;
    
    // zero based range
    max = bar->max;
    
    return GFX_PercentOf(max, per);
}

void _laScrollBarWidget_Constructor(laScrollBarWidget* bar)
{
    _laWidget_Constructor((laWidget*)bar);
    
    bar->widget.destructor = (laWidget_Destructor_FnPtr)&_laScrollBarWidget_Destructor;

    bar->widget.type = LA_WIDGET_SCROLLBAR;

    // override base class methods
    bar->widget.paint = (laWidget_Paint_FnPtr)&_laScrollBarWidget_Paint;

    bar->widget.rect.width = DEFAULT_WIDTH;
    bar->widget.rect.height = DEFAULT_HEIGHT;

    bar->state = LA_SCROLLBAR_STATE_NONE;

    bar->widget.borderType = LA_WIDGET_BORDER_NONE;

    bar->alignment = LA_SCROLLBAR_ORIENT_VERTICAL;
    
    //bar->min = DEFAULT_MIN;
    bar->max = DEFAULT_MAX;
    bar->extent = DEFAULT_EXTENT;
    bar->value = DEFAULT_VALUE;
    bar->step = DEFAULT_STEP;

    bar->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&_laScrollBarWidget_TouchDownEvent;
    bar->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&_laScrollBarWidget_TouchUpEvent;
    bar->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&_laScrollBarWidget_TouchMovedEvent;

    bar->widget.invalidateBorderAreas = (laWidget_InvalidateBorderAreas_FnPtr)&_laScrollBarWidget_InvalidateBorderAreas;
}

void _laScrollBarWidget_Destructor(laScrollBarWidget* bar)
{
    _laWidget_Destructor((laWidget*)bar);
}

laScrollBarWidget* laScrollBarWidget_New()
{
    laScrollBarWidget* bar = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    bar = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laScrollBarWidget));
    
    _laScrollBarWidget_Constructor(bar);

    return bar;
}

laScrollBarOrientation laScrollBarWidget_GetOrientation(laScrollBarWidget* bar)
{
    if(bar == NULL)
        return 0;

    return bar->alignment;
}

laResult laScrollBarWidget_SetOrientation(laScrollBarWidget* bar,
                                          laScrollBarOrientation align,
                                          laBool swapDimensions)
{
    uint32_t t;
    
    if(bar == NULL || bar->alignment == align)
        return LA_FAILURE;
        
    if(bar->alignment == align)
        return LA_SUCCESS;

    bar->alignment = align;
    
    // reverse dimensions
    if(swapDimensions == LA_TRUE)
    {
        t = bar->widget.rect.width;
        bar->widget.rect.width = bar->widget.rect.height;
        bar->widget.rect.height = t;
    }
    
    laWidget_Invalidate((laWidget*)bar);
    
    return LA_SUCCESS;
}

laScrollBarWidget_ValueChangedEvent laScrollBarWidget_GetValueChangedEventCallback(laScrollBarWidget* bar)
{
    if(bar == NULL)
        return NULL;

    return bar->valueChangedEvent;
}

uint32_t laScrollBarWidget_GetMaxinumValue(laScrollBarWidget* bar)
{
    if(bar == NULL)
        return 0;
        
    return bar->max;
}

laResult laScrollBarWidget_SetMaximumValue(laScrollBarWidget* bar,
                                           uint32_t val)
{
    if(bar == NULL)
        return LA_FAILURE;
        
    if(bar->max == val)
        return LA_SUCCESS;
        
    bar->max = val;
    
    laWidget_Invalidate((laWidget*)bar);
        
    return LA_SUCCESS;
}                                                 

uint32_t laScrollBarWidget_GetExtentValue(laScrollBarWidget* bar)
{
    if(bar == NULL)
        return 0;
        
    return bar->extent;
}

laResult laScrollBarWidget_SetExtentValue(laScrollBarWidget* bar,
                                          uint32_t val)
{
    if(bar == NULL)
        return LA_FAILURE;
        
    if(bar->extent == val)
        return LA_SUCCESS;
        
    bar->extent = val;
    
    laWidget_Invalidate((laWidget*)bar);
        
    return LA_SUCCESS;
}

uint32_t laScrollBarWidget_GetScrollPercentage(laScrollBarWidget* bar)
{
    if(bar == NULL)
        return 0;
        
    return GFX_PercentWholeRounded(bar->value, bar->max);
}

laResult laScrollBarWidget_SetScrollPercentage(laScrollBarWidget* bar,
                                               uint32_t val)
{
    if(bar == NULL)
        return LA_FAILURE;
        
    if(bar->value == val)
        return LA_SUCCESS;
        
    bar->value = val;
    
    if(bar->valueChangedEvent != NULL)
        bar->valueChangedEvent(bar);
    
    laWidget_Invalidate((laWidget*)bar);
        
    return LA_SUCCESS;
} 
                                                                                                               
uint32_t laScrollBarWidget_GetScrollValue(laScrollBarWidget* bar)
{
    if(bar == NULL)
        return 0;
        
    return bar->value;
}

laResult laScrollBarWidget_SetScrollValue(laScrollBarWidget* bar,
                                          uint32_t val) 
{
    if(bar == NULL)
        return LA_FAILURE;
        
    if(bar->value == val)
        return LA_SUCCESS;
        
    if(val > bar->max)
        val = bar->max;
    
    invalidateHandleRect(bar);
   
    bar->value = val;
    
    if(bar->valueChangedEvent != NULL)
        bar->valueChangedEvent(bar);
    
    invalidateHandleRect(bar);
        
    return LA_SUCCESS;
}

uint32_t laScrollBarWidget_GetStepSize(laScrollBarWidget* bar)
{
    if(bar == NULL)
        return 0;
        
    return bar->step;
}

laResult laScrollBarWidget_SetStepSize(laScrollBarWidget* bar,
                                       uint32_t val)
{
    if(bar == NULL)
        return LA_FAILURE;
        
    bar->step = val;
        
    return LA_SUCCESS;
}

laResult laScrollBarWidget_StepBackward(laScrollBarWidget* bar)
{
    if(bar == NULL || bar->value == DEFAULT_MIN)
        return LA_FAILURE;
        
    invalidateHandleRect(bar);
        
    if(bar->value - bar->step < DEFAULT_MIN ||
       bar->value - bar->step > bar->max)
    {
        bar->value = DEFAULT_MIN;
    }
    else
        bar->value -= bar->step;
        
    if(bar->valueChangedEvent != NULL)
        bar->valueChangedEvent(bar);
        
    //printf("%u\n", bar->value);
        
    invalidateHandleRect(bar);
        
    return LA_SUCCESS;
}

laResult laScrollBarWidget_StepForward(laScrollBarWidget* bar)
{
    if(bar == NULL || bar->value == bar->max)
        return LA_FAILURE;
        
    invalidateHandleRect(bar);
        
    if(bar->value + bar->step < DEFAULT_MIN ||
       bar->value + bar->step > bar->max)
    {
        bar->value = bar->max;
    }
    else
        bar->value += bar->step;
        
    if(bar->valueChangedEvent != NULL)
        bar->valueChangedEvent(bar);
        
    //printf("%u\n", bar->value);
        
    invalidateHandleRect(bar);
        
    return LA_SUCCESS;
}

laResult laScrollBarWidget_SetValueChangedEventCallback(laScrollBarWidget* bar,
                                                        laScrollBarWidget_ValueChangedEvent cb)
{
    if(bar == NULL || bar->valueChangedEvent == cb)
        return LA_FAILURE;

    bar->valueChangedEvent = cb;
    
    return LA_SUCCESS;
}

void _laScrollBarWidget_TouchDownEvent(laScrollBarWidget* bar,
                                       laInput_TouchDownEvent* evt)
{
    GFX_Rect rect;
    GFX_Point pnt;

    pnt.x = evt->x;
    pnt.y = evt->y;
    
    // already guaranteed to be inside widget rectangle, accept event
    evt->event.accepted = LA_TRUE;

    // was the up/left button pressed
    _laScrollBar_GetUpLeftButtonRect(bar, &rect);
    
    if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
    {
        bar->state = LA_SCROLLBAR_STATE_TOP_INSIDE;
        
        invalidateUpLeftButton(bar);
        
        return;
    }
    
    // was the up/left button pressed
    _laScrollBar_GetDownRightButtonRect(bar, &rect);
    
    if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
    {
        bar->state = LA_SCROLLBAR_STATE_BOTTOM_INSIDE;
        
        invalidateDownRightButton(bar);
        
        return;
    }
    
    // was the handle touched
    _laScrollBar_GetHandleRect(bar, &rect);
    
    if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
    {
        bar->state = LA_SCROLLBAR_STATE_HANDLE_DOWN;
        
        bar->handleDownOffset.x = evt->x - rect.x;
        bar->handleDownOffset.y = evt->y - rect.y;
        
        //printf("%i, %i\n", bar->handleDownOffset.x, bar->handleDownOffset.y); 
        
        //invalidateScrollArea(bar);
        
        return;
    }
}

void _laScrollBarWidget_TouchUpEvent(laScrollBarWidget* bar,
                                  laInput_TouchUpEvent* evt)
{
    GFX_Rect rect = bar->widget.rect;
    GFX_Point pnt;

    pnt.x = evt->x;
    pnt.y = evt->y;

    evt->event.accepted = LA_TRUE;
    
    if(bar->state == LA_SCROLLBAR_STATE_TOP_INSIDE)
    {
        _laScrollBar_GetUpLeftButtonRect(bar, &rect);
    
        if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
        {
            laScrollBarWidget_StepBackward(bar);
            
            //printf("bar up event\n");

            invalidateUpLeftButton(bar);
        }
    }
    else if(bar->state == LA_SCROLLBAR_STATE_BOTTOM_INSIDE)
    {
        _laScrollBar_GetDownRightButtonRect(bar, &rect);
    
        if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
        {
            laScrollBarWidget_StepForward(bar);
            
            //printf("bar down event\n");

            invalidateDownRightButton(bar);
        }
    }
    
    bar->state = LA_SCROLLBAR_STATE_NONE;
    
    invalidateScrollArea(bar);
}

void _laScrollBarWidget_TouchMovedEvent(laScrollBarWidget* bar,
                                     laInput_TouchMovedEvent* evt)
{
    GFX_Rect rect = bar->widget.rect;
    GFX_Point pnt;
    uint32_t percent;
    uint32_t val;

    pnt.x = evt->x;
    pnt.y = evt->y;

    evt->event.accepted = LA_TRUE;

    if(bar->state == LA_SCROLLBAR_STATE_TOP_PRESSED ||
       bar->state == LA_SCROLLBAR_STATE_TOP_INSIDE)
    {
        _laScrollBar_GetUpLeftButtonRect(bar, &rect);
    
        if(bar->state == LA_SCROLLBAR_STATE_TOP_INSIDE &&
           GFX_RectContainsPoint(&rect, &pnt) == GFX_FALSE)
        {
            bar->state = LA_SCROLLBAR_STATE_TOP_PRESSED;

            invalidateUpLeftButton(bar);
        }
        else if(bar->state == LA_SCROLLBAR_STATE_TOP_PRESSED &&
           GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
        {
            bar->state = LA_SCROLLBAR_STATE_TOP_INSIDE;

            invalidateDownRightButton(bar);
        }
    }
    else if(bar->state == LA_SCROLLBAR_STATE_BOTTOM_PRESSED ||
            bar->state == LA_SCROLLBAR_STATE_BOTTOM_INSIDE)
    {
        _laScrollBar_GetDownRightButtonRect(bar, &rect);
    
        if(bar->state == LA_SCROLLBAR_STATE_BOTTOM_INSIDE &&
           GFX_RectContainsPoint(&rect, &pnt) == GFX_FALSE)
        {
            bar->state = LA_SCROLLBAR_STATE_BOTTOM_PRESSED;

            laWidget_Invalidate((laWidget*)bar);
        }
        else if(bar->state == LA_SCROLLBAR_STATE_BOTTOM_PRESSED &&
           GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
        {
            bar->state = LA_SCROLLBAR_STATE_BOTTOM_INSIDE;

            invalidateScrollArea(bar);
        }
    }
    else if(bar->state == LA_SCROLLBAR_STATE_HANDLE_DOWN)
    {
        pnt.x = evt->x - bar->handleDownOffset.x;
        pnt.y = evt->y - bar->handleDownOffset.y;
        
        //printf("%i, %i\n", pnt.x, pnt.y);
        
        percent = _laScrollBar_GetPercentFromPoint(bar, &pnt);
        
        val = _laScrollBar_GetValueFromPercent(bar, percent);
        
        //printf("%u, %i\n", percent, bar->value);
        
        if(bar->value != val)
            laScrollBarWidget_SetScrollValue(bar, val);
    }
}

#endif // LA_SCROLLBAR_WIDGET_ENABLED