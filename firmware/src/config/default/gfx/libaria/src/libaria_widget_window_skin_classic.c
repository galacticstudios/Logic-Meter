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


#include "gfx/libaria/inc/libaria_widget_window.h"

#if LA_WINDOW_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#define DEFAULT_BAR_HEIGHT 20

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_TITLE_BAR,
    DRAW_ICON,
    WAIT_ICON,
    DRAW_STRING,
    WAIT_STRING,
    DRAW_BORDER,
};

void laWindowWidget_GetTitleBarRect(laWindowWidget* win, GFX_Rect* barRect)
{
    barRect->x = win->widget.margin.left;
    
    barRect->y = win->widget.margin.top;
    
    barRect->width = win->widget.rect.width -
                     win->widget.margin.left -
                     win->widget.margin.right;
    
    barRect->height = laString_GetHeight(&win->title) +
                      win->widget.margin.top +
                      win->widget.margin.bottom;
                      
    if(barRect->height == 0)
        barRect->height = DEFAULT_BAR_HEIGHT;                      
}

void laWindowWidget_GetIconRect(laWindowWidget* win,
                                GFX_Rect* imgRect,
                                GFX_Rect* imgSrcRect)
{
    GFX_Rect barRect;
    
    imgRect->x = 0;
    imgRect->y = 0;
    
    if(win->icon == NULL)
    {
        imgRect->width = 0;
        imgRect->height = 0;
        
        imgSrcRect->x = 0;
        imgSrcRect->y = 0;
        imgSrcRect->width = 0;
        imgRect->height = 0;
    
        return;
    }
    
    imgRect->width = win->icon->width;
    imgRect->height = win->icon->height;
    
    *imgSrcRect = *imgRect;
    
    laWindowWidget_GetTitleBarRect(win, &barRect);
    
    barRect.y += 2;
    barRect.height -= 4;
    
    laUtils_ArrangeRectangleRelative(imgRect,
                                     GFX_Rect_Zero,
                                     barRect,
                                     LA_HALIGN_LEFT,
                                     LA_VALIGN_MIDDLE,
                                     LA_RELATIVE_POSITION_LEFTOF,
                                     win->widget.margin.left,
                                     win->widget.margin.top,
                                     win->widget.margin.left,
                                     win->widget.margin.bottom,
                                     win->iconMargin);
                                     
    *imgRect = GFX_RectClipAdj(imgRect, &barRect, imgSrcRect);    
    
    laUtils_RectToLayerSpace((laWidget*)win, imgRect);                                 
}

void laWindowWidget_GetTextRect(laWindowWidget* win,
                                GFX_Rect* textRect,
                                GFX_Rect* drawRect)
{
    GFX_Rect barRect, imageRect;
    
    laWindowWidget_GetTitleBarRect(win, &barRect);
    
    laString_GetRect(&win->title, textRect);
    
    imageRect = GFX_Rect_Zero;
    
    if(win->icon != NULL)
    {
        imageRect.width = win->icon->width;
        imageRect.height = win->icon->height;
    }
    
    // arrange relative to image rect
    laUtils_ArrangeRectangleRelative(textRect,
                                     imageRect,
                                     barRect,
                                     LA_HALIGN_LEFT,
                                     LA_VALIGN_MIDDLE,
                                     LA_RELATIVE_POSITION_LEFTOF,
                                     win->widget.margin.left,
                                     win->widget.margin.top,
                                     win->widget.margin.right,
                                     win->widget.margin.bottom,
                                     win->iconMargin);
                                      
    GFX_RectClip(textRect, &barRect, drawRect);
    
    laUtils_RectToLayerSpace((laWidget*)win, textRect);
    laUtils_RectToLayerSpace((laWidget*)win, drawRect);                                         
}

static void drawBackground(laWindowWidget* win);
static void drawTitleBar(laWindowWidget* win);
static void drawIcon(laWindowWidget* win);
static void waitIcon(laWindowWidget* win);
static void drawString(laWindowWidget* win);
static void waitString(laWindowWidget* win);
static void drawBorder(laWindowWidget* win);

static void nextState(laWindowWidget* win)
{
    switch(win->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(win->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                win->widget.drawState = DRAW_BACKGROUND;
                win->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            win->widget.drawState = DRAW_TITLE_BAR;
            win->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawTitleBar;
            
            return;
        }
        case DRAW_TITLE_BAR:
        {
            if(win->icon != NULL)
            {
                win->widget.drawState = DRAW_ICON;
                win->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawIcon;
                
                return;
            }
        }
        case DRAW_ICON:
        {
            if(laString_IsEmpty(&win->title) == LA_FALSE)
            {
                win->widget.drawState = DRAW_STRING;
                win->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;
                
                return;
            }
        }
        case DRAW_STRING:
        {
            if(win->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                win->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                win->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            win->widget.drawState = DONE;
            win->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laWindowWidget* win)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)win);
    
    nextState(win);
}

static void drawTitleBar(laWindowWidget* win)
{
    GFX_Rect barRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)win);
    
    laWindowWidget_GetTitleBarRect(win, &barRect);
    laUtils_RectToLayerSpace((laWidget*)win, &barRect);
    
    laWidget_SkinClassic_FillRect(&barRect,
                                  win->widget.scheme->background,
                                  &layer->clippedDrawingRect);
    
    // draw title bar border
    /*if(win->widget.borderType == LA_WIDGET_BORDER_LINE)
    {
        laDraw_LineBorder(&barRect, win->widget.scheme->shadowDark);
    }
    else if(win->widget.borderType == LA_WIDGET_BORDER_BEVEL)
    {
        laDraw_2x2BevelBorder(&barRect,
                              win->widget.scheme->base,
                              win->widget.scheme->highlightLight,
                              win->widget.scheme->shadow,
                              win->widget.scheme->shadowDark);
    }*/
    
    nextState(win);
}

static void drawIcon(laWindowWidget* win)
{
    GFX_Rect iconRect, imgSrcRect, clipRect;
	GFX_Bool alphaEnable;

    laLayer* layer = laUtils_GetLayer((laWidget*)win);
    
    laWindowWidget_GetIconRect(win, &iconRect, &imgSrcRect);
        
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &iconRect) == GFX_TRUE)
    {
        clipRect = GFX_RectClipAdj(&iconRect, &layer->clippedDrawingRect, &imgSrcRect);
        
		GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
		GFX_Set(GFXF_DRAW_ALPHA_ENABLE, win->widget.alphaEnabled);

		GFXU_DrawImage(win->icon,
                       imgSrcRect.x,
                       imgSrcRect.y,
                       imgSrcRect.width,
                       imgSrcRect.height,
                       clipRect.x,
                       clipRect.y,
                       &laContext_GetActive()->memIntf,
                       &win->reader);
                       
		GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);

		if (win->reader != NULL)
        {
            win->widget.drawState = WAIT_ICON;
            win->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitIcon;
        
            return;
        }
    }
        
    nextState(win);
}

static void waitIcon(laWindowWidget* win)
{
    if(win->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        win->reader->run(win->reader);
        
        return;
    }
    
    // free the reader
    win->reader->memIntf->heap.free(win->reader);
    win->reader = NULL;
    
    win->widget.drawState = DRAW_ICON;
    
    nextState(win);
}

static void drawString(laWindowWidget* win)
{
    GFX_Rect textRect, drawRect, clipRect;
    
    laLayer* layer = laUtils_GetLayer((laWidget*)win);
    
    laWindowWidget_GetTextRect(win, &textRect, &drawRect);
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &drawRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        GFX_Set(GFXF_DRAW_COLOR, win->widget.scheme->text);
        
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
            
        laString_DrawClipped(&win->title,
                             clipRect.x,
                             clipRect.y,
                             clipRect.width,
                             clipRect.height,
                             textRect.x,
                             textRect.y,
                             &win->reader);
    
        if(win->reader != NULL)
        {
            win->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
            win->widget.drawState = WAIT_STRING;
            
            return;
        }
    }
    
    nextState(win);
}

static void waitString(laWindowWidget* win)
{
    if(win->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        win->reader->run(win->reader);
        
        return;
    }
    
    // free the reader
    win->reader->memIntf->heap.free(win->reader);
    win->reader = NULL;
    
    win->widget.drawState = DRAW_STRING;
    
    nextState(win);
}

static void drawBorder(laWindowWidget* win)
{
    GFX_Rect rect;
    laLayer* layer;
    
    if(win->widget.borderType == LA_WIDGET_BORDER_LINE)
    {
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)win);
    }
    else if(win->widget.borderType == LA_WIDGET_BORDER_BEVEL)
    {
        rect = laUtils_WidgetLocalRect((laWidget*)win);
        layer = laUtils_GetLayer((laWidget*)win);
        
        laUtils_RectToLayerSpace((laWidget*)win, &rect);
        
        laWidget_SkinClassic_Draw2x2BeveledBorder(&rect,
                                                  win->widget.scheme->base,
                                                  win->widget.scheme->highlightLight,
                                                  win->widget.scheme->shadowDark,
                                                  win->widget.scheme->shadow,
                                                  &layer->clippedDrawingRect);
    }
    
    nextState(win);
}

void _laWindowWidget_Paint(laWindowWidget* win)
{
    laContext* context = laContext_GetActive();
    
    if(win->widget.scheme == NULL)
    {
        win->widget.drawState = DONE;
        
        return;
    }
    
    if(win->widget.drawState == NOT_STARTED)
        nextState(win);
    
    while(win->widget.drawState != DONE)
    {
        win->widget.drawFunc((laWidget*)win);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           win->widget.drawState == WAIT_ICON ||
           win->widget.drawState == WAIT_STRING)
            break;
    }
}

#endif // LA_WINDOW_WIDGET_ENABLED