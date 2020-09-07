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


#include "gfx/libaria/inc/libaria_widget_textfield.h"

#if LA_TEXTFIELD_WIDGET_ENABLED

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
    DRAW_CURSOR,
    DRAW_BORDER,
};

void _laTextFieldWidget_GetTextRect(laTextFieldWidget* txt,
                                    GFX_Rect* textRect,
                                    GFX_Rect* drawRect)
{
    GFX_Rect bounds = laUtils_WidgetLocalRect((laWidget*)txt);
    
    laString_GetRect(&txt->text, textRect);
    
    laUtils_ArrangeRectangleRelative(textRect,
                                     GFX_Rect_Zero,
                                     bounds,
                                     txt->halign,
                                     LA_VALIGN_MIDDLE,
                                     0,
                                     txt->editWidget.widget.margin.left,
                                     txt->editWidget.widget.margin.top,
                                     txt->editWidget.widget.margin.right,
                                     txt->editWidget.widget.margin.bottom,
                                     0);
                                     
    GFX_RectClip(textRect, &bounds, drawRect);
    
    laUtils_RectToLayerSpace((laWidget*)txt, textRect);
    laUtils_RectToLayerSpace((laWidget*)txt, drawRect);
}

void _laTextFieldWidget_GetCursorRect(laTextFieldWidget* txt, GFX_Rect* cursorRect)
{
    GFX_Rect textRect;
    GFX_Rect bounds = laUtils_WidgetLocalRect((laWidget*)txt);
    
    laString_GetRect(&txt->text, &textRect);
    
    laUtils_ArrangeRectangleRelative(&textRect,
                                     GFX_Rect_Zero,
                                     bounds,
                                     txt->halign,
                                     LA_VALIGN_MIDDLE,
                                     0,
                                     txt->editWidget.widget.margin.left,
                                     txt->editWidget.widget.margin.top,
                                     txt->editWidget.widget.margin.right,
                                     txt->editWidget.widget.margin.bottom,
                                     0);
    
    if(textRect.height == 0)
    {
        textRect.y = bounds.y + txt->editWidget.widget.margin.top + 2;
        textRect.height = bounds.height - txt->editWidget.widget.margin.top - txt->editWidget.widget.margin.bottom - 4;
    }
    
    cursorRect->x = textRect.x + laString_GetCharOffset(&txt->text, txt->cursorPos);
    cursorRect->y = textRect.y;
    cursorRect->width = 1;
    cursorRect->height = textRect.height;
    
    laUtils_RectToLayerSpace((laWidget*)txt, cursorRect);
}

static void drawBackground(laTextFieldWidget* txt);
static void drawString(laTextFieldWidget* txt);
static void waitString(laTextFieldWidget* txt);
static void drawCursor(laTextFieldWidget* txt);
static void drawBorder(laTextFieldWidget* txt);

static void nextState(laTextFieldWidget* txt)
{
    switch(txt->editWidget.widget.drawState)
    {
        case NOT_STARTED:
        {
            if(txt->editWidget.widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                txt->editWidget.widget.drawState = DRAW_BACKGROUND;
                txt->editWidget.widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            if(laString_IsEmpty(&txt->text) == LA_FALSE)
            {
                txt->editWidget.widget.drawState = DRAW_STRING;
                txt->editWidget.widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;
                
                return;
            }
        }
        case DRAW_STRING:
        {
            if(txt->cursorEnable == LA_TRUE && txt->cursorVisible == LA_TRUE)
            {
                txt->editWidget.widget.drawState = DRAW_CURSOR;
                txt->editWidget.widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawCursor;
                
                return; 
            }
        }
        case DRAW_CURSOR:
        {
            if(txt->editWidget.widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                txt->editWidget.widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                txt->editWidget.widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            txt->editWidget.widget.drawState = DONE;
            txt->editWidget.widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laTextFieldWidget* txt)
{
    if(txt->editWidget.widget.backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        laWidget_SkinClassic_DrawBackground((laWidget*)txt, txt->editWidget.widget.scheme->background);
    }
    else if(txt->editWidget.widget.backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        laWidget_SkinClassic_DrawBlit((laWidget*)txt, txt->editWidget.widget.cache);
    }
    
    nextState(txt);
}

static void drawString(laTextFieldWidget* txt)
{
    GFX_Rect textRect, drawRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)txt);
    
    _laTextFieldWidget_GetTextRect(txt, &textRect, &drawRect);
    
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    GFX_Set(GFXF_DRAW_COLOR, txt->editWidget.widget.scheme->text);
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &drawRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
            
        laString_DrawClipped(&txt->text,
                             clipRect.x,
                             clipRect.y,
                             clipRect.width,
                             clipRect.height,
                             textRect.x,
                             textRect.y,
                             &txt->reader);
                             
        if(txt->reader != NULL)
        {
            txt->editWidget.widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
            txt->editWidget.widget.drawState = WAIT_STRING;
            
            return;
        }
    }
    
    nextState(txt);
}

static void waitString(laTextFieldWidget* txt)
{
    if(txt->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        txt->reader->run(txt->reader);
        
        return;
    }
    
    // free the reader
    txt->reader->memIntf->heap.free(txt->reader);
    txt->reader = NULL;
    
    txt->editWidget.widget.drawState = DRAW_STRING;
    
    nextState(txt);
}

static void drawCursor(laTextFieldWidget* txt)
{
    GFX_Rect cursorRect, clipRect;
    
    laLayer* layer = laUtils_GetLayer((laWidget*)txt);
    
    _laTextFieldWidget_GetCursorRect(txt, &cursorRect);
    
    // draw cursor line
    if(GFX_RectIntersects(&cursorRect, &layer->clippedDrawingRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        GFX_Set(GFXF_DRAW_COLOR, txt->editWidget.widget.scheme->foreground);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
    
        GFX_RectClip(&cursorRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    nextState(txt);
}

static void drawBorder(laTextFieldWidget* txt)
{
    if(txt->editWidget.widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)txt);
    else if(txt->editWidget.widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardLoweredBorder((laWidget*)txt);
    
    nextState(txt);
}

void _laTextFieldWidget_Paint(laTextFieldWidget* txt)
{
    laContext* context = laContext_GetActive();
    
    if(txt->editWidget.widget.scheme == NULL)
    {
        txt->editWidget.widget.drawState = DONE;
        
        return;
    }

    if(txt->editWidget.widget.drawState == NOT_STARTED)
        nextState(txt);
    
    while(txt->editWidget.widget.drawState != DONE)
    {
        txt->editWidget.widget.drawFunc((laWidget*)txt);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           txt->editWidget.widget.drawState == WAIT_STRING)
            break;
    }
}

#endif // LA_TEXTFIELD_WIDGET_ENABLED