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


#include "gfx/libaria/inc/libaria_widget_label.h"

#if LA_LABEL_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#define DEFAULT_NUM_LINES 5

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    ANALYZE_STRING,
    DRAW_STRING,
    WAIT_STRING,
    DRAW_BORDER,
};

void _laLabelWidget_GetTextRect(laLabelWidget* lbl,
                                GFX_Rect* textRect,
								GFX_Rect* drawRect)
{
    GFX_Rect bounds;
    
    bounds = laUtils_WidgetLocalRect((laWidget*)lbl);
    
    laString_GetMultiLineRect(&lbl->text, textRect, lbl->textLineSpace);
    
    // arrange relative to image rect
    laUtils_ArrangeRectangleRelative(textRect,
                                     GFX_Rect_Zero,
                                     bounds,
                                     lbl->halign,
                                     lbl->valign,
                                     0,
                                     lbl->widget.margin.left,
                                     lbl->widget.margin.top,
                                     lbl->widget.margin.right,
                                     lbl->widget.margin.bottom,
                                     0);

    GFX_RectClip(textRect, &bounds, drawRect);

	// move the rects to layer space
	laUtils_RectToLayerSpace((laWidget*)lbl, textRect);
    laUtils_RectToLayerSpace((laWidget*)lbl, drawRect);
}

static void drawBackground(laLabelWidget* lbl);
static void drawALineOfString(laLabelWidget* lbl);
static void analyzeMultiLineString(laLabelWidget* lbl);
static void waitString(laLabelWidget* lbl);
static void drawBorder(laLabelWidget* lbl);

static void nextState(laLabelWidget* lbl)
{
    switch(lbl->widget.drawState)
    {
        case NOT_STARTED:
        {
            //printf("layer painting\n");
            
            if(lbl->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                lbl->widget.drawState = DRAW_BACKGROUND;
                lbl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            if(laString_IsEmpty(&lbl->text) == LA_FALSE)
            {
                lbl->widget.drawState = ANALYZE_STRING;
                lbl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&analyzeMultiLineString;

                return;
            }
        }
        case ANALYZE_STRING:
        {
            if (lbl->drawObj.numlines > 0)
            {            
                lbl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawALineOfString;
                lbl->widget.drawState = DRAW_STRING;
                
                return;
            }
        }
        case DRAW_STRING:
        {   
            if (lbl->drawObj.line < lbl->drawObj.numlines)
            {
                lbl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawALineOfString;
                lbl->widget.drawState = DRAW_STRING;
                
                return;
            }
            else if(lbl->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                lbl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                lbl->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            lbl->widget.drawState = DONE;
            lbl->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laLabelWidget* lbl)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)lbl);
    
    nextState(lbl);
}

static void analyzeMultiLineString(laLabelWidget* lbl)
{
    GFX_Rect bounds = GFX_Rect_Zero;
    uint32_t maxLines = DEFAULT_NUM_LINES;
        
    lbl->drawObj.drawRect = GFX_Rect_Zero;
    lbl->drawObj.clipRect = GFX_Rect_Zero;
    lbl->drawObj.textRect = GFX_Rect_Zero;
    lbl->drawObj.newoffset = 0;

    lbl->drawObj.numlines = 0;
    lbl->drawObj.line = 0;
    lbl->drawObj.lineY = 0;

    lbl->drawObj.lineRect = laContext_GetActive()->memIntf.heap.calloc(maxLines * sizeof(GFX_Rect), 1);
    lbl->drawObj.offset = laContext_GetActive()->memIntf.heap.calloc(maxLines * sizeof(uint32_t), 1);
    
	bounds = laUtils_WidgetLocalRect((laWidget*)lbl);

	while (lbl->drawObj.lineRect != NULL && lbl->drawObj.offset != NULL)
	{
		lbl->drawObj.offset[lbl->drawObj.numlines] = lbl->drawObj.newoffset;

		laString_GetLineRect(&lbl->text,
                             lbl->drawObj.offset[lbl->drawObj.numlines],
                             &lbl->drawObj.lineRect[lbl->drawObj.numlines],
                             &lbl->drawObj.newoffset);

        if (lbl->drawObj.offset[lbl->drawObj.numlines] == lbl->drawObj.newoffset)
        {
            if (lbl->textLineSpace >= 0)
                lbl->drawObj.textRect.height += lbl->textLineSpace;
            else
                lbl->drawObj.textRect.height += laString_GetHeight(&lbl->text) - 
                               laString_GetAscent(&lbl->text);            
            
            break;
        }

        if (lbl->textLineSpace >= 0)
            lbl->drawObj.textRect.height += lbl->textLineSpace;
        else
            lbl->drawObj.textRect.height += laString_GetAscent(&lbl->text);        
        

		if (lbl->drawObj.lineRect[lbl->drawObj.numlines].width >
            lbl->drawObj.textRect.width)
		{
			lbl->drawObj.textRect.width = lbl->drawObj.lineRect[lbl->drawObj.numlines].width;
		}

		lbl->drawObj.numlines++;
        
        if (lbl->drawObj.numlines >= maxLines)
        {
            maxLines += DEFAULT_NUM_LINES;
            
            lbl->drawObj.lineRect = laContext_GetActive()->memIntf.heap.realloc(
                            lbl->drawObj.lineRect, maxLines * sizeof(GFX_Rect));
            lbl->drawObj.offset = laContext_GetActive()->memIntf.heap.realloc(
                            lbl->drawObj.offset, maxLines * sizeof(uint32_t));
            
            if (lbl->drawObj.lineRect == NULL || lbl->drawObj.offset == NULL)
            {
                //Unable to allocate for all lines
                lbl->drawObj.numlines = 0;
                break;
            }
        }
	}

	// arrange relative to image rect
    laUtils_ArrangeRectangleRelative(&lbl->drawObj.textRect,
                                     GFX_Rect_Zero,
                                     bounds,
                                     lbl->halign,
                                     lbl->valign,
                                     0,
                                     lbl->widget.margin.left,
                                     lbl->widget.margin.top,
                                     lbl->widget.margin.right,
                                     lbl->widget.margin.bottom,
                                     0);

    //Clip text to widget rectangle size
    GFX_RectClip(&lbl->drawObj.textRect, &bounds, &lbl->drawObj.drawRect);

	// move the rects to layer space
	laUtils_RectToLayerSpace((laWidget*)lbl, &lbl->drawObj.textRect);
    laUtils_RectToLayerSpace((laWidget*)lbl, &lbl->drawObj.drawRect);
    
    nextState(lbl);
}

static void drawALineOfString(laLabelWidget* lbl)
{
    uint32_t end = 0;
    laLayer* layer = laUtils_GetLayer((laWidget*)lbl);
    
    switch(lbl->halign)
    {
        case LA_HALIGN_LEFT:
            //No break. Fall through
        default:
            lbl->drawObj.lineRect[lbl->drawObj.line].x = lbl->drawObj.textRect.x;
            break;
        case LA_HALIGN_CENTER:
            lbl->drawObj.lineRect[lbl->drawObj.line].x = 
                                lbl->drawObj.textRect.x +
                                lbl->drawObj.textRect.width/2 -
                                lbl->drawObj.lineRect[lbl->drawObj.line].width/2;
            break;
        case LA_HALIGN_RIGHT:
            lbl->drawObj.lineRect[lbl->drawObj.line].x = 
                                lbl->drawObj.textRect.x +
                                lbl->drawObj.textRect.width -
                                lbl->drawObj.lineRect[lbl->drawObj.line].width;
            break;
    }

    lbl->drawObj.lineRect[lbl->drawObj.line].y = lbl->drawObj.textRect.y +
                                                 lbl->drawObj.lineY;

    //Clip to the line rectangle 
    GFX_RectClip(&lbl->drawObj.textRect,
                 &lbl->drawObj.lineRect[lbl->drawObj.line],
                 &lbl->drawObj.drawRect);

    if(GFX_RectIntersects(&layer->clippedDrawingRect,
                          &lbl->drawObj.drawRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        GFX_Set(GFXF_DRAW_COLOR, lbl->widget.scheme->text);

        GFX_RectClip(&lbl->drawObj.drawRect,
                     &layer->clippedDrawingRect,
                     &lbl->drawObj.clipRect);

        if (lbl->drawObj.line + 1 < lbl->drawObj.numlines)
        {
            end = lbl->drawObj.offset[lbl->drawObj.line + 1];
        }
        else
        {
            end = lbl->drawObj.newoffset;
        }

        laString_DrawSubStringClipped(&lbl->text,
                                     lbl->drawObj.offset[lbl->drawObj.line],
                                     end,
                                     lbl->drawObj.clipRect.x,
                                     lbl->drawObj.clipRect.y,
                                     lbl->drawObj.clipRect.width,
                                     lbl->drawObj.clipRect.height,
                                     lbl->drawObj.lineRect[lbl->drawObj.line].x,
                                     lbl->drawObj.lineRect[lbl->drawObj.line].y,
                                     &lbl->reader);
    }
    
    if (lbl->textLineSpace >= 0)
        lbl->drawObj.lineY += lbl->textLineSpace;
    else
        lbl->drawObj.lineY += laString_GetAscent(&lbl->text);            

    lbl->drawObj.line++;
    
    if (lbl->drawObj.line == lbl->drawObj.numlines)
    {
        laContext_GetActive()->memIntf.heap.free(lbl->drawObj.lineRect);
        laContext_GetActive()->memIntf.heap.free(lbl->drawObj.offset);
        
        lbl->drawObj.lineRect = NULL;
        lbl->drawObj.offset = NULL;
        
    }    

    if(lbl->reader != NULL)
    {
        lbl->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitString;
        lbl->widget.drawState = WAIT_STRING;
    }
    else
    {
        nextState(lbl);
    }
}

static void waitString(laLabelWidget* lbl)
{
    if(lbl->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        lbl->reader->run(lbl->reader);
        
        return;
    }
    
    // free the reader
    lbl->reader->memIntf->heap.free(lbl->reader);
    lbl->reader = NULL;
    
    lbl->widget.drawState = DRAW_STRING;
    
    nextState(lbl);
}

static void drawBorder(laLabelWidget* lbl)
{
    if(lbl->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)lbl);
    else if(lbl->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)lbl);
    
    nextState(lbl);
}

void _laLabelWidget_Paint(laLabelWidget* lbl)
{
    laContext* context = laContext_GetActive();
    
    if(lbl->widget.scheme == NULL)
    {
        lbl->widget.drawState = DONE;
        
        return;
    }

    if(lbl->widget.drawState == NOT_STARTED)
        nextState(lbl);
    
    while(lbl->widget.drawState != DONE)
    {
        lbl->widget.drawFunc((laWidget*)lbl);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           lbl->widget.drawState == WAIT_STRING)
            break;
    }
}

#endif // LA_IMAGE_WIDGET_ENABLED