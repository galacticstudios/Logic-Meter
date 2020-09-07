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


#include "gfx/libaria/inc/libaria_widget_list.h"

#if LA_LIST_WIDGET_ENABLED && LA_SCROLLBAR_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_STRING,
    WAIT_STRING,
    DRAW_ICON,
    WAIT_ICON,
    DRAW_BORDER,
};

void _laListWidget_GetListRect(laListWidget* lst,
                               GFX_Rect* rect)
{
    *rect = lst->widget.rect;
    
    //if(lst->widget
    
    if(lst->widget.borderType == LA_WIDGET_BORDER_NONE)
    {
        if(lst->scrollbar->widget.visible == LA_TRUE)
            rect->width -= lst->scrollbar->widget.rect.width;
    }
    else if(lst->widget.borderType == LA_WIDGET_BORDER_LINE)
    {
        rect->x += 1;
        rect->width -= 1;
        
        if(lst->scrollbar->widget.visible == LA_TRUE)
            rect->width -= lst->scrollbar->widget.rect.width;
        else
            rect->width -= 1;
            
        rect->y += 1;
        rect->height -= 2;
    }
    else if(lst->widget.borderType == LA_WIDGET_BORDER_BEVEL)
    {
        rect->x += 2;
        rect->width -= 2;
        
        if(lst->scrollbar->widget.visible == LA_TRUE)
            rect->width -= lst->scrollbar->widget.rect.width;
        else
            rect->width -= 2;
            
        rect->y += 2;
        rect->height -= 4;
    }
}

void _laListWidget_GetRowRect(laListWidget* lst,
                              uint32_t idx,
                              GFX_Rect* rect)
{
    GFX_Rect textRect;
    
    laListItem* item;
    
    // get list item
    item = lst->items.values[idx];

    // get text rectangle
    laString_GetRect(&item->string, &textRect);

    // craft item entry rectangle
    rect->x = 0;
    rect->width = lst->widget.rect.width;
    
    if(lst->scrollbar->widget.visible == LA_TRUE)
        rect->width -= lst->scrollbar->widget.rect.width;
        
    rect->y = _laListWidget_GetRowY(lst, idx);
    rect->height = item->rowRect.height;

    laUtils_RectToLayerSpace((laWidget*)lst, rect);
}

void _laListWidget_GetTextRect(laListWidget* lst,
                               uint32_t idx,
                               GFX_Rect* textRect,
                               GFX_Rect* drawRect)
{
    GFX_Rect rowRect, imageRect;
    int32_t y;
    
    laListItem* item;
    
    // get list item
    item = lst->items.values[idx];

    // get text rectangle
    laString_GetRect(&item->string, textRect);
    
    // craft item entry rectangle
    rowRect.x = 0;
    rowRect.width = lst->widget.rect.width;
    
    if(lst->scrollbar->widget.visible == LA_TRUE)
        rowRect.width -= lst->scrollbar->widget.rect.width;
        
    rowRect.y = 0;
    rowRect.height = item->rowRect.height;
                      
    imageRect = GFX_Rect_Zero;
    
    if(item->icon != NULL)
    {
        imageRect.width = item->icon->width;
        imageRect.height = item->icon->height;
    }
    
    // arrange relative to image rect
    laUtils_ArrangeRectangleRelative(textRect,
                                     imageRect,
                                     rowRect,
                                     lst->halign,
                                     LA_VALIGN_MIDDLE,
                                     lst->iconPos,
                                     lst->widget.margin.left,
                                     lst->widget.margin.top,
                                     lst->widget.margin.right,
                                     lst->widget.margin.bottom,
                                     lst->iconMargin);

    GFX_RectClip(textRect, &rowRect, drawRect);                                     
                                     
    laUtils_RectToLayerSpace((laWidget*)lst, textRect);
    laUtils_RectToLayerSpace((laWidget*)lst, drawRect);
    
    y = _laListWidget_GetRowY(lst, idx);
    
    textRect->y += y;
    drawRect->y += y;
}

void _laListWidget_GetIconRect(laListWidget* lst,
                               uint32_t idx,
                               GFX_Rect* iconRect,
                               GFX_Rect* imgSrcRect)
{
    GFX_Rect rowRect, textRect;
    
    laListItem* item;
    
    // get list item
    item = lst->items.values[idx];

    // get text rectangle
    laString_GetRect(&item->string, &textRect);
    
    // craft item entry rectangle
    rowRect.x = 0;
    rowRect.width = lst->widget.rect.width;
    
    if(lst->scrollbar->widget.visible == LA_TRUE)
        rowRect.width -= lst->scrollbar->widget.rect.width;
        
    rowRect.y = 0;
    rowRect.height = item->rowRect.height;
                      
    *iconRect = GFX_Rect_Zero;
    
    if(item->icon != NULL)
    {
        iconRect->width = item->icon->width;
        iconRect->height = item->icon->height;
    }
    
    *imgSrcRect = *iconRect;
    
    // arrange relative to image rect
    laUtils_ArrangeRectangle(iconRect,
                             textRect,
                             rowRect,
                             lst->halign,
                             LA_VALIGN_MIDDLE,
                             lst->iconPos,
                             lst->widget.margin.left,
                             lst->widget.margin.top,
                             lst->widget.margin.right,
                             lst->widget.margin.bottom,
                             lst->iconMargin);
                             
    *iconRect = GFX_RectClipAdj(iconRect, &rowRect, imgSrcRect); 
    
    laUtils_RectToLayerSpace((laWidget*)lst, iconRect);
    
    iconRect->y += _laListWidget_GetRowY(lst, idx);
}

int32_t _laListWidget_GetRowY(laListWidget* lst, uint32_t idx)
{
    uint32_t row;
    int32_t y = 0;
    laListItem* item;
    
    //if(idx == 0)
    //    return 0;
    
    for(row = 0; row < idx; row++)
    {
        // get list item
        item = lst->items.values[row];
        
        y += item->rowRect.height;
    }

    y -= lst->scrollbar->value;
    
    return y;
}

void _laListWidget_recalculateRowRect(laListWidget* lst,
                                     uint32_t idx)
{
    GFX_Rect textRect;

    laListItem* item;

    // get list item
    item = lst->items.values[idx];

    // get text rectangle
    laString_GetRect(&item->string, &textRect);

    // craft item entry rectangle
    item->rowRect.x = 0;
    item->rowRect.width = lst->widget.rect.width;

    if(lst->scrollbar->widget.visible == LA_TRUE)
        item->rowRect.width -= lst->scrollbar->widget.rect.width;

    item->rowRect.y = _laListWidget_GetRowY(lst, idx);

    if (item->icon != NULL && item->icon->height > textRect.height)
    {
        item->rowRect.height = item->icon->height +
                       lst->widget.margin.top +
                       lst->widget.margin.bottom;
    }
    else
    {
        item->rowRect.height = textRect.height +
                       lst->widget.margin.top +
                       lst->widget.margin.bottom;
    }

    laUtils_RectToLayerSpace((laWidget*)lst, &item->rowRect);
}

void _laListWidget_GetLogicalRect(laListWidget* lst, GFX_Rect* rect)
{
    uint32_t idx;
    GFX_Rect row;
    
    rect->x = 0;
    rect->y = 0;
    rect->width = lst->widget.rect.width;
    rect->height = 0;
    
    for(idx = 0; idx < lst->items.size; idx++)
    {
        _laListWidget_GetRowRect(lst, idx, &row);
        
        rect->height += row.height;
    }
}

static void drawBackground(laListWidget* lst);
static void drawString(laListWidget* lst);
static void waitString(laListWidget* lst);
static void drawIcon(laListWidget* lst);
static void waitIcon(laListWidget* lst);
static void drawBorder(laListWidget* lst);

static void nextState(laListWidget* lst)
{
    switch(lst->widget.drawState)
    {
        case NOT_STARTED:
        {
            lst->widget.drawState = DRAW_BACKGROUND;
            lst->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

            return;
        }
        case DRAW_BACKGROUND:
        {
            if(lst->items.size > 0)
            {
                lst->paintState.nextItem = 0;
                lst->paintState.y = 0;
                
                lst->widget.drawState = DRAW_STRING;
                lst->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;
            
                return;
            }
        }
        case DRAW_STRING:
        {            
            if(lst->items.size > 0)
            {
                lst->paintState.nextItem = 0;
                lst->paintState.y = 0;
                
                lst->widget.drawState = DRAW_ICON;
                lst->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawIcon;
            
                return;
            }
        }
        case DRAW_ICON:
        {
            if(lst->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                lst->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                lst->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            lst->widget.drawState = DONE;
            lst->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laListWidget* lst)
{
    uint32_t i;
    GFX_Rect widgetRect, rowRect, clipRect;
    laListItem* item;
    laLayer* layer = laUtils_GetLayer((laWidget*)lst);
    
    // draw widget background
    if(lst->widget.backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        laWidget_SkinClassic_DrawBackground((laWidget*)lst, lst->widget.scheme->background);
    }
    else if(lst->widget.backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        laWidget_SkinClassic_DrawBlit((laWidget*)lst, lst->widget.cache);
    }
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)lst);
    
    // draw item highlights
    if(lst->items.size > 0)
    {
        for(i = 0; i < lst->items.size; i++)
        {
            item = lst->items.values[i];
            
            _laListWidget_GetRowRect(lst, i, &rowRect);
            
            if(GFX_RectIntersects(&rowRect, &layer->clippedDrawingRect) == GFX_TRUE)
            {        
                if(i == lst->itemDown && item->enabled == LA_TRUE)
                {
                    GFX_Set(GFXF_DRAW_COLOR, lst->widget.scheme->backgroundInactive);
                }
                else if(item->selected == LA_TRUE)
                {
                    GFX_Set(GFXF_DRAW_COLOR, lst->widget.scheme->textHighlight);
                }
                else
                {
                    continue;
                }
                    //GFX_Set(GFXF_DRAW_COLOR, lst->widget.scheme->background);
                    
                GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
                GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
                
                GFX_RectClip(&rowRect, &layer->clippedDrawingRect, &clipRect);
                
                // row may be out of the list bounds
                if(GFX_RectIntersects(&clipRect, &widgetRect) == GFX_TRUE)
                {
                    // clip again to the widget bounds
                    rowRect = clipRect;
                        
                    GFX_RectClip(&rowRect, &widgetRect, &clipRect);                    
                    
                    GFX_DrawRect(clipRect.x,
                                 clipRect.y,
                                 clipRect.width,
                                 clipRect.height);
                }
            }
        }
    }
    
    nextState(lst);
}

static void drawString(laListWidget* lst)
{
    GFX_Rect widgetRect, textRect, drawRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)lst);
    
    if(lst->paintState.nextItem == lst->items.size)
    {
        nextState(lst);

        return;
    }
        
    // get rectangles
    lst->paintState.item = lst->items.values[lst->paintState.nextItem];
    
    _laListWidget_GetTextRect(lst,
                              lst->paintState.nextItem,
                              &textRect,
                              &drawRect);
                              
    widgetRect = laUtils_WidgetLayerRect((laWidget*)lst);
                              
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &drawRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
            
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        
        if((lst->paintState.nextItem == lst->itemDown ||
           lst->paintState.item->selected == LA_FALSE) && lst->paintState.item->enabled == LA_TRUE)
	{
            GFX_Set(GFXF_DRAW_COLOR, lst->widget.scheme->text);
	}
	else if((lst->paintState.nextItem == lst->itemDown ||
           lst->paintState.item->selected == LA_FALSE) && lst->paintState.item->enabled == LA_FALSE)
	{
	    GFX_Set(GFXF_DRAW_COLOR, lst->widget.scheme->textDisabled);
	}    
    else
	{
            GFX_Set(GFXF_DRAW_COLOR, lst->widget.scheme->textHighlightText);
	}
        
        // row may be out of widget bounds
        if(GFX_RectIntersects(&clipRect, &widgetRect) == GFX_TRUE)
        {
            // clip again to widget bounds
            drawRect = clipRect;
            
            GFX_RectClip(&drawRect, &widgetRect, &clipRect);
            
            laString_DrawClipped(&lst->paintState.item->string,
                                 clipRect.x,
                                 clipRect.y,
                                 clipRect.width,
                                 clipRect.height,
                                 textRect.x,
                                 textRect.y,
                                 &lst->reader);
                                 
            if(lst->reader != NULL)
            {
                lst->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
                lst->widget.drawState = WAIT_STRING;
                
                return;
            }
        }
    }
    
    lst->paintState.nextItem++;
}

static void waitString(laListWidget* lst)
{
    if(lst->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        lst->reader->run(lst->reader);
        
        return;
    }
    
    // free the reader
    lst->reader->memIntf->heap.free(lst->reader);
    lst->reader = NULL;
    
    lst->paintState.nextItem++;
    
    lst->widget.drawState = DRAW_STRING;
    lst->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;
}

static void drawIcon(laListWidget* lst)
{
    GFX_Rect widgetRect, imgRect, imgSrcRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)lst);
	GFX_Bool alphaEnable;

    if(lst->paintState.nextItem == lst->items.size)
    {
        nextState(lst);

        return;
    }
    
    lst->paintState.item = lst->items.values[lst->paintState.nextItem];
    
    // skip if no icon
    if(lst->paintState.item->icon == NULL)
    {
        lst->paintState.nextItem++;
        
        if(lst->paintState.nextItem == lst->items.size)
            nextState(lst);
        
        return;
    }
    
    _laListWidget_GetIconRect(lst, lst->paintState.nextItem, &imgRect, &imgSrcRect);
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)lst);
    
    if(GFX_RectIntersects(&imgRect, &layer->clippedDrawingRect) == GFX_TRUE)
    {
        clipRect = GFX_RectClipAdj(&imgRect, &layer->clippedDrawingRect, &imgSrcRect);
        
        // row may be out of widget bounds
        if(GFX_RectIntersects(&clipRect, &widgetRect) == GFX_TRUE)
        {    
            // clip again to widget bounds
            imgRect = clipRect;
            
            clipRect = GFX_RectClipAdj(&imgRect, &widgetRect, &imgSrcRect);
            
			GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
			GFX_Set(GFXF_DRAW_ALPHA_ENABLE, lst->widget.alphaEnabled);

			GFXU_DrawImage(lst->paintState.item->icon,
                           imgSrcRect.x,
                           imgSrcRect.y,
                           imgSrcRect.width,
                           imgSrcRect.height,
                           clipRect.x,
                           clipRect.y,
                           &laContext_GetActive()->memIntf,
                           &lst->reader);
                           
			GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);

			if (lst->reader != NULL)
            {
                lst->widget.drawState = WAIT_ICON;
                lst->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitIcon;
                
                return;
            }
        }
    }
    
    lst->paintState.nextItem++;
}

static void waitIcon(laListWidget* lst)
{
    if(lst->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        lst->reader->run(lst->reader);
        
        return;
    }
    
    // free the reader
    lst->reader->memIntf->heap.free(lst->reader);
    lst->reader = NULL;
    
    lst->paintState.nextItem++;
    
    lst->widget.drawState = DRAW_ICON;
    lst->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawIcon;
}

static void drawBorder(laListWidget* lst)
{
    // kind of a hack to make sure the border types stay in sync
    // without having to create an event to override
    laWidget_SetBorderType((laWidget*)lst->scrollbar, lst->widget.borderType);
   
    if(lst->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)lst);
    else if(lst->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardLoweredBorder((laWidget*)lst);
    
    nextState(lst);
}

void _laListWidget_Paint(laListWidget* lst)
{
    laContext* context = laContext_GetActive();
    
    if(lst->widget.scheme == NULL)
    {
        lst->widget.drawState = DONE;
        
        return;
    }
    
    if(lst->widget.drawState == NOT_STARTED)
        nextState(lst);
    
    while(lst->widget.drawState != DONE)
    {
        lst->widget.drawFunc((laWidget*)lst);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           lst->widget.drawState == WAIT_STRING ||
           lst->widget.drawState == WAIT_ICON)
            break;
    }
}

#endif // LA_LIST_WIDGET_ENABLED && LA_SCROLLBAR_WIDGET_ENABLED
