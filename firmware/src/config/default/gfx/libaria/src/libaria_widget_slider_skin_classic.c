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
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_util.h"

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_BAR,
    DRAW_HANDLE,
    DRAW_BORDER,
};

void _laSliderWidget_GetSlideAreaRect(laSliderWidget* sld, GFX_Rect* rect)
{
    GFX_Rect scrollRect;
    GFX_Rect widgetRect = laUtils_WidgetLayerRect((laWidget*)sld);
    
    if(sld->alignment == LA_SLIDER_ORIENT_VERTICAL)
    {
        scrollRect.width = widgetRect.width / 4;
        scrollRect.height = widgetRect.height - sld->grip;
        scrollRect.x = widgetRect.x + (widgetRect.width / 2) - (scrollRect.width / 2);
        scrollRect.y = widgetRect.y + sld->grip / 2;
    }
    else
    {
        scrollRect.width = widgetRect.width - sld->grip;
        scrollRect.height = widgetRect.height / 4;
        scrollRect.x = widgetRect.x + sld->grip / 2;
        scrollRect.y = widgetRect.y + (widgetRect.height / 2) - (scrollRect.height / 2);
    }
    
    *rect = scrollRect;
}

void _laSliderWidget_GetHandleRect(laSliderWidget* sld, GFX_Rect* rect)
{
    GFX_Rect scrollRect;
    uint32_t val, max, percent;
    GFX_Rect widgetRect = laUtils_WidgetLayerRect((laWidget*)sld);
    
    val = sld->value - sld->min;
    max = sld->max - sld->min;
    
    percent = GFX_PercentWholeRounded(val, max);
    
    _laSliderWidget_GetSlideAreaRect(sld, &scrollRect);
    
    if(sld->alignment == LA_SLIDER_ORIENT_VERTICAL)
    {
        percent = GFX_PercentOf(scrollRect.height, percent);
        
        rect->x = widgetRect.x;
        rect->y = ((scrollRect.y + scrollRect.height) - percent) - (sld->grip / 2);
        rect->width = widgetRect.width;
        rect->height = sld->grip;
    }
    else
    {
        percent = GFX_PercentOf(scrollRect.width, percent);
        
        rect->x = (scrollRect.x + percent) - (sld->grip / 2);
        rect->y = widgetRect.y;
        rect->width = sld->grip;
        rect->height = widgetRect.height;
    }
}

void _laSliderWidget_InvalidateBorderAreas(laSliderWidget* sld)
{
    laWidget_Invalidate((laWidget*)sld);
}

static void drawBackground(laSliderWidget* sld);
static void drawBar(laSliderWidget* sld);
static void drawHandle(laSliderWidget* sld);
//static void drawBorder(laSliderWidget* sld);

static void nextState(laSliderWidget* sld)
{
    switch(sld->widget.drawState)
    {
        case NOT_STARTED:
        {            
            if(sld->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                sld->widget.drawState = DRAW_BACKGROUND;
                sld->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            sld->widget.drawState = DRAW_BAR;
            sld->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBar;
            
            return;
        }
        case DRAW_BAR:
        {
            sld->widget.drawState = DRAW_HANDLE;
            sld->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawHandle;
            
            return;
        }
        case DRAW_HANDLE:
        {
            /*if(sld->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                sld->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                sld->widget.drawState = DRAW_BORDER;
                
                return;
            }*/
            
            sld->widget.drawState = DONE;
            sld->widget.drawFunc = NULL;
        }
        /*case DRAW_BORDER:
        {
            sld->widget.drawState = DONE;
            sld->widget.drawFunc = NULL;
        }*/
    }
}

static void drawBackground(laSliderWidget* sld)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)sld);

    nextState(sld);
}

static void drawBar(laSliderWidget* sld)
{
    GFX_Rect barRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)sld);
    
    _laSliderWidget_GetSlideAreaRect(sld, &barRect);
    
    // fill bar area
    laWidget_SkinClassic_FillRect(&barRect,
                                  sld->widget.scheme->background,
                                  &layer->clippedDrawingRect);
                 
    // draw border
    if(sld->widget.borderType == LA_WIDGET_BORDER_LINE)
    {
        laWidget_SkinClassic_DrawLineBorder(&barRect,
                                            sld->widget.scheme->shadowDark,
                                            &layer->clippedDrawingRect);
    }
    else if(sld->widget.borderType == LA_WIDGET_BORDER_BEVEL)
    {
        laWidget_SkinClassic_Draw2x2BeveledBorder(&barRect,
                                                  sld->widget.scheme->shadow,
                                                  sld->widget.scheme->shadowDark,
                                                  sld->widget.scheme->highlightLight,
                                                  sld->widget.scheme->highlightLight,
                                                  &layer->clippedDrawingRect);
    }
    
    nextState(sld);
}

static void drawHandle(laSliderWidget* sld)
{
    GFX_Rect handleRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)sld);
    
    _laSliderWidget_GetHandleRect(sld, &handleRect);
    
    // fill handle area       
    laWidget_SkinClassic_FillRect(&handleRect,
                                  sld->widget.scheme->base,
                                  &layer->clippedDrawingRect);
    
    // draw handle border
    if(sld->widget.borderType == LA_WIDGET_BORDER_LINE)
    {
        laWidget_SkinClassic_DrawLineBorder(&handleRect,
                                            sld->widget.scheme->shadowDark,
                                            &layer->clippedDrawingRect);
    }
    else if(sld->widget.borderType == LA_WIDGET_BORDER_BEVEL)
    {
        laWidget_SkinClassic_Draw1x2BeveledBorder(&handleRect,
                                                  sld->widget.scheme->highlightLight,
                                                  sld->widget.scheme->shadowDark,
                                                  sld->widget.scheme->shadow,
                                                  &layer->clippedDrawingRect);
    }
    
    nextState(sld);
}

/*static void drawBorder(laSliderWidget* sld)
{
    laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)sld);
    
    nextState(sld);
}*/

void _laSliderWidget_Paint(laSliderWidget* sld)
{
    laContext* context = laContext_GetActive();
    
    if(sld->widget.scheme == NULL)
    {
        sld->widget.drawState = DONE;
        
        return;
    }

    if(sld->widget.drawState == NOT_STARTED)
        nextState(sld);
    
    while(sld->widget.drawState != DONE)
    {
        sld->widget.drawFunc((laWidget*)sld);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_SLIDER_WIDGET_ENABLED