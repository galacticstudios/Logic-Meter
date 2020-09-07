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
#include "gfx/hal/inc/gfx_math.h"

#if LA_LISTWHEEL_WIDGET_ENABLED

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
    DRAW_INDICATORS,
    DRAW_BORDER,
};

static int32_t getItemY(laListWheelWidget* whl,
                        int32_t pos,
                        int32_t itemHeight)
{
    int32_t y = pos * whl->paintState.per;
    
    return (y - (itemHeight / 2)) + whl->rotation;
}

void _laListWheelWidget_GetItemTextRect(laListWheelWidget* whl,
                                        uint32_t idx,
                                        uint32_t pos,
                                        GFX_Rect* textRect,
                                        GFX_Rect* drawRect)
{
    GFX_Rect bounds, imageRect;
    laListWheelItem* item;
    int32_t y;
        
    // get rectangles
    item = whl->items.values[idx];
    
    laString_GetRect(&item->string, textRect);
    
    bounds.x = 0;
    bounds.y = 0;
    bounds.width = whl->widget.rect.width;
    bounds.height = textRect->height;
    
    imageRect = GFX_Rect_Zero;
    
    if(item->icon != NULL)
    {
        if(bounds.height < (int32_t)item->icon->height)
            bounds.height = (int32_t)item->icon->height;
            
        imageRect.width = item->icon->width;
        imageRect.height = item->icon->height;
    }
    
    // arrange relative to image rect
    laUtils_ArrangeRectangleRelative(textRect,
                                     imageRect,
                                     bounds,
                                     whl->halign,
                                     LA_VALIGN_MIDDLE,
                                     whl->iconPos,
                                     whl->widget.margin.left,
                                     whl->widget.margin.top,
                                     whl->widget.margin.right,
                                     whl->widget.margin.bottom,
                                     whl->iconMargin);
                                     
    GFX_RectClip(textRect, &bounds, drawRect);

	// move the rects to layer space
	laUtils_RectToLayerSpace((laWidget*)whl, textRect);
    laUtils_RectToLayerSpace((laWidget*)whl, drawRect);   
    
    y = getItemY(whl, pos, bounds.height);
    
    textRect->y += y;
    drawRect->y += y;  
}

GFX_Rect _laListWheelWidget_GetIndicatorRect(laListWheelWidget* whl)
{
    GFX_Rect indicatorRect, widgetRect;
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)whl);
    
    indicatorRect.x = widgetRect.x;
    indicatorRect.y = widgetRect.y + widgetRect.height/2 - whl->indicatorArea;
    indicatorRect.width = widgetRect.width;
    indicatorRect.height = whl->indicatorArea * 2;
    
    return indicatorRect;
}

void _laListWheelWidget_ItemRectApplyEffects(laListWheelWidget* whl,
                                             GFX_Rect itemRect,
                                             GFX_Rect* newItemRect)
{
    GFX_Rect midLineRect,widgetRect;
    int32_t pct = 100;
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)whl);
    
    midLineRect.y = widgetRect.y + widgetRect.height/2;
    midLineRect.x = widgetRect.x;
    midLineRect.height = 1;
    midLineRect.width = widgetRect.width;
    
    newItemRect->height = itemRect.height;
    newItemRect->width = itemRect.width;
    if (whl->zoomEffects == LA_LISTWHEEL_ZOOM_EFFECT_FIXED_SCALE)
    {
            newItemRect->height = itemRect.height*2/3;
            newItemRect->width = itemRect.width*2/3;
    }
    else
    {
        pct = 100;

        if (itemRect.y > midLineRect.y)
            pct = GFX_PercentWholeRounded(widgetRect.y + widgetRect.height - itemRect.y, widgetRect.height/2);
        else if (itemRect.y + itemRect.height < midLineRect.y)
            pct = GFX_PercentWholeRounded(itemRect.y + itemRect.height - widgetRect.y, widgetRect.height/2);

        switch (whl->zoomEffects)
        {
            case LA_LISTWHEEL_ZOOM_EFFECT_VSCALE:
                newItemRect->height = (itemRect.height * pct)/100;
                break;
            case LA_LISTWHEEL_ZOOM_EFFECT_FULL_SCALE:
            {
                newItemRect->height = (itemRect.height * pct)/100;
                newItemRect->width = (itemRect.width * pct)/100;
                break;
            }
            default:
                break;
        }
    }
}

void _laListWheelWidget_GetItemIconRect(laListWheelWidget* whl,
                                        uint32_t idx,
                                        uint32_t pos,
                                        GFX_Rect* imgRect,
                                        GFX_Rect* imgSrcRect)
{
    GFX_Rect bounds = {0};
    GFX_Rect textRect = {0};
    laListWheelItem* item = whl->items.values[idx];
        
    GFXU_ImageAsset* img = item->icon;
    
    imgRect->x = 0;
    imgRect->y = 0;
    imgRect->width = img->width;
    imgRect->height = img->height;
    
    laString_GetRect(&item->string, &textRect);
    
    bounds.x = 0;
    bounds.y = 0;
    bounds.width = whl->widget.rect.width;
    bounds.height = textRect.height;
    
    if(item->icon != NULL)
    {
        if(bounds.height < (int32_t)item->icon->height)
            bounds.height = (int32_t)item->icon->height;
            
        imgRect->width = item->icon->width;
        imgRect->height = item->icon->height;
    }
    
    *imgSrcRect = *imgRect;
    
    // arrange image rect
    laUtils_ArrangeRectangle(imgRect,
                             textRect,
                             bounds,
                             whl->halign,
                             LA_VALIGN_MIDDLE,
                             whl->iconPos,
                             whl->widget.margin.left,
                             whl->widget.margin.top,
                             whl->widget.margin.right,
                             whl->widget.margin.bottom,
                             whl->iconMargin);   
                             
    *imgRect = GFX_RectClipAdj(imgRect, &bounds, imgSrcRect); 
    
    laUtils_RectToLayerSpace((laWidget*)whl, imgRect); 
    
    imgRect->y += getItemY(whl, pos, bounds.height);
    
    //if(pos == 1)
    //    printf("y=%i\n", getItemY(whl, pos, bounds.height));
}

void _laListWheelWidget_GetItemRect(laListWheelWidget* whl,
                                    uint32_t idx,
                                    uint32_t pos,
                                    GFX_Rect* itemRect)
{
    GFX_Rect textRect = GFX_Rect_Zero;
    laListWheelItem* item = whl->items.values[idx];

    *itemRect = GFX_Rect_Zero;
    
    laString_GetRect(&item->string, &textRect);

    if(item->icon != NULL)
    {
        itemRect->height = item->icon->height;
    }

    itemRect->width = whl->widget.rect.width;
    
    if(textRect.height > itemRect->height)
        itemRect->height = textRect.height;

    laUtils_RectToLayerSpace((laWidget*)whl, itemRect); 
    
    itemRect->y += getItemY(whl, pos, itemRect->height);    
}

static void drawBackground(laListWheelWidget* whl);
static void drawString(laListWheelWidget* whl);
static void waitString(laListWheelWidget* whl);
static void drawIcon(laListWheelWidget* whl);
static void waitIcon(laListWheelWidget* whl);
static void drawIndicators(laListWheelWidget* whl);
static void drawBorder(laListWheelWidget* whl);

static void nextState(laListWheelWidget* whl)
{
    switch(whl->widget.drawState)
    {
        case NOT_STARTED:
        {
            
            whl->paintState.per = GFX_DivideRounding(whl->widget.rect.height, whl->visibleItems - 1);
            
            whl->widget.drawState = DRAW_BACKGROUND;
            whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

            return;
        }
        case DRAW_BACKGROUND:
        {
            if(whl->items.size > 0)
            {
                whl->paintState.nextItem = whl->topItem;
                whl->paintState.y = -1;
                
                whl->widget.drawState = DRAW_STRING;
                whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;
            
                return;
            }
        }
        case DRAW_STRING:
        {            
            //printf("\n");
            
            if(whl->items.size > 0)
            {
                whl->paintState.nextItem = whl->topItem;
                whl->paintState.y = -1;
                
                whl->widget.drawState = DRAW_ICON;
                whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawIcon;
            
                return;
            }
        }
        case DRAW_ICON:
        {
            if(whl->showIndicators == LA_TRUE)
            {                
                whl->widget.drawState = DRAW_INDICATORS;
                whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawIndicators;
            
                return;
            }
        }
        case DRAW_INDICATORS:
        {
            if(whl->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                whl->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            whl->widget.drawState = DONE;
            whl->widget.drawFunc = NULL;
        }
    }
}

static void nextItem(laListWheelWidget* whl)
{
    whl->paintState.nextItem++;
    whl->paintState.y++;
    
    if(whl->paintState.nextItem >= whl->items.size)
        whl->paintState.nextItem = 0;
}



static void drawBackground(laListWheelWidget* whl)
{
    GFX_Rect widgetRect, drawRect, clipRect;
    laLayer* layer;
    
    //printf("drawbackground\n");
    if(whl->widget.backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        if(whl->shaded == LA_TRUE)
        {
            widgetRect = laUtils_WidgetLocalRect((laWidget*)whl);
            laUtils_RectToLayerSpace((laWidget*)whl, &widgetRect);
            
            layer = laUtils_GetLayer((laWidget*)whl);
        
            GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
            GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
        
            // upper rectangle
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR,
                whl->widget.scheme->backgroundDisabled,
                whl->widget.scheme->backgroundInactive,
                NULL,
                NULL);
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_GRADIENT_TOP_BOTTOM);
            
            drawRect.x = widgetRect.x;
            drawRect.y = widgetRect.y;
            drawRect.width = widgetRect.width;
            drawRect.height = (widgetRect.height / 2) - whl->indicatorArea;
            
            if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
            {
                GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
                
                GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
                
                GFX_DrawRect(drawRect.x,
                             drawRect.y,
                             drawRect.width,
                             drawRect.height);
            }
                         
            // lower rectangle
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR,
                whl->widget.scheme->backgroundInactive,
                whl->widget.scheme->backgroundDisabled,
                NULL,
                NULL);
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_GRADIENT_TOP_BOTTOM);
            
            drawRect.y = (widgetRect.y + widgetRect.height / 2) + whl->indicatorArea;
            drawRect.height = (widgetRect.height / 2) - whl->indicatorArea;
            
            if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
            {
                GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
                
                GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
                
                GFX_DrawRect(drawRect.x,
                             drawRect.y,
                             drawRect.width,
                             drawRect.height);
            }
            
            //GFX_Set(GFXF_DRAW_CLIP_RECT, &widgetRect);
            GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_FALSE);
        }
        else
        {
            laWidget_SkinClassic_DrawBackground((laWidget*)whl, whl->widget.scheme->background);
        }
    }
    else if(whl->widget.backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        laWidget_SkinClassic_DrawBlit((laWidget*)whl, whl->widget.cache);
    }
    
    if (whl->indicatorFill != LA_LISTWHEEL_INDICATOR_FILL_NONE)
    {
        widgetRect = laUtils_WidgetLocalRect((laWidget*)whl);
        laUtils_RectToLayerSpace((laWidget*)whl, &widgetRect);
            
        layer = laUtils_GetLayer((laWidget*)whl);
        
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
        
        drawRect = _laListWheelWidget_GetIndicatorRect(whl);
        
        if (whl->indicatorFill == LA_LISTWHEEL_INDICATOR_FILL_GRADIENT)
        {
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR,
                    whl->widget.scheme->foregroundDisabled,
                    whl->widget.scheme->foregroundInactive,
                    NULL,
                    NULL);
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_GRADIENT_TOP_BOTTOM);
        }
        else
        {
            GFX_Set(GFXF_DRAW_COLOR, whl->widget.scheme->foregroundInactive);
            GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
        }
            
        if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
        {
            GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);

            GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);

            GFX_DrawRect(drawRect.x,
                         drawRect.y,
                         drawRect.width,
                         drawRect.height);
        }

        GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_FALSE);
    }
   
    nextState(whl);    
}

static void drawString(laListWheelWidget* whl)
{
    GFX_Rect textRect, drawRect, widgetRect, clipRect;
    laListWheelItem* item;
    laLayer* layer = laUtils_GetLayer((laWidget*)whl);
 
    //if(whl->paintState.nextItem < whl->items.size)
    //    nextItem(whl);
    
    if(!(whl->paintState.y < whl->visibleItems + 1))
    {
        nextState(whl);

        return;
    }
    
    item = whl->items.values[whl->paintState.nextItem];
    
    //printf("drawing item: %i, rot=%i\n", whl->paintState.nextItem, whl->rotation);
    
    if(laString_IsEmpty(&item->string) == LA_TRUE)
    {
        nextItem(whl);
        
        return;
    }
    
    _laListWheelWidget_GetItemTextRect(whl,
                                       whl->paintState.nextItem,
                                       whl->paintState.y,
                                       &textRect,
                                       &drawRect);
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)whl);
    
    if(GFX_RectIntersects(&drawRect, &widgetRect) == GFX_FALSE)
    {
        nextItem(whl);
        
        return;
    }
    
    // clip draw rect to widget rect
    GFX_RectClip(&drawRect, &widgetRect, &clipRect);
    drawRect = clipRect;
    
    item = whl->items.values[whl->paintState.nextItem];

    if(GFX_RectIntersects(&layer->clippedDrawingRect, &drawRect) == GFX_TRUE)
    {        
        GFX_Rect midLineRect;
        GFX_Rect itemRect;
        
        midLineRect.y = widgetRect.y + widgetRect.height/2;
        midLineRect.x = widgetRect.x;
        midLineRect.height = 1;
        midLineRect.width = widgetRect.width;
        
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        
        _laListWheelWidget_GetItemRect(whl,
                                       whl->paintState.nextItem,
                                       whl->paintState.y,
                                       &itemRect);
        
        if (GFX_RectIntersects(&itemRect, &midLineRect) == GFX_TRUE)
        {
            GFX_Set(GFXF_DRAW_COLOR, whl->widget.scheme->textHighlightText);
            
            laString_DrawClipped(&item->string,
                                 clipRect.x,
                                 clipRect.y,
                                 clipRect.width,
                                 clipRect.height,
                                 textRect.x,
                                 textRect.y,
                                 &whl->reader);
            
            if(whl->reader != NULL)
            {
                whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
                whl->widget.drawState = WAIT_STRING;

                return;
            }
        }
        else if (whl->hideWheel == LA_FALSE)
        {
            GFX_Set(GFXF_DRAW_COLOR, whl->widget.scheme->text);
            
            if (whl->zoomEffects == LA_LISTWHEEL_ZOOM_EFFECT_NONE)
            {
                laString_DrawClipped(&item->string,
                                     clipRect.x,
                                     clipRect.y,
                                     clipRect.width,
                                     clipRect.height,
                                     textRect.x,
                                     textRect.y,
                                     &whl->reader);

                if(whl->reader != NULL)
                {
                    whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
                    whl->widget.drawState = WAIT_STRING;

                    return;
                }
            }
            else
            {
                GFX_PixelBuffer pixelBuff;
                void * buf;
                GFX_BlendMode blendMode;
                GFX_Rect newTextRect = textRect;

                //Pre-render to a buffer
                buf = laContext_GetActive()->memIntf.heap.calloc(1, 
                                                textRect.width * 
                                                textRect.height * 
                                                4);
                if (buf != NULL)
                {
                    GFX_PixelBufferCreate(textRect.width,
                                          textRect.height,
                                          GFX_COLOR_MODE_RGBA_8888,
                                          buf,
                                          &pixelBuff);
                    memset(pixelBuff.pixels, 0x0, pixelBuff.buffer_length);

                    GFX_Set(GFXF_DRAW_COLOR, GFX_ColorConvert(
                                                laContext_GetActive()->colorMode,
                                                GFX_COLOR_MODE_RGBA_8888,
                                                whl->widget.scheme->text));

                    GFX_Set(GFXF_DRAW_TARGET, &pixelBuff);

                    laString_DrawClipped(&item->string,
                                         0,
                                         0,
                                         textRect.width,
                                         textRect.height,
                                         0,
                                         0,
                                         &whl->reader);

                    if(whl->reader != NULL)
                    {
                        whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
                        whl->widget.drawState = WAIT_STRING;

                        laContext_GetActive()->memIntf.heap.free(buf);

                        return;
                    }

                    GFX_Set(GFXF_DRAW_TARGET, NULL);

                    //Process alpha channel
                    GFX_Set(GFXF_DRAW_RESIZE_MODE, GFX_RESIZE_BILINEAR);
                    GFX_Get(GFXF_DRAW_BLEND_MODE, &blendMode);
                    GFX_Set(GFXF_DRAW_BLEND_MODE, blendMode | GFX_BLEND_CHANNEL);

                    //Apply scale effects on text rectangle
                    _laListWheelWidget_ItemRectApplyEffects(whl,
                                                    textRect,
                                                    &newTextRect);

                    if (GFX_RectIntersects(&newTextRect,
                                           &layer->clippedDrawingRect) == GFX_TRUE)
                    {
                        GFX_Rect imgRect = {0}, imgSrcRect, bounds;

                        if (item->icon != NULL)
                        {
                            _laListWheelWidget_GetItemIconRect(whl,
                                                               whl->paintState.nextItem,
                                                               whl->paintState.y,
                                                               &imgRect,
                                                               &imgSrcRect);

                            _laListWheelWidget_ItemRectApplyEffects(whl, 
                                                                    imgRect,
                                                                    &imgRect);
                        }

                        bounds.x = 0;
                        bounds.y = 0;
                        bounds.width = whl->widget.rect.width;
                        bounds.height = textRect.height;

                        // arrange image rect
                        laUtils_ArrangeRectangleRelative(&newTextRect,
                                                        imgRect,
                                                        bounds,
                                                        whl->halign,
                                                        LA_VALIGN_MIDDLE,
                                                        whl->iconPos,
                                                        whl->widget.margin.left,
                                                        whl->widget.margin.top,
                                                        whl->widget.margin.right,
                                                        whl->widget.margin.bottom,
                                                        whl->iconMargin);
                        
                        laUtils_RectToLayerSpace((laWidget*)whl, &newTextRect); 

                        newTextRect.y += getItemY(whl, whl->paintState.y, bounds.height);        

                        GFX_RectClip(&newTextRect, 
                                     &layer->clippedDrawingRect,
                                     &newTextRect);


                        GFX_DrawStretchBlit(&pixelBuff,
                                            0,
                                            0,
                                            pixelBuff.size.width,
                                            pixelBuff.size.height,
                                            newTextRect.x,
                                            newTextRect.y,
                                            newTextRect.width,
                                            newTextRect.height);
                    }

                    GFX_Set(GFXF_DRAW_BLEND_MODE, blendMode);

                    laContext_GetActive()->memIntf.heap.free(buf);
                }
            }
        }
    }
    
    nextItem(whl);
}

static void waitString(laListWheelWidget* whl)
{
    if(whl->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        whl->reader->run(whl->reader);
        
        return;
    }
    
    // free the reader
    whl->reader->memIntf->heap.free(whl->reader);
    whl->reader = NULL;
    
    nextItem(whl);
    
    whl->widget.drawState = DRAW_STRING;
    whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;
}

static void drawIcon(laListWheelWidget* whl)
{
    GFX_Rect widgetRect, imgRect, imgSrcRect, clipRect;
	GFX_Bool alphaEnable;
	//int32_t y;
    laLayer* layer = laUtils_GetLayer((laWidget*)whl);
    laListWheelItem* item = whl->items.values[whl->paintState.nextItem];
    
    if(!(whl->paintState.y < whl->visibleItems + 1))
    {
        nextState(whl);

        return;
    }
    
    // skip if no icon
    if(item->icon == NULL)
    {
        nextItem(whl);
        
        return;
    }
    
    _laListWheelWidget_GetItemIconRect(whl,
                                       whl->paintState.nextItem,
                                       whl->paintState.y,
                                       &imgRect,
                                       &imgSrcRect);
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)whl);
    
    if(GFX_RectIntersects(&imgRect, &widgetRect) == GFX_TRUE)
    {
        // clip imgrect to widget rect
        imgRect = GFX_RectClipAdj(&imgRect, &widgetRect, &imgSrcRect);
        
        if(GFX_RectIntersects(&imgRect, &layer->clippedDrawingRect) == GFX_TRUE)
        {
            GFX_Rect midLineRect;
            GFX_Rect itemRect;
        
            midLineRect.y = widgetRect.y + widgetRect.height/2;
            midLineRect.x = widgetRect.x;
            midLineRect.height = 1;
            midLineRect.width = widgetRect.width;
            
            clipRect = GFX_RectClipAdj(&imgRect, &layer->clippedDrawingRect, &imgSrcRect);
            
            _laListWheelWidget_GetItemRect(whl,
                                       whl->paintState.nextItem,
                                       whl->paintState.y,
                                       &itemRect);
            
            if (GFX_RectIntersects(&itemRect, &midLineRect) == GFX_TRUE)
            {
				GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
				GFX_Set(GFXF_DRAW_ALPHA_ENABLE, whl->widget.alphaEnabled);

				GFXU_DrawImage(item->icon,
                               imgSrcRect.x,
                               imgSrcRect.y,
                               imgSrcRect.width,
                               imgSrcRect.height,
                               clipRect.x,
                               clipRect.y,
                               &laContext_GetActive()->memIntf,
                               &whl->reader);

				GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);

				if (whl->reader != NULL)
                {
                    whl->widget.drawState = WAIT_ICON;
                    whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitIcon;

                    return;
                }
            }
            else if (whl->hideWheel == LA_FALSE)
            {
                if (whl->zoomEffects == LA_LISTWHEEL_ZOOM_EFFECT_NONE)
                {
					GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
					GFX_Set(GFXF_DRAW_ALPHA_ENABLE, whl->widget.alphaEnabled);

					GFXU_DrawImage(item->icon,
                                   imgSrcRect.x,
                                   imgSrcRect.y,
                                   imgSrcRect.width,
                                   imgSrcRect.height,
                                   clipRect.x,
                                   clipRect.y,
                                   &laContext_GetActive()->memIntf,
                                   &whl->reader);

					GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);
					
					if (whl->reader != NULL)
                    {
                        whl->widget.drawState = WAIT_ICON;
                        whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitIcon;

                        return;
                    }                
                }
                else 
                {
                    GFX_PixelBuffer pixelBuff;
                    void * buf;
                    GFX_BlendMode blendMode;
                    GFX_Rect newImgRect = imgRect;

                    //Pre-render to a buffer
                    buf = laContext_GetActive()->memIntf.heap.calloc(1, 
                                                    imgRect.width * 
                                                    imgRect.height * 
                                                    4);
                    if (buf != NULL)
                    {
                        GFX_PixelBufferCreate(imgRect.width,
                                              imgRect.height,
                                              GFX_COLOR_MODE_RGBA_8888,
                                              buf,
                                              &pixelBuff);
                        memset(pixelBuff.pixels, 0x0, pixelBuff.buffer_length);

                        GFX_Set(GFXF_DRAW_COLOR, GFX_ColorConvert(
                                                    laContext_GetActive()->colorMode,
                                                    GFX_COLOR_MODE_RGBA_8888,
                                                    whl->widget.scheme->text));

                        GFX_Set(GFXF_DRAW_TARGET, &pixelBuff);

						GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
						GFX_Set(GFXF_DRAW_ALPHA_ENABLE, whl->widget.alphaEnabled);

						GFXU_DrawImage(item->icon,
                                       0,
                                       0,
                                       imgSrcRect.width,
                                       imgSrcRect.height,
                                       0,
                                       0,
                                       &laContext_GetActive()->memIntf,
                                       &whl->reader);

						GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);
						
						if (whl->reader != NULL)
                        {
                            whl->widget.drawState = WAIT_ICON;
                            whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitIcon;

                            laContext_GetActive()->memIntf.heap.free(buf);                        

                            return;
                        }

                        GFX_Set(GFXF_DRAW_TARGET, NULL);

                        //Process alpha channel
                        GFX_Set(GFXF_DRAW_RESIZE_MODE, GFX_RESIZE_BILINEAR);
                        GFX_Get(GFXF_DRAW_BLEND_MODE, &blendMode);
                        GFX_Set(GFXF_DRAW_BLEND_MODE, blendMode | GFX_BLEND_CHANNEL);

                        //Use draw blit to scale it up!
                        _laListWheelWidget_ItemRectApplyEffects(whl,
                                                        imgRect,
                                                        &newImgRect);

                        if (newImgRect.height != 0 &&
                            newImgRect.width != 0 && 
                            GFX_RectIntersects(&newImgRect,
                                               &layer->clippedDrawingRect) == GFX_TRUE)
                        {
                            GFX_Rect textRect, textSrcRect, bounds;

                            _laListWheelWidget_GetItemTextRect(whl,
                                           whl->paintState.nextItem,
                                           whl->paintState.y,
                                           &textRect,
                                           &textSrcRect);

                            _laListWheelWidget_ItemRectApplyEffects(whl, 
                                                                    textRect,
                                                                    &textRect);

                            bounds.x = 0;
                            bounds.y = 0;
                            bounds.width = whl->widget.rect.width;
                            bounds.height = textSrcRect.height;

                            // arrange image rect
                            laUtils_ArrangeRectangle(&newImgRect,
                                                     textRect,
                                                     bounds,
                                                     whl->halign,
                                                     LA_VALIGN_MIDDLE,
                                                     whl->iconPos,
                                                     whl->widget.margin.left,
                                                     whl->widget.margin.top,
                                                     whl->widget.margin.right,
                                                     whl->widget.margin.bottom,
                                                     whl->iconMargin); 
                            
                            laUtils_RectToLayerSpace((laWidget*)whl, &newImgRect); 

                            newImgRect.y += getItemY(whl, whl->paintState.y, bounds.height);                        

                            GFX_RectClip(&newImgRect, 
                                         &layer->clippedDrawingRect,
                                         &newImgRect);

                            GFX_DrawStretchBlit(&pixelBuff,
                                            0,
                                            0,
                                            pixelBuff.size.width,
                                            pixelBuff.size.height,
                                            newImgRect.x,
                                            newImgRect.y,
                                            newImgRect.width,
                                            newImgRect.height);
                        }

                        GFX_Set(GFXF_DRAW_BLEND_MODE, blendMode);

                        laContext_GetActive()->memIntf.heap.free(buf);
                    }
                }
            }
        }
    }
    
    nextItem(whl);
}

static void waitIcon(laListWheelWidget* whl)
{
    if(whl->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        whl->reader->run(whl->reader);
        
        return;
    }
    
    // free the reader
    whl->reader->memIntf->heap.free(whl->reader);
    whl->reader = NULL;
    
    nextItem(whl);
    
    whl->widget.drawState = DRAW_ICON;
    whl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawIcon;
}

static void drawIndicators(laListWheelWidget* whl)
{
    GFX_Rect rect, drawRect, clipRect;
    uint32_t halfHeight;
    uint32_t topLine;
    uint32_t bottomLine;
    laLayer* layer;
    
    layer = laUtils_GetLayer((laWidget*)whl);
    
    rect = laUtils_WidgetLocalRect((laWidget*)whl);
    halfHeight = rect.height / 2;
    
    laUtils_RectToLayerSpace((laWidget*)whl, &rect);
    
    topLine = (rect.y + halfHeight) - whl->indicatorArea;
    bottomLine = (rect.y + halfHeight) + whl->indicatorArea;

    GFX_Set(GFXF_DRAW_COLOR, whl->widget.scheme->foreground);
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
    
    // upper lines
    drawRect.x = rect.x;
    drawRect.y = topLine;
    drawRect.width = rect.width;
    drawRect.height = 1;
    
    // top outer line
    if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    drawRect.y = bottomLine;
    
    // bottom inner line
    if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    drawRect.y = topLine + 1;
    
    // bottom inner line
    if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    // bottom outer line
    drawRect.y = bottomLine + 1;
    
    if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    //if the widget has no border, draw the vertical indicator borders
    if (whl->widget.borderType == LA_WIDGET_BORDER_NONE)
    {
        //left line
        drawRect.x = rect.x;
        drawRect.y = topLine + 2;
        drawRect.width = 2;
        drawRect.height = bottomLine - topLine;
        
        if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
        {
            GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
        
            GFX_DrawRect(clipRect.x,
                         clipRect.y,
                         clipRect.width,
                         clipRect.height);
        }
        
        //right line
        drawRect.x = rect.x + rect.width - 2;
        drawRect.y = topLine + 2;
        drawRect.width = 2;
        drawRect.height = bottomLine - drawRect.y;
        
        if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
        {
            GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
        
            GFX_DrawRect(clipRect.x,
                         clipRect.y,
                         clipRect.width,
                         clipRect.height);
        }
        
        
    }
    
    nextState(whl);
}

static void drawBorder(laListWheelWidget* whl)
{
    if(whl->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)whl);
    else if(whl->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardLoweredBorder((laWidget*)whl);
    
    nextState(whl);
}

void _laListWheelWidget_Paint(laListWheelWidget* whl)
{
    laContext* context = laContext_GetActive();
    
    if(whl->widget.scheme == NULL)
    {
        whl->widget.drawState = DONE;
        
        return;
    }
    
    if(whl->widget.drawState == NOT_STARTED)
        nextState(whl);
    
    while(whl->widget.drawState != DONE)
    {
        whl->widget.drawFunc((laWidget*)whl);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           whl->widget.drawState == WAIT_STRING ||
           whl->widget.drawState == WAIT_ICON)
            break;
    }
}

#endif // LA_LISTWHEEL_WIDGET_ENABLED