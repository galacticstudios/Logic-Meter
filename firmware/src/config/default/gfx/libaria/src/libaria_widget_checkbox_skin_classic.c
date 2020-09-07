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


#include "gfx/libaria/inc/libaria_widget_checkbox.h"

#if LA_CHECKBOX_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_scheme.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#define CHECKBOX_SIZE       13
#define CHECKBOX_SIZE_HALF  6

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_IMAGE,
    WAIT_IMAGE,
    DRAW_STRING,
    WAIT_STRING,
    DRAW_BORDER
};

void _laCheckBoxWidget_GetImageRect(laCheckBoxWidget* cbox,
									GFX_Rect* imgRect,
									GFX_Rect* imgSrcRect)
{
    GFX_Rect textRect;
    GFX_Rect bounds = laUtils_WidgetLocalRect((laWidget*)cbox);
    
    imgRect->x = 0;
    imgRect->y = 0;
    
    laString_GetRect(&cbox->text, &textRect);
    
    if(cbox->checked == LA_FALSE)
    {
        if(cbox->uncheckedImage == NULL)
        {
            imgRect->width = CHECKBOX_SIZE;
            imgRect->height = CHECKBOX_SIZE;
        }
        else
        {
            imgRect->width = cbox->uncheckedImage->width;
            imgRect->height = cbox->uncheckedImage->height;
        }
    }
    else
    {
        if(cbox->checkedImage == NULL)
        {
            imgRect->width = CHECKBOX_SIZE;
            imgRect->height = CHECKBOX_SIZE;
        }
        else
        {
            imgRect->width = cbox->checkedImage->width;
            imgRect->height = cbox->checkedImage->height;
        }
    }

	*imgSrcRect = *imgRect;
    
    laUtils_ArrangeRectangle(imgRect,
                             textRect,
                             bounds,
                             cbox->halign,
                             cbox->valign,
                             cbox->imagePosition,
                             cbox->widget.margin.left,
                             cbox->widget.margin.top,
                             cbox->widget.margin.right,
                             cbox->widget.margin.bottom,
                             cbox->imageMargin);

	*imgRect = GFX_RectClipAdj(imgRect, &bounds, imgSrcRect);

	// move the rect to layer space
	laUtils_RectToLayerSpace((laWidget*)cbox, imgRect);
}

void _laCheckBoxWidget_GetTextRect(laCheckBoxWidget* cbox,
								   GFX_Rect* textRect,
								   GFX_Rect* drawRect)
{
    GFX_Rect bounds;
    
    GFX_Rect imgRect = {0};
    
    bounds = laUtils_WidgetLocalRect((laWidget*)cbox);
    
    laString_GetRect(&cbox->text, textRect);
    
    // calculate image dimensions
    if(cbox->checked == LA_TRUE)
    {
        if(cbox->checkedImage != NULL)
        {
            imgRect.width = cbox->checkedImage->width;
            imgRect.height = cbox->checkedImage->height;
        }
        else
        {
            imgRect.width = CHECKBOX_SIZE;
            imgRect.height = CHECKBOX_SIZE;
        }
    }
    else
    {
        if(cbox->uncheckedImage != NULL)
        {
            imgRect.width = cbox->uncheckedImage->width;
            imgRect.height = cbox->uncheckedImage->height;
        }
        else
        {
            imgRect.width = CHECKBOX_SIZE;
            imgRect.height = CHECKBOX_SIZE;
        }
    }
    
    // arrange relative to image rect
    laUtils_ArrangeRectangleRelative(textRect,
                                     imgRect,
                                     bounds,
                                     cbox->halign,
                                     cbox->valign,
                                     cbox->imagePosition,
                                     cbox->widget.margin.left,
                                     cbox->widget.margin.top,
                                     cbox->widget.margin.right,
                                     cbox->widget.margin.bottom,
                                     cbox->imageMargin);

	GFX_RectClip(textRect, &bounds, drawRect);

	// move the rects to layer space
	laUtils_RectToLayerSpace((laWidget*)cbox, textRect);
    laUtils_RectToLayerSpace((laWidget*)cbox, drawRect);
}

static void drawBackground(laCheckBoxWidget* cbox);
static void drawImage(laCheckBoxWidget* cbox);
static void waitImage(laCheckBoxWidget* cbox);
static void drawString(laCheckBoxWidget* cbox);
static void waitString(laCheckBoxWidget* cbox);
static void drawBorder(laCheckBoxWidget* cbox);

static void nextState(laCheckBoxWidget* cbox)
{
    switch(cbox->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(cbox->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                cbox->widget.drawState = DRAW_BACKGROUND;
                cbox->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            cbox->widget.drawState = DRAW_IMAGE;
            cbox->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawImage;

            return;
        }
        case DRAW_IMAGE:
        {            
            if(laString_IsEmpty(&cbox->text) == LA_FALSE)
            {
                cbox->widget.drawState = DRAW_STRING;
                cbox->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawString;

                return;
            }
        }
        case DRAW_STRING:
        {
            if(cbox->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                cbox->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                cbox->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            cbox->widget.drawState = DONE;
            cbox->widget.drawFunc = NULL;
        }
    }
}

static void drawCheckBox(laCheckBoxWidget* cbox, GFX_Rect* rect)
{
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    
    // draw box fill
    GFX_Set(GFXF_DRAW_COLOR, cbox->widget.scheme->background);
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
    GFX_DrawRect(rect->x,
                 rect->y,
                 CHECKBOX_SIZE,
                 CHECKBOX_SIZE);
    
    // draw box border
    laDraw_2x2BevelBorder(rect,
                          cbox->widget.scheme->shadow,
                          cbox->widget.scheme->shadowDark,
                          cbox->widget.scheme->highlight,
                          cbox->widget.scheme->highlightLight);
        
    if(cbox->checked == LA_TRUE)
    {
        GFX_Set(GFXF_DRAW_COLOR, cbox->widget.scheme->foreground);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_THICKNESS, 3);
        
        GFX_DrawLine(rect->x + 3, rect->y + 5,
                     rect->x + 5, rect->y + 7);
        GFX_DrawLine(rect->x + 5, rect->y + 7,
                     rect->x + 9, rect->y + 3);
        
        GFX_DrawLine(rect->x + 3, rect->y + 6,
                     rect->x + 5, rect->y + 8);
        GFX_DrawLine(rect->x + 5, rect->y + 8,
                     rect->x + 9, rect->y + 4);
                     
        GFX_DrawLine(rect->x + 3, rect->y + 7,
                     rect->x + 5, rect->y + 9);
        GFX_DrawLine(rect->x + 5, rect->y + 9,
                     rect->x + 9, rect->y + 5);
    }
}

static void drawBackground(laCheckBoxWidget* cbox)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)cbox);

    nextState(cbox);
}

static void drawImage(laCheckBoxWidget* cbox)
{
	GFXU_ImageAsset* img = NULL;
	GFX_Rect imgRect, imgSrcRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)cbox);
	GFX_Bool alphaEnable;

	_laCheckBoxWidget_GetImageRect(cbox, &imgRect, &imgSrcRect);
	
	if(cbox->checked == LA_FALSE)
        img = cbox->uncheckedImage;
    else
        img = cbox->checkedImage;
        
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &imgRect) == GFX_TRUE)
    {
        clipRect = GFX_RectClipAdj(&imgRect, &layer->clippedDrawingRect, &imgSrcRect);
        
        if(img == NULL)
        {
			GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
			GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);

            drawCheckBox(cbox, &imgRect);

			GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_FALSE);
        }
        else
        {
			GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
			GFX_Set(GFXF_DRAW_ALPHA_ENABLE, cbox->widget.alphaEnabled);

			GFXU_DrawImage(img,
                           imgSrcRect.x,
                           imgSrcRect.y,
                           imgSrcRect.width,
                           imgSrcRect.height,
                           clipRect.x,
                           clipRect.y,
                           &laContext_GetActive()->memIntf,
                           &cbox->reader);
                           
			GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);

			if (cbox->reader != NULL)
            {  
                cbox->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitImage;
                cbox->widget.drawState = WAIT_IMAGE;
                
                return;
            }
        }
    }
    
    nextState(cbox);
}

static void waitImage(laCheckBoxWidget* cbox)
{
    if(cbox->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        cbox->reader->run(cbox->reader);

        return;
    }
    else
    {
        // free the reader
        cbox->reader->memIntf->heap.free(cbox->reader);
        cbox->reader = NULL;
    }
            
    cbox->widget.drawState = DRAW_IMAGE;
    
    nextState(cbox);
}

static void drawString(laCheckBoxWidget* cbox)
{
	GFX_Rect textRect, drawRect, clipRect;
	laLayer* layer = laUtils_GetLayer((laWidget*)cbox);

	_laCheckBoxWidget_GetTextRect(cbox, &textRect, &drawRect);

    if(GFX_RectIntersects(&layer->clippedDrawingRect, &drawRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, &layer->clippedDrawingRect, &clipRect);
                    
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        GFX_Set(GFXF_DRAW_COLOR, cbox->widget.scheme->text);
        
        laString_DrawClipped(&cbox->text,
                             clipRect.x,
                             clipRect.y,
                             clipRect.width,
                             clipRect.height,
                             textRect.x,
                             textRect.y,
                             &cbox->reader);
    
        if(cbox->reader != NULL)
        {
            cbox->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
            cbox->widget.drawState = WAIT_STRING;
            
            return;
        }
    }
    
    nextState(cbox);
}

static void waitString(laCheckBoxWidget* cbox)
{
    if(cbox->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        cbox->reader->run(cbox->reader);
        
        return;
    }
    
    // free the reader
    cbox->reader->memIntf->heap.free(cbox->reader);
    cbox->reader = NULL;
    
    cbox->widget.drawState = DRAW_STRING;
    
    nextState(cbox);
}

static void drawBorder(laCheckBoxWidget* cbox)
{
    if(cbox->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)cbox);
    else if(cbox->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)cbox);
    
    nextState(cbox);
}

void _laCheckBoxWidget_Paint(laCheckBoxWidget* cbox)
{
    laContext* context = laContext_GetActive();
    
    if(cbox->widget.scheme == NULL)
    {
        cbox->widget.drawState = DONE;
        
        return;
    }
    
    if(cbox->widget.drawState == NOT_STARTED)
        nextState(cbox);
    
    while(cbox->widget.drawState != DONE)
    {
        cbox->widget.drawFunc((laWidget*)cbox);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           cbox->widget.drawState == WAIT_IMAGE ||
           cbox->widget.drawState == WAIT_STRING)
            break;
    }
}

#endif // LA_CHECKBOX_WIDGET_ENABLED