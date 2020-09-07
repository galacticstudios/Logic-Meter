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


#include "gfx/libaria/inc/libaria_widget_groupbox.h"

#if LA_GROUPBOX_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#define OUTLINE_SPACE 2

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_OUTLINE,
    DRAW_STRING,
    WAIT_STRING,
    DRAW_BORDER,
};

void _laGroupBoxWidget_GetTextRect(laGroupBoxWidget* box,
								   GFX_Rect* textRect,
								   GFX_Rect* drawRect)
{
    GFX_Rect bounds = laUtils_WidgetLocalRect((laWidget*)box);
    
    laString_GetRect(&box->text, textRect);
    
    //boundRect = box->widget.rect;
    bounds.x += (OUTLINE_SPACE * 3) + 2;
    bounds.y += OUTLINE_SPACE;
    bounds.width -= ((OUTLINE_SPACE * 3) + 2) * 2;
    bounds.height -= OUTLINE_SPACE * 2;
    
    // arrange relative to image rect
    laUtils_ArrangeRectangleRelative(textRect,
                                     GFX_Rect_Zero,
                                     bounds,
                                     box->halign,
                                     LA_VALIGN_TOP,
                                     0,
                                     0,
                                     0,
                                     0,
                                     0,
                                     0);
    
    GFX_RectClip(textRect, &bounds, drawRect);
    
    // move the rect to layer space   
    laUtils_RectToLayerSpace((laWidget*)box, textRect);
    laUtils_RectToLayerSpace((laWidget*)box, drawRect);
}

static void drawBackground(laGroupBoxWidget* box);
static void drawOutline(laGroupBoxWidget* box);
static void drawString(laGroupBoxWidget* box);
static void waitString(laGroupBoxWidget* box);
static void drawBorder(laGroupBoxWidget* box);

static void nextState(laGroupBoxWidget* box)
{
    switch(box->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(box->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                box->widget.drawState = DRAW_BACKGROUND;
                box->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            box->widget.drawState = DRAW_OUTLINE;
            box->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawOutline;

            return;
        }
        case DRAW_OUTLINE:
        {            
            if(laString_IsEmpty(&box->text) == LA_FALSE)
            {
                box->widget.drawState = DRAW_STRING;
                box->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;

                return;
            }
        }
        case DRAW_STRING:
        {
            if(box->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                box->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                box->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            box->widget.drawState = DONE;
            box->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laGroupBoxWidget* box)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)box);

    nextState(box);
}

static void drawLine(GFX_Rect* rect,
                     int32_t x1,
                     int32_t y1,
                     int32_t x2,
                     int32_t y2)
{
    GFX_Rect lineRect, clipRect;
    
    lineRect.x = x1;
    lineRect.y = y1;
    lineRect.width = x2 - x1;
    lineRect.height = y2 - y1;
    
    if(GFX_RectIntersects(&lineRect, rect) == GFX_FALSE)
        return;

    GFX_RectClip(&lineRect, rect, &clipRect);

    GFX_DrawLine(clipRect.x,
                 clipRect.y,
                 clipRect.x + clipRect.width,
                 clipRect.y + clipRect.height);
}

static void drawOutline(laGroupBoxWidget* box)
{
    GFX_Rect widgetRect, textRect; 
    int32_t top, bottom, left, right;
    laLayer* layer = laUtils_GetLayer((laWidget*)box);
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)box);
    
    laString_GetRect(&box->text, &textRect);
    
    // draw vertical lines
    top = widgetRect.y + textRect.height / 2;   
    bottom = widgetRect.y + widgetRect.height - OUTLINE_SPACE;

    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    
    GFX_Set(GFXF_DRAW_COLOR, box->widget.scheme->shadow);
    
    // left outer line
    drawLine(&layer->clippedDrawingRect,
             widgetRect.x + OUTLINE_SPACE,
             top,
             widgetRect.x + OUTLINE_SPACE,
             bottom);
    
    // right inner line
    drawLine(&layer->clippedDrawingRect,
             widgetRect.x + box->widget.rect.width - OUTLINE_SPACE - 2,
             top,
             widgetRect.x + box->widget.rect.width - OUTLINE_SPACE - 2,
             bottom - 1);
             
    GFX_Set(GFXF_DRAW_COLOR, box->widget.scheme->highlightLight);
    
    // left inner line
    drawLine(&layer->clippedDrawingRect,
             widgetRect.x + OUTLINE_SPACE + 1,
             top + 1,
             widgetRect.x + OUTLINE_SPACE + 1,
             bottom - 1);
             
    // right outer line
    drawLine(&layer->clippedDrawingRect,
             widgetRect.x + box->widget.rect.width - OUTLINE_SPACE - 1,
             top,
             widgetRect.x + box->widget.rect.width - OUTLINE_SPACE - 1,
             bottom);
             
    if(box->halign == LA_HALIGN_LEFT)
    {   
        left = OUTLINE_SPACE * 3;
        right = left + textRect.width + 1;
        
        if(right > box->widget.rect.width - OUTLINE_SPACE * 3)
            right = box->widget.rect.width - OUTLINE_SPACE * 3;
    }
    else if(box->halign == LA_HALIGN_CENTER)
    {
        left = box->widget.rect.width / 2 - textRect.width / 2 - 2;
        right = left + textRect.width + 3;
    }
    else
    {
        right = box->widget.rect.width - OUTLINE_SPACE * 3;
        left = right - textRect.width - 1;
        
        if(left < OUTLINE_SPACE * 3)
            left = OUTLINE_SPACE * 3;
    }
             
    // draw horizontal lines
    GFX_Set(GFXF_DRAW_COLOR, box->widget.scheme->shadow);
    
    left += widgetRect.x;
    right += widgetRect.x;
    
    // top outer line left segment
    drawLine(&layer->clippedDrawingRect,
             widgetRect.x + OUTLINE_SPACE + 1,
             top,
             left,
             top);
             
    // top outer line right segment
    drawLine(&layer->clippedDrawingRect,
             right,
             top,
             widgetRect.x + widgetRect.width - OUTLINE_SPACE * 2,
             top);
                 
    // bottom inner line
    drawLine(&layer->clippedDrawingRect,
             widgetRect.x + OUTLINE_SPACE + 1,
             bottom - 1,
             widgetRect.x + widgetRect.width - (OUTLINE_SPACE * 2),
             bottom - 1);
             
    GFX_Set(GFXF_DRAW_COLOR, box->widget.scheme->highlightLight);
    
    // top inner line left segment
    drawLine(&layer->clippedDrawingRect,
             widgetRect.x + OUTLINE_SPACE * 2,
             top + 1,
             left,
             top + 1);
             
    // top inner line right segment
    drawLine(&layer->clippedDrawingRect,
             right,
             top + 1,
             widgetRect.x + widgetRect.width - (OUTLINE_SPACE * 2) - 1,
             top + 1);
             
    // bottom outer line
    drawLine(&layer->clippedDrawingRect,
             widgetRect.x + OUTLINE_SPACE,
             bottom,
             widgetRect.x + widgetRect.width - (OUTLINE_SPACE * 2),
             bottom);
           
    nextState(box);
}

static void drawString(laGroupBoxWidget* box)
{
    GFX_Rect textRect, drawRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)box);
    
    _laGroupBoxWidget_GetTextRect(box, &textRect, &drawRect);
        
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    GFX_Set(GFXF_DRAW_COLOR, box->widget.scheme->text);
        
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &drawRect) == GFX_TRUE)
    {    
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
    
        laString_DrawClipped(&box->text,
                             clipRect.x,
                             clipRect.y,
                             clipRect.width,
                             clipRect.height,
                             textRect.x,
                             textRect.y,
                             &box->reader);
    
        if(box->reader != NULL)
        {
            box->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
            box->widget.drawState = WAIT_STRING;
            
            return;
        }
    }
    
    nextState(box);
}

static void waitString(laGroupBoxWidget* box)
{
    if(box->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        box->reader->run(box->reader);
        
        return;
    }
    
    // free the reader
    box->reader->memIntf->heap.free(box->reader);
    box->reader = NULL;
    
    box->widget.drawState = DRAW_STRING;
    
    nextState(box);
}

static void drawBorder(laGroupBoxWidget* box)
{
    if(box->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)box);
    else if(box->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)box);
        
    nextState(box);
}

void _laGroupBoxWidget_Paint(laGroupBoxWidget* box)
{
    laContext* context = laContext_GetActive();
    
    if(box->widget.scheme == NULL)
    {
        box->widget.drawState = DONE;
        
        return;
    }
    
    if(box->widget.drawState == NOT_STARTED)
        nextState(box);
    
    while(box->widget.drawState != DONE)
    {
        box->widget.drawFunc((laWidget*)box);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           box->widget.drawState == WAIT_STRING)
            break;
    }
}

#endif // LA_GROUPBOX_WIDGET_ENABLED