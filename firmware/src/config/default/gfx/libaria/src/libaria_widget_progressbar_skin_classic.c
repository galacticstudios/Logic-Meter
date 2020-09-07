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


#include "gfx/libaria/inc/libaria_widget_progressbar.h"

#if LA_PROGRESSBAR_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_util.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_FILL,
    DRAW_BORDER,
};

void _laProgressBarWidget_GetBarRect(laProgressBarWidget* bar,
                                     int32_t val1,
                                     int32_t val2,
                                     GFX_Rect* barRect)
{
    GFX_Rect rect;
    GFX_Rect fill;
    int32_t i;
    uint32_t p1, p2;

	fill.x = 0;
	fill.y = 0;
	fill.width = 0;
	fill.height = 0;

	rect = laUtils_WidgetLayerRect((laWidget*)bar);
    
    if(val2 < val1)
    {
        i = val2;
        val2 = val1;
        val1 = i;
    }
    
    // fill bar
    if(bar->direction == LA_PROGRESSBAR_DIRECTION_RIGHT)
    {
        p1 = GFX_PercentOf(rect.width, val1);
        p2 = GFX_PercentOf(rect.width, val2);
        
        fill.x = rect.x + p1;
        fill.y = rect.y;
        fill.width = p2 - p1;
        fill.height = rect.height;
    }
    else if(bar->direction == LA_PROGRESSBAR_DIRECTION_UP)
    {
        p1 = GFX_PercentOf(rect.height, val1);
        p2 = GFX_PercentOf(rect.height, val2);
        
        fill.height = p2 - p1;
        fill.x = rect.x;
        fill.y = rect.y + rect.height - fill.height - p1;
        fill.width = rect.width;
    }
    else if(bar->direction == LA_PROGRESSBAR_DIRECTION_LEFT)
    {
        p1 = GFX_PercentOf(rect.width, val1);
        p2 = GFX_PercentOf(rect.width, val2);
        
        fill.width = p2 - p1;
        fill.x = rect.x + rect.width - fill.width - p1;
        fill.y = rect.y;
        fill.height = rect.height;
    }
    else if(bar->direction == LA_PROGRESSBAR_DIRECTION_DOWN)
    {
        p1 = GFX_PercentOf(rect.height, val1);
        p2 = GFX_PercentOf(rect.height, val2);
        
        fill.x = rect.x;
        fill.y = rect.y + p1;
        fill.width = rect.width;
        fill.height = p2 - p1;
    }
    
    *barRect = fill;
}

static void drawBackground(laProgressBarWidget* bar);
static void drawFill(laProgressBarWidget* bar);
static void drawBorder(laProgressBarWidget* bar);

static void nextState(laProgressBarWidget* bar)
{
    switch(bar->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(bar->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                bar->widget.drawState = DRAW_BACKGROUND;
                bar->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            if(bar->value > 0)
            {
                bar->widget.drawState = DRAW_FILL;
                bar->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawFill;
                
                return;
            }
        }
        case DRAW_FILL:
        {
            if(bar->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                bar->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                bar->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            bar->widget.drawState = DONE;
            bar->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laProgressBarWidget* bar)
{
    if(bar->widget.backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        laWidget_SkinClassic_DrawBackground((laWidget*)bar, bar->widget.scheme->base);
    }
    else if(bar->widget.backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        laWidget_SkinClassic_DrawBlit((laWidget*)bar, bar->widget.cache);
    }
    
    nextState(bar);
}

static void drawFill(laProgressBarWidget* bar)
{
    GFX_Rect barRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)bar);

    _laProgressBarWidget_GetBarRect(bar, 0, bar->value, &barRect);
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &barRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        GFX_Set(GFXF_DRAW_COLOR, bar->widget.scheme->textHighlight);
    
        GFX_RectClip(&barRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
                 
    nextState(bar);
}

static void drawBorder(laProgressBarWidget* bar)
{
    if(bar->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)bar);
    else if(bar->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardLoweredBorder((laWidget*)bar);
    
    nextState(bar);
}

void _laProgressBarWidget_Paint(laProgressBarWidget* bar)
{
    laContext* context = laContext_GetActive();
    
    if(bar->widget.scheme == NULL)
    {
        bar->widget.drawState = DONE;
        
        return;
    }

    if(bar->widget.drawState == NOT_STARTED)
        nextState(bar);
    
    while(bar->widget.drawState != DONE)
    {
        bar->widget.drawFunc((laWidget*)bar);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_PROGRESSBAR_WIDGET_ENABLED