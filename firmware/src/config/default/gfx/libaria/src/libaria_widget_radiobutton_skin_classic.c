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


#include "gfx/libaria/inc/libaria_widget_radiobutton.h"

#if LA_RADIOBUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#define MIN_CIRCLE_SIZE       15
#define MIN_CIRCLE_SIZE_HALF  6

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_IMAGE,
    WAIT_IMAGE,
    DRAW_STRING,
    WAIT_STRING,
    DRAW_BORDER,
};

void _laRadioButtonWidget_GetImageRect(laRadioButtonWidget* btn,
									   GFX_Rect* imgRect,
									   GFX_Rect* imgSrcRect)
{
    GFX_Rect textRect;
    GFX_Rect bounds = laUtils_WidgetLocalRect((laWidget*)btn);
    
    imgRect->x = 0;
    imgRect->y = 0;
    
    laString_GetRect(&btn->text, &textRect);
    
    // calculate image dimensions
    if(btn->selected == LA_TRUE)
    {
        if(btn->selectedImage != NULL)
        {
            imgRect->width = btn->selectedImage->width;
            imgRect->height = btn->selectedImage->height;
        }
        else
        {
            imgRect->width = btn->circleButtonSize;
            imgRect->height = btn->circleButtonSize;
        }
    }
    else
    {
        if(btn->unselectedImage != NULL)
        {
            imgRect->width = btn->unselectedImage->width;
            imgRect->height = btn->unselectedImage->height;
        }
        else
        {
            imgRect->width = btn->circleButtonSize;
            imgRect->height = btn->circleButtonSize;
        }
    }

	*imgSrcRect = *imgRect;
    
    laUtils_ArrangeRectangle(imgRect,
                             textRect,
                             bounds,
                             btn->halign,
                             btn->valign,
                             btn->imagePosition,
                             btn->widget.margin.left,
                             btn->widget.margin.top,
                             btn->widget.margin.right,
                             btn->widget.margin.bottom,
                             btn->imageMargin);

	*imgRect = GFX_RectClipAdj(imgRect, &bounds, imgSrcRect);

	// move the rect to layer space
	laUtils_RectToLayerSpace((laWidget*)btn, imgRect);
}

void _laRadioButtonWidget_GetTextRect(laRadioButtonWidget* btn,
								      GFX_Rect* textRect,
								      GFX_Rect* drawRect)
{
    GFX_Rect bounds;
    
    GFX_Rect imgRect = {0};
    
    bounds = laUtils_WidgetLocalRect((laWidget*)btn);
    
    laString_GetRect(&btn->text, textRect);
    
    // calculate image dimensions
    if(btn->selected == LA_TRUE)
    {
        if(btn->selectedImage != NULL)
        {
            imgRect.width = btn->selectedImage->width;
            imgRect.height = btn->selectedImage->height;
        }
        else
        {
            imgRect.width = btn->circleButtonSize;
            imgRect.height = btn->circleButtonSize;
        }
    }
    else
    {
        if(btn->unselectedImage != NULL)
        {
            imgRect.width = btn->unselectedImage->width;
            imgRect.height = btn->unselectedImage->height;
        }
        else
        {
            imgRect.width = btn->circleButtonSize;
            imgRect.height = btn->circleButtonSize;
        }
    }
    
    // arrange relative to image rect
    laUtils_ArrangeRectangleRelative(textRect,
                                     imgRect,
                                     bounds,
                                     btn->halign,
                                     btn->valign,
                                     btn->imagePosition,
                                     btn->widget.margin.left,
                                     btn->widget.margin.top,
                                     btn->widget.margin.right,
                                     btn->widget.margin.bottom,
                                     btn->imageMargin);

	GFX_RectClip(textRect, &bounds, drawRect);

	// move the rects to layer space
	laUtils_RectToLayerSpace((laWidget*)btn, textRect);
    laUtils_RectToLayerSpace((laWidget*)btn, drawRect);
}

static void drawBackground(laRadioButtonWidget* btn);
static void drawImage(laRadioButtonWidget* btn);
static void waitImage(laRadioButtonWidget* btn);
static void drawString(laRadioButtonWidget* btn);
static void waitString(laRadioButtonWidget* btn);
static void drawBorder(laRadioButtonWidget* btn);

static void nextState(laRadioButtonWidget* btn)
{
    switch(btn->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(btn->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                btn->widget.drawState = DRAW_BACKGROUND;
                btn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            btn->widget.drawState = DRAW_IMAGE;
            btn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawImage;

            return;
        }
        case DRAW_IMAGE:
        {            
            if(laString_IsEmpty(&btn->text) == LA_FALSE)
            {
                btn->widget.drawState = DRAW_STRING;
                btn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;

                return;
            }
        }
        case DRAW_STRING:
        {
            if(btn->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                btn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                btn->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            btn->widget.drawState = DONE;
            btn->widget.drawFunc = NULL;
        }
    }
}

static void drawCircle(laRadioButtonWidget* btn, GFX_Rect* rect, laBool filled)
{
    int x = rect->x;
    int y = rect->y;
    
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    
    // fill
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->background);
    GFX_DrawLine(x + 4, y + 2, x + 10, y + 2);
    GFX_DrawLine(x + 3, y + 3, x + 10, y + 3);
    GFX_DrawLine(x + 2, y + 4, x + 11, y + 4);
    GFX_DrawLine(x + 2, y + 5, x + 11, y + 5);
    GFX_DrawLine(x + 2, y + 6, x + 11, y + 6);
    GFX_DrawLine(x + 2, y + 7, x + 11, y + 7);
    GFX_DrawLine(x + 3, y + 8, x + 10, y + 8);
    GFX_DrawLine(x + 4, y + 9, x + 10, y + 9);
    GFX_DrawLine(x + 2, y + 10, x + 9, y + 10);
    GFX_DrawLine(x + 4, y + 11, x + 7, y + 11);
    
    // upper outer ring
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->shadow);
    GFX_DrawLine(x + 4, y + 0, x + 7, y + 0);
    GFX_DrawLine(x + 2, y + 1, x + 3, y + 1);
    GFX_DrawLine(x + 8, y + 1, x + 9, y + 1);
    GFX_DrawLine(x + 8, y + 1, x + 9, y + 1);
    GFX_DrawLine(x + 1, y + 2, x + 1, y + 3);
    GFX_DrawLine(x + 1, y + 8, x + 1, y + 9);
    GFX_DrawLine(x + 1, y + 2, x + 1, y + 3);
    GFX_DrawLine(x + 0, y + 4, x + 0, y + 7);
    
    // upper inner ring
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->shadowDark);
    GFX_DrawLine(x + 4, y + 1, x + 7, y + 1);
    GFX_DrawLine(x + 2, y + 2, x + 3, y + 2);
    GFX_DrawLine(x + 8, y + 2, x + 9, y + 2);
    GFX_DrawPixel(x + 2, y + 3);
    GFX_DrawLine(x + 1, y + 4, x + 1, y + 7);
    GFX_DrawPixel(x + 2, y + 8);
    
    // lower inner ring
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->highlight);
    GFX_DrawPixel(x + 9, y + 3);
    GFX_DrawLine(x + 10, y + 4, x + 10, y + 7);
    GFX_DrawPixel(x + 9, y + 8);
    GFX_DrawLine(x + 2, y + 9, x + 3, y + 9);
    GFX_DrawLine(x + 8, y + 9, x + 9, y + 9);    
    GFX_DrawLine(x + 4, y + 10, x + 7, y + 10);
    
    if(filled == LA_TRUE)
    {
        GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->foreground);
        GFX_DrawLine(x + 5, y + 4, x + 6, y + 4);
        GFX_DrawLine(x + 4, y + 5, x + 7, y + 5);
        GFX_DrawLine(x + 4, y + 6, x + 7, y + 6);
        GFX_DrawLine(x + 5, y + 7, x + 6, y + 7);
    }
}

static void drawCircleArcs(laRadioButtonWidget* btn, GFX_Rect* rect, laBool filled)
{
#define THICKNESS_DIV 9

    GFX_Point center;
    
    uint32_t outRadius = (rect->width < rect->height) ? 
                            rect->width/2 : rect->height/2;
    uint32_t thickness = (outRadius/THICKNESS_DIV > 1) ? outRadius/THICKNESS_DIV : 1;
    
    center.x = rect->x + rect->width/2;
    center.y = rect->y + rect->height/2;
    
    // upper outer ring
    GFX_Set(GFXF_DRAW_THICKNESS, thickness);
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->shadow);
    GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
            btn->widget.scheme->shadow & 0xffffff00, 
            btn->widget.scheme->shadow, 
            NULL, 
            NULL);    
    GFX_DrawArc(center.x, center.y, outRadius, 45, 180);
    
    // lower outer ring
    GFX_Set(GFXF_DRAW_THICKNESS, thickness*2);
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->highlightLight);
    GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
            btn->widget.scheme->highlightLight & 0xffffff00, 
            btn->widget.scheme->highlightLight, 
            NULL, 
            NULL);
    GFX_DrawArc(center.x, center.y, outRadius, 225, 180);
    
    
    // upper inner ring
    outRadius -= thickness;
    GFX_Set(GFXF_DRAW_THICKNESS, thickness*2);
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->shadowDark);
    GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
            btn->widget.scheme->shadowDark & 0xffffff00, 
            btn->widget.scheme->shadowDark, 
            NULL, 
            NULL);
    GFX_DrawArc(center.x, center.y, outRadius, 45, 180);            
    
    
    // lower inner ring
    outRadius -= thickness;
    GFX_Set(GFXF_DRAW_THICKNESS, thickness*2);
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->highlight);
    GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
            btn->widget.scheme->highlight & 0xffffff00, 
            btn->widget.scheme->highlight, 
            NULL, 
            NULL);
    GFX_DrawArc(center.x, center.y, outRadius, 225, 180); 
    
    //Center base circle
    outRadius -= thickness;
    GFX_Set(GFXF_DRAW_THICKNESS, outRadius);
    GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->background);
    GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
            btn->widget.scheme->background & 0xffffff00, 
            btn->widget.scheme->background, 
            NULL, 
            NULL);
    GFX_DrawArc(center.x, center.y, outRadius, 0, 360); 
    
    if(filled == LA_TRUE)
    {
        outRadius = ((outRadius * 2)/3 < 3) ? 3 : (outRadius * 2)/3;
        GFX_Set(GFXF_DRAW_THICKNESS, outRadius);
        GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->foreground);
        GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                btn->widget.scheme->foreground & 0xffffff00, 
                btn->widget.scheme->foreground, 
                NULL, 
                NULL);
        GFX_DrawArc(center.x, center.y, outRadius, 0, 360);         
    }
    
}

static void drawBackground(laRadioButtonWidget* btn)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)btn);

    nextState(btn);
}

static void drawImage(laRadioButtonWidget* btn)
{
    GFX_Rect imgRect, imgSrcRect, clipRect;
    GFXU_ImageAsset* img = NULL;
    laLayer* layer = laUtils_GetLayer((laWidget*)btn);
    
    _laRadioButtonWidget_GetImageRect(btn, &imgRect, &imgSrcRect);
    
    if(btn->selected == LA_FALSE)
        img = btn->unselectedImage;
    else
        img = btn->selectedImage;
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &imgRect) == GFX_TRUE)
    {
        clipRect = GFX_RectClipAdj(&imgRect, &layer->clippedDrawingRect, &imgSrcRect);
        
        if(img == NULL)
        {
            GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
			GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);

            if (btn->circleButtonSize < MIN_CIRCLE_SIZE)
            {
                drawCircle(btn, &imgRect, btn->selected);
            }
            else
            {
                drawCircleArcs(btn, &imgRect, btn->selected);
            }
            
            GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_FALSE);
        }
        else
        {
            GFXU_DrawImage(img,
                           imgSrcRect.x,
                           imgSrcRect.y,
                           imgSrcRect.width,
                           imgSrcRect.height,
                           clipRect.x,
                           clipRect.y,
                           &laContext_GetActive()->memIntf,
                           &btn->reader);
                           
            if(btn->reader != NULL)
            {  
                btn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitImage;
                btn->widget.drawState = WAIT_IMAGE;
                
                return;
            }
        }
    }
    
    nextState(btn);
}

static void waitImage(laRadioButtonWidget* btn)
{
    if(btn->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        btn->reader->run(btn->reader);

        return;
    }
    else
    {
        // free the reader
        btn->reader->memIntf->heap.free(btn->reader);
        btn->reader = NULL;
    }
            
    btn->widget.drawState = DRAW_IMAGE;
    
    nextState(btn);
}

static void drawString(laRadioButtonWidget* btn)
{
    GFX_Rect textRect, drawRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)btn);
    
    _laRadioButtonWidget_GetTextRect(btn, &textRect, &drawRect);
        
   if(GFX_RectIntersects(&drawRect, &layer->clippedDrawingRect) == GFX_TRUE)
   {
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
            
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        GFX_Set(GFXF_DRAW_COLOR, btn->widget.scheme->text);
        
        laString_DrawClipped(&btn->text,
                             clipRect.x,
                             clipRect.y,
                             clipRect.width,
                             clipRect.height,
                             textRect.x,
                             textRect.y,
                             &btn->reader);
                             
        if(btn->reader != NULL)
        {
            btn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
            btn->widget.drawState = WAIT_STRING;
        
            return;
        }
    }
    
    nextState(btn);
}

static void waitString(laRadioButtonWidget* btn)
{
    if(btn->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        btn->reader->run(btn->reader);
        
        return;
    }
    
    // free the reader
    btn->reader->memIntf->heap.free(btn->reader);
    btn->reader = NULL;
    
    btn->widget.drawState = DRAW_STRING;
    
    nextState(btn);
}

static void drawBorder(laRadioButtonWidget* btn)
{
    if(btn->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)btn);
    else if(btn->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)btn);
    
    nextState(btn);
}

void _laRadioButtonWidget_Paint(laRadioButtonWidget* btn)
{
    laContext* context = laContext_GetActive();
    
    if(btn->widget.scheme == NULL)
    {
        btn->widget.drawState = DONE;
        
        return;
    }
    
    if(btn->widget.drawState == NOT_STARTED)
        nextState(btn);
    
    while(btn->widget.drawState != DONE)
    {
        btn->widget.drawFunc((laWidget*)btn);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           btn->widget.drawState == WAIT_IMAGE ||
           btn->widget.drawState == WAIT_STRING)
            break;
    }
}

#endif // LA_RADIOBUTTON_WIDGET_ENABLED