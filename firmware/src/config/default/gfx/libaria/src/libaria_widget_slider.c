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


#include "gfx/libaria/inc/libaria_widget_slider.h"

#if LA_SLIDER_WIDGET_ENABLED

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
#define DEFAULT_VALUE           0

#define DEFAULT_STEP            1

#define DEFAULT_GRIP            10

/*
static void invalidateSlideArea(laSliderWidget* sld)
{
    GFX_Rect rect;
    
    _laSliderWidget_GetSlideAreaRect(sld, &rect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)sld),
                          &rect,
                          GFX_FALSE);
}*/

static void invalidateHandle(laSliderWidget* sld)
{
    GFX_Rect rect;
    
    _laSliderWidget_GetHandleRect(sld, &rect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)sld),
                          &rect,
                          GFX_FALSE);
}

uint32_t _laSliderWidget_GetPercentFromPoint(laSliderWidget* sld, GFX_Point* pnt)
{
    GFX_Rect scrollRect;
    int32_t val;
    
    _laSliderWidget_GetSlideAreaRect(sld, &scrollRect);
    
    if(sld->alignment == LA_SLIDER_ORIENT_VERTICAL)
    {
        // translate rect and point in to rect space
        val = scrollRect.height - (pnt->y - scrollRect.y);
        scrollRect.y = 0;
        
        if(val <= 0)
            return 0;
        else if(val >= scrollRect.height)
            return 100;
            
        return GFX_PercentWholeRounded(val, scrollRect.height);
    }
    else
    {
        // translate rect and point in to rect space
        val = pnt->x - scrollRect.x;
        scrollRect.x = 0;
        
        if(val <= 0)
            return 0;
        else if(val >= scrollRect.width)
            return 100;
        
        return GFX_PercentWholeRounded(val, scrollRect.width);
    }
}

uint32_t _laSliderWidget_GetValueFromPercent(laSliderWidget* sld, uint32_t per)
{
    int32_t max, val;
    
    if(per == 0)
        return sld->min;
    else if(per == 100)
        return sld->max;
    
    // zero based range
    max = sld->max - sld->min;
    
    val = GFX_PercentOf(max, per);
        
    return val - sld->min;
}

void _laSliderWidget_Constructor(laSliderWidget* sld)
{
    _laWidget_Constructor((laWidget*)sld);
    
    sld->widget.destructor = (laWidget_Destructor_FnPtr)&_laSliderWidget_Destructor;

    sld->widget.type = LA_WIDGET_SLIDER;

    // override base class methods
    sld->widget.update = (laWidget_Update_FnPtr)&_laSliderWidget_Update;
    sld->widget.paint = (laWidget_Paint_FnPtr)&_laSliderWidget_Paint;

    sld->widget.rect.width = DEFAULT_WIDTH;
    sld->widget.rect.height = DEFAULT_HEIGHT;

    sld->state = LA_SLIDER_STATE_NONE;

    sld->widget.borderType = LA_WIDGET_BORDER_BEVEL;
    sld->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;

    sld->alignment = LA_SLIDER_ORIENT_VERTICAL;
    
    sld->min = DEFAULT_MIN;
    sld->max = DEFAULT_MAX;
    sld->value = DEFAULT_VALUE;
    sld->grip = DEFAULT_GRIP;

    sld->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&_laSliderWidget_TouchDownEvent;
    sld->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&_laSliderWidget_TouchUpEvent;
    sld->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&_laSliderWidget_TouchMovedEvent;

    sld->widget.invalidateBorderAreas = (laWidget_InvalidateBorderAreas_FnPtr)&_laSliderWidget_InvalidateBorderAreas;
}

void _laSliderWidget_Destructor(laSliderWidget* sld)
{
    _laWidget_Destructor((laWidget*)sld);
}

laWidgetUpdateState _laSliderWidget_Update(laSliderWidget* sld)
{
    return LA_WIDGET_UPDATE_STATE_DONE;
}

laSliderWidget* laSliderWidget_New()
{
    laSliderWidget* sld = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    sld = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laSliderWidget));
    
    _laSliderWidget_Constructor(sld);

    return sld;
}

laSliderOrientation laSliderWidget_GetOrientation(laSliderWidget* sld)
{
    if(sld == NULL)
        return 0;

    return sld->alignment;
}

laResult laSliderWidget_SetOrientation(laSliderWidget* sld,
                                       laSliderOrientation align,
                                       laBool swapDimensions)
{
    uint32_t t;
    
    if(sld == NULL)
        return LA_FAILURE;
        
    if(sld->alignment == align)
        return LA_SUCCESS;

    sld->alignment = align;
    
    // reverse dimensions
    if(swapDimensions == LA_TRUE)
    {
        t = sld->widget.rect.width;
        sld->widget.rect.width = sld->widget.rect.height;
        sld->widget.rect.height = t;
    }
    
    laWidget_Invalidate((laWidget*)sld);
    
    return LA_SUCCESS;
}

uint32_t laSliderWidget_GetGripSize(laSliderWidget* sld)
{
    if(sld == NULL)
        return 0;
        
    return sld->grip;
}

laResult laSliderWidget_SetGripSize(laSliderWidget* sld, uint32_t size)
{
    if(sld == NULL)
        return LA_FAILURE;
        
    if(sld->grip == size)
        return LA_SUCCESS;
        
    invalidateHandle(sld);
        
    sld->grip = size;
    
    invalidateHandle(sld);
        
    return LA_SUCCESS;
}

uint32_t laSliderWidget_GetMininumValue(laSliderWidget* sld)
{
    if(sld == NULL)
        return 0;
        
    return sld->min;
}

laResult laSliderWidget_SetMinimumValue(laSliderWidget* sld, 
                                           uint32_t val)
{
    if(sld == NULL)
        return LA_FAILURE;
        
    if(sld->min == val)
        return LA_SUCCESS;
        
    invalidateHandle(sld);
        
    sld->min = val;
    
    invalidateHandle(sld);
    
    if(sld->valueChangedEvent != NULL)
        sld->valueChangedEvent(sld);
        
    return LA_SUCCESS;
}
                                                      
uint32_t laSliderWidget_GetMaxinumValue(laSliderWidget* sld)
{
    if(sld == NULL)
        return 0;
        
    return sld->max;
}

laResult laSliderWidget_SetMaximumValue(laSliderWidget* sld,
                                        uint32_t val)
{
    if(sld == NULL)
        return LA_FAILURE;
        
    if(sld->max == val)
        return LA_SUCCESS;
    
    invalidateHandle(sld);
   
    sld->max = val;
    
    invalidateHandle(sld);
        
    if(sld->valueChangedEvent != NULL)
        sld->valueChangedEvent(sld);
        
    return LA_SUCCESS;
}                                                 

uint32_t laSliderWidget_GetSliderPercentage(laSliderWidget* sld)
{
    if(sld == NULL)
        return 0;
        
    return GFX_PercentWholeRounded(sld->value, sld->max - sld->min);
}

laResult laSliderWidget_SetSliderPercentage(laSliderWidget* sld,
                                            uint32_t val)
{
    uint32_t value;

    if(sld == NULL)
        return LA_FAILURE;

    value = GFX_PercentOf(sld->max - sld->min, val) + sld->min;

    if(sld->value == value)
        return LA_SUCCESS;

    invalidateHandle(sld);

    sld->value = value;

    invalidateHandle(sld);

    if(sld->valueChangedEvent != NULL)
        sld->valueChangedEvent(sld);

    return LA_SUCCESS;
} 
                                                                                                               
int32_t laSliderWidget_GetSliderValue(laSliderWidget* sld)
{
    if(sld == NULL)
        return 0;
        
    return sld->value;
}

laResult laSliderWidget_SetSliderValue(laSliderWidget* sld, int32_t val) 
{
    if(sld == NULL)
        return LA_FAILURE;
        
    if(sld->value == val)
        return LA_SUCCESS;
        
    if(val > sld->max)
        val = sld->max;
        
    if(val < sld->min)
        val = sld->min;
        
    invalidateHandle(sld); 
   
    sld->value = val;
    
    invalidateHandle(sld);
    
    if(sld->valueChangedEvent != NULL)
        sld->valueChangedEvent(sld);
        
    return LA_SUCCESS;
}

laResult laSliderWidget_Step(laSliderWidget* sld, int32_t amount)
{
    if(sld == NULL || amount == 0)
        return LA_FAILURE;
    
    invalidateHandle(sld);
    
    if(amount < 0)
    {
        sld->value -= amount;
        
        if(sld->value < sld->min)
            sld->value = sld->min;
    }
    else
    {
        sld->value += amount;
        
        if(sld->value > sld->max)
            sld->value = sld->max;
    }
        
    if(sld->valueChangedEvent != NULL)
        sld->valueChangedEvent(sld);
        
    //printf("%u\n", sld->value);
        
    invalidateHandle(sld);
        
    return LA_SUCCESS;
}

laSliderWidget_ValueChangedEvent laSliderWidget_GetValueChangedEventCallback(laSliderWidget* sld)
{
    if(sld == NULL)
        return NULL;

    return sld->valueChangedEvent;
}

laResult laSliderWidget_SetValueChangedEventCallback(laSliderWidget* sld,
                                                     laSliderWidget_ValueChangedEvent cb)
{
    if(sld == NULL || sld->valueChangedEvent == cb)
        return LA_FAILURE;

    sld->valueChangedEvent = cb;
    
    return LA_SUCCESS;
}

void _laSliderWidget_TouchDownEvent(laSliderWidget* sld,
                                    laInput_TouchDownEvent* evt)
{
    GFX_Rect rect;
    GFX_Point pnt;
    uint32_t percent;
    uint32_t value;

    pnt.x = evt->x;
    pnt.y = evt->y;
    
    // already guaranteed to be inside widget rectangle, accept event
    evt->event.accepted = LA_TRUE;
    
    // was the handle touched
    _laSliderWidget_GetHandleRect(sld, &rect);
    
    laUtils_ClipRectToParent((laWidget*)sld, &rect);
    laUtils_RectToScreenSpace((laWidget*)sld, &rect);
    
    if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
    {
        sld->handleDownOffset.x = evt->x - rect.x - (rect.width / 2);
        sld->handleDownOffset.y = evt->y - rect.y - (rect.height / 2);
    }
    else
    {
        sld->handleDownOffset.x = 0;
        sld->handleDownOffset.y = 0;
        
        percent = _laSliderWidget_GetPercentFromPoint(sld, &pnt);
        
        value = _laSliderWidget_GetValueFromPercent(sld, percent);
        
        if(sld->value != value)
        {
            invalidateHandle(sld);
            
            sld->value = value;
            
            invalidateHandle(sld);
            
            if(sld->valueChangedEvent != NULL)
                sld->valueChangedEvent(sld);
        }
    }
    
    sld->state = LA_SLIDER_STATE_HANDLE_DOWN;
}

void _laSliderWidget_TouchUpEvent(laSliderWidget* sld,
                                  laInput_TouchUpEvent* evt)
{
    evt->event.accepted = LA_TRUE;
    
    sld->state = LA_SLIDER_STATE_NONE;
}

void _laSliderWidget_TouchMovedEvent(laSliderWidget* sld,
                                     laInput_TouchMovedEvent* evt)
{
    GFX_Point pnt;
    uint32_t percent;

    pnt.x = evt->x;
    pnt.y = evt->y;

    evt->event.accepted = LA_TRUE;

    if(sld->state == LA_SLIDER_STATE_HANDLE_DOWN)
    {
        pnt.x = evt->x - sld->handleDownOffset.x;
        pnt.y = evt->y - sld->handleDownOffset.y;
        
        //printf("%i, %i\n", pnt.x, pnt.y);
        
        percent = _laSliderWidget_GetPercentFromPoint(sld, &pnt);
        
        //printf("%u\n", percent);
        
        percent = _laSliderWidget_GetValueFromPercent(sld, percent);
        
        //printf("    %u, %u\n", percent, i);
        
        if(percent != sld->value)
        {
            invalidateHandle(sld);
            
            sld->value = percent;
            
            invalidateHandle(sld);
            
            if(sld->valueChangedEvent != NULL)
                sld->valueChangedEvent(sld);
        }
    }
}

#endif // LA_SLIDER_WIDGET_ENABLED