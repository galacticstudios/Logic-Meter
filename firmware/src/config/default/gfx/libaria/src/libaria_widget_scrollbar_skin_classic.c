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
    DRAW_UP_BUTTON,
    DRAW_DOWN_BUTTON,
    DRAW_HANDLE
};

void _laScrollBar_GetUpLeftButtonRect(laScrollBarWidget* bar, GFX_Rect* rect)
{
    rect->x = 0;
    rect->y = 0;
        
    if(bar->alignment == LA_SCROLLBAR_ORIENT_VERTICAL)
    {
        rect->width = bar->widget.rect.width;
        rect->height = rect->width;
    }
    else
    {
        rect->width = bar->widget.rect.height;
        rect->height = rect->width;
    }
    
    laUtils_RectToLayerSpace((laWidget*)bar, rect);
}

void _laScrollBar_GetDownRightButtonRect(laScrollBarWidget* bar, GFX_Rect* rect)
{
    if(bar->alignment == LA_SCROLLBAR_ORIENT_VERTICAL)
    {
        rect->width = bar->widget.rect.width;
        rect->height = rect->width;
        rect->x = 0;
        rect->y = bar->widget.rect.height - rect->width;
    }
    else
    {
        rect->width = bar->widget.rect.height;
        rect->height = rect->width;
        rect->y = 0;
        rect->x = bar->widget.rect.width - rect->height;
    }
    
    laUtils_RectToLayerSpace((laWidget*)bar, rect);
}

void _laScrollBar_GetScrollAreaRect(laScrollBarWidget* bar, GFX_Rect* rect)
{
    GFX_Rect scrollRect;
    
    if(bar->alignment == LA_SCROLLBAR_ORIENT_VERTICAL)
    {
        scrollRect.x = 0;
        scrollRect.y = bar->widget.rect.width;
        scrollRect.width = bar->widget.rect.width;
        scrollRect.height = bar->widget.rect.height - (bar->widget.rect.width * 2);
    }
    else
    {
        scrollRect.x = bar->widget.rect.height;
        scrollRect.y = 0;
        scrollRect.width = bar->widget.rect.width - (bar->widget.rect.height * 2);
        scrollRect.height = bar->widget.rect.height;
    }
    
    *rect = scrollRect;
    
    laUtils_RectToLayerSpace((laWidget*)bar, rect);
}

void _laScrollBar_GetHandleRect(laScrollBarWidget* bar, GFX_Rect* rect)
{
    GFX_Rect scrollRect;
    uint32_t extent;
    uint32_t sizePercent;
    uint32_t locPercent;
    
    extent = _laScrollWidget_GetExtent(bar);
        
    if(bar->value >= 1)
        extent = extent;
    
    sizePercent = GFX_PercentWholeRounded(extent, bar->max);
    locPercent = GFX_PercentWholeRounded(bar->value, bar->max);
    
    _laScrollBar_GetScrollAreaRect(bar, &scrollRect);
    
    if(bar->alignment == LA_SCROLLBAR_ORIENT_VERTICAL)
    {
        sizePercent = GFX_PercentOf(scrollRect.height, sizePercent);
        locPercent = GFX_PercentOf(scrollRect.height - sizePercent, locPercent);
        
        rect->x = scrollRect.x;
        rect->y = scrollRect.y + locPercent;
        rect->width = scrollRect.width;
        rect->height = sizePercent;
    }
    else
    {
        sizePercent = GFX_PercentOf(scrollRect.width, sizePercent);
        locPercent = GFX_PercentOf(scrollRect.width - sizePercent, locPercent);
        
        rect->x = scrollRect.x + locPercent;
        rect->y = scrollRect.y;
        rect->width = sizePercent;
        rect->height = scrollRect.height;
    }
}

void _laScrollBarWidget_InvalidateBorderAreas(laScrollBarWidget* bar)
{
    laWidget_Invalidate((laWidget*)bar);
}

static void drawBackground(laScrollBarWidget* bar);
static void drawUpButton(laScrollBarWidget* bar);
static void drawDownButton(laScrollBarWidget* bar);
static void drawHandle(laScrollBarWidget* bar);

static void nextState(laScrollBarWidget* bar)
{
    switch(bar->widget.drawState)
    {
        case NOT_STARTED:
        {
            bar->widget.drawState = DRAW_BACKGROUND;
            bar->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

            return;
        }
        case DRAW_BACKGROUND:
        {
            bar->widget.drawState = DRAW_UP_BUTTON;
            bar->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawUpButton;
            
            return;
        }
        case DRAW_UP_BUTTON:
        {            
            bar->widget.drawState = DRAW_DOWN_BUTTON;
            bar->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawDownButton;
            
            return;
        }
        case DRAW_DOWN_BUTTON:
        {            
            bar->widget.drawState = DRAW_HANDLE;
            bar->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawHandle;
            
            return;
        }
        case DRAW_HANDLE:
        {
            bar->widget.drawState = DONE;
            bar->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laScrollBarWidget* bar)
{
    GFX_Rect rect;
    laLayer* layer;
    
    if(bar->widget.backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        layer = laUtils_GetLayer((laWidget*)bar);
        
        _laScrollBar_GetScrollAreaRect(bar, &rect);
     
        laWidget_SkinClassic_FillRect(&rect,
                                      bar->widget.scheme->background,
                                      &layer->clippedDrawingRect);
    }
    else if(bar->widget.backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        laWidget_SkinClassic_DrawBlit((laWidget*)bar, bar->widget.cache);
    }
    
    nextState(bar);
}

static void drawUpButton(laScrollBarWidget* bar)
{
    GFX_Rect rect, clipRect;
    GFX_Point pnt;
    uint32_t arrowOffset = 0;
    laLayer* layer = laUtils_GetLayer((laWidget*)bar);
    
    _laScrollBar_GetUpLeftButtonRect(bar, &rect);
    
    if(GFX_RectIntersects(&rect, &layer->clippedDrawingRect) == GFX_FALSE)
    {
        nextState(bar);
        
        return;
    }
    
    GFX_RectClip(&rect, &layer->clippedDrawingRect, &clipRect);
    
    GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
    GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);   
    
    // fill button area
    if(bar->state == LA_SCROLLBAR_STATE_TOP_INSIDE)
    {
        laWidget_SkinClassic_FillRect(&rect,
                                      bar->widget.scheme->background,
                                      &layer->clippedDrawingRect);
        
        arrowOffset = 1;
    }
    else
    {
        laWidget_SkinClassic_FillRect(&rect,
                                      bar->widget.scheme->base,
                                      &layer->clippedDrawingRect);
    }
                 
    // draw button arrow
    GFX_Set(GFXF_DRAW_COLOR, bar->widget.scheme->foreground);
    
    if(bar->alignment == LA_SCROLLBAR_ORIENT_VERTICAL)
    {
        pnt.x = rect.x + (rect.width / 2) - 1 + arrowOffset;
        pnt.y = rect.y + (rect.height / 2) - 1 + arrowOffset;
        
        GFX_DrawPixel(pnt.x, pnt.y - 1);
        GFX_DrawLine(pnt.x - 1, pnt.y + 0, pnt.x + 1, pnt.y + 0);
        GFX_DrawLine(pnt.x - 2, pnt.y + 1, pnt.x + 2, pnt.y + 1);
        GFX_DrawLine(pnt.x - 3, pnt.y + 2, pnt.x + 3, pnt.y + 2);
    }
    else
    {
        pnt.x = rect.x + (rect.width / 2) - 1 + arrowOffset;
        pnt.y = rect.y + (rect.height / 2) - 1 + arrowOffset;
        
        GFX_DrawPixel(pnt.x - 1, pnt.y);
        GFX_DrawLine(pnt.x, pnt.y - 1, pnt.x, pnt.y + 1);
        GFX_DrawLine(pnt.x + 1, pnt.y - 2, pnt.x + 1, pnt.y + 2);
        GFX_DrawLine(pnt.x + 2, pnt.y - 3, pnt.x + 2, pnt.y + 3);
    }
    
    // draw button border
    if(bar->widget.borderType == LA_WIDGET_BORDER_LINE)
    {
        laWidget_SkinClassic_DrawLineBorder(&rect,
                                            bar->widget.scheme->shadowDark,
                                            &layer->clippedDrawingRect);
    }
    else if(bar->widget.borderType == LA_WIDGET_BORDER_BEVEL)
    {
        if(bar->state == LA_SCROLLBAR_STATE_TOP_INSIDE)
        {
            laWidget_SkinClassic_Draw2x2BeveledBorder(&rect,
                                            bar->widget.scheme->shadowDark,
                                            bar->widget.scheme->shadow,
                                            bar->widget.scheme->highlightLight,
                                            bar->widget.scheme->highlight,
                                            &layer->clippedDrawingRect);
        }
        else
        {
            laWidget_SkinClassic_Draw1x2BeveledBorder(&rect,
                                  bar->widget.scheme->highlightLight,
                                  bar->widget.scheme->shadowDark,
                                  bar->widget.scheme->shadow,
                                  &layer->clippedDrawingRect);
        }
    }
    
    GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_FALSE);
    
    nextState(bar);
}

static void drawDownButton(laScrollBarWidget* bar)
{
    GFX_Rect rect, clipRect;
    GFX_Point pnt;
    uint32_t arrowOffset = 0;
    laLayer* layer = laUtils_GetLayer((laWidget*)bar);
    
    _laScrollBar_GetDownRightButtonRect(bar, &rect);
    
    if(GFX_RectIntersects(&rect, &layer->clippedDrawingRect) == GFX_FALSE)
    {
        nextState(bar);
        
        return;
    }
    
    GFX_RectClip(&rect, &layer->clippedDrawingRect, &clipRect);
    
    GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
    GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
    
    // fill button area
    if(bar->state == LA_SCROLLBAR_STATE_BOTTOM_INSIDE)
    {
        laWidget_SkinClassic_FillRect(&rect,
                                      bar->widget.scheme->background,
                                      &layer->clippedDrawingRect);
        
        arrowOffset = 1;
    }
    else
    {
        laWidget_SkinClassic_FillRect(&rect,
                                      bar->widget.scheme->base,
                                      &layer->clippedDrawingRect);
    }
        
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
    
    GFX_DrawRect(rect.x,
                 rect.y,
                 rect.width,
                 rect.height);
    
    // draw button arrow      
    GFX_Set(GFXF_DRAW_COLOR, bar->widget.scheme->foreground);
    
    if(bar->alignment == LA_SCROLLBAR_ORIENT_VERTICAL)
    {
        pnt.x = rect.x + (rect.width / 2) - 1 + arrowOffset;
        pnt.y = rect.y + (rect.height / 2) - 1 + arrowOffset;
        
        GFX_DrawLine(pnt.x - 3, pnt.y - 1, pnt.x + 3, pnt.y - 1);
        GFX_DrawLine(pnt.x - 2, pnt.y + 0, pnt.x + 2, pnt.y + 0);
        GFX_DrawLine(pnt.x - 1, pnt.y + 1, pnt.x + 1, pnt.y + 1);
        GFX_DrawPixel(pnt.x, pnt.y + 2);
    }
    else
    {
        pnt.x = rect.x + (rect.width / 2) - 1 + arrowOffset;
        pnt.y = rect.y + (rect.height / 2) - 1 + arrowOffset;
        
        GFX_DrawLine(pnt.x - 1, pnt.y - 3, pnt.x - 1, pnt.y + 3);
        GFX_DrawLine(pnt.x + 0, pnt.y - 2, pnt.x + 0, pnt.y + 2);
        GFX_DrawLine(pnt.x + 1, pnt.y - 1, pnt.x + 1, pnt.y + 1);
        GFX_DrawPixel(pnt.x + 2, pnt.y);
    }
    
    // draw button border
    if(bar->widget.borderType == LA_WIDGET_BORDER_LINE)
    {
        laWidget_SkinClassic_DrawLineBorder(&rect,
                                            bar->widget.scheme->shadowDark,
                                            &layer->clippedDrawingRect);
    }
    else if(bar->widget.borderType == LA_WIDGET_BORDER_BEVEL)
    {
        if(bar->state == LA_SCROLLBAR_STATE_BOTTOM_INSIDE)
        {
            laWidget_SkinClassic_Draw2x2BeveledBorder(&rect,
                                        bar->widget.scheme->shadowDark,
                                        bar->widget.scheme->shadow,
                                        bar->widget.scheme->highlightLight,
                                        bar->widget.scheme->highlight,
                                        &layer->clippedDrawingRect);
        }
        else
        {
            laWidget_SkinClassic_Draw1x2BeveledBorder(&rect,
                                        bar->widget.scheme->highlightLight,
                                        bar->widget.scheme->shadowDark,
                                        bar->widget.scheme->shadow,
                                        &layer->clippedDrawingRect);
        }
    }
    
    GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_FALSE);
    
    nextState(bar);
}

static void drawHandle(laScrollBarWidget* bar)
{
    GFX_Rect rect;
    laLayer* layer = laUtils_GetLayer((laWidget*)bar);
    
    _laScrollBar_GetHandleRect(bar, &rect);
    
    // fill handle area
    laWidget_SkinClassic_FillRect(&rect,
                                  bar->widget.scheme->base,
                                  &layer->clippedDrawingRect);
    
    // draw handle border
    if(bar->widget.borderType == LA_WIDGET_BORDER_LINE)
    {
        laWidget_SkinClassic_DrawLineBorder(&rect,
                                            bar->widget.scheme->shadowDark,
                                            &layer->clippedDrawingRect);
    }
    else if(bar->widget.borderType == LA_WIDGET_BORDER_BEVEL)
    {
        laWidget_SkinClassic_Draw1x2BeveledBorder(&rect,
                      bar->widget.scheme->highlightLight,
                      bar->widget.scheme->shadowDark,
                      bar->widget.scheme->shadow,
                      &layer->clippedDrawingRect);
    }
    
    nextState(bar);
}

void _laScrollBarWidget_Paint(laScrollBarWidget* bar)
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

#endif // LA_SCROLLBAR_WIDGET_ENABLED