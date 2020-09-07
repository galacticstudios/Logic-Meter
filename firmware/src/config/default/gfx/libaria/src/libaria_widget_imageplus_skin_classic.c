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


#include "gfx/libaria/inc/libaria_widget_imageplus.h"

#if LA_IMAGEPLUS_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#include "gfx/hal/inc/gfx_math.h"

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_IMAGE,
    DRAW_BORDER,
};

void _laImagePlusWidget_GetImageRect(laImagePlusWidget* img,
                                     GFX_Rect* imgRect,
                                     GFX_Rect* imgSrcRect)
{
    GFX_Rect clipRect;
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;
    
    int32_t aspect;
    
    GFX_Rect bounds = laUtils_WidgetLocalRect((laWidget*)img);
    
    imgRect->x = 0;
    imgRect->y = 0;
    
    if(img->image != NULL)
    {
        if(img->resizeToFit == LA_FALSE)
        {
            imgRect->width = img->image->width + img->transformWidth;
            imgRect->height = img->image->height + img->transformHeight;
            imgRect->x = bounds.width / 2 - imgRect->width / 2;
            imgRect->y = bounds.height / 2 - imgRect->height / 2;
        }
        else
        {
            imgRect->width = img->widget.rect.width;
            imgRect->height = img->widget.rect.height;
            
            imgRect->x += img->widget.margin.left;
            imgRect->width -= img->widget.margin.left;

            imgRect->y += img->widget.margin.top;
            imgRect->height -= img->widget.margin.top;

            imgRect->width -= img->widget.margin.right;
            imgRect->height -= img->widget.margin.bottom;
                
            if(img->preserveAspect == LA_TRUE)
            {
                if(img->image->width < img->image->height)
                {
                    aspect = GFX_PercentWholeRounded(img->image->height, img->image->width);
                    imgRect->height = GFX_PercentOf(imgRect->width, aspect);
                }
                else
                {                    
                    aspect = GFX_PercentWholeRounded(img->image->width, img->image->height);
                
                    imgRect->width = GFX_PercentOf(imgRect->height, aspect);
                    
                    if(imgRect->width > bounds.width - img->widget.margin.left - img->widget.margin.right)
                    {
                        imgRect->width = bounds.width - img->widget.margin.left - img->widget.margin.right;
                        
                        aspect = GFX_PercentWholeRounded(img->image->height, img->image->width);
                        
                        imgRect->height = GFX_PercentOf(imgRect->width, aspect);
                    }
                }
                
                imgRect->x = bounds.width / 2 - imgRect->width / 2;
                imgRect->y = bounds.height / 2 - imgRect->height / 2;
            }
        }
        
        imgSrcRect->x = 0;
        imgSrcRect->y = 0;
        imgSrcRect->width = img->image->width;
        imgSrcRect->height = img->image->height;
    }
    else
    {
        imgRect->width = 0;
        imgRect->height = 0;
        
        *imgSrcRect = *imgRect;
    }
    
    imgRect->x += img->transformX;
    imgRect->y += img->transformY;
    
    GFX_RectClip(imgRect, &bounds, &clipRect);
    
    w = img->image->width;
    h = img->image->height;
    
    // scaled rectangle has been clipped, adjust source rectangle
    if(clipRect.width != imgRect->width)
    {
        w = GFX_PercentWholeRounded(clipRect.width, imgRect->width);
        w = GFX_PercentOf(imgSrcRect->width, w);
        
        // calculate the delta in x
        x = clipRect.x - imgRect->x;
        
        if(x > 0)
        {
            x = GFX_PercentWholeRounded(x, imgRect->width);
            x = GFX_PercentOf(imgSrcRect->width, x);
        }
    }
    
    if(clipRect.height != imgRect->height)
    {
        h = GFX_PercentWholeRounded(clipRect.height, imgRect->height);
        h = GFX_PercentOf(imgSrcRect->height, h);
        
        // calculate the delta in x
        y = clipRect.y - imgRect->y;
        
        if(y > 0)
        {
            y = GFX_PercentWholeRounded(y, imgRect->height);
            y = GFX_PercentOf(imgSrcRect->height, y);
        }
    }
    
    imgSrcRect->x = x;
    imgSrcRect->y = y;
    imgSrcRect->width = w;
    imgSrcRect->height = h;
    
    *imgRect = clipRect;
    
    laUtils_RectToLayerSpace((laWidget*)img, imgRect); 
}

static void drawBackground(laImagePlusWidget* img);
static void drawImage(laImagePlusWidget* img);
static void drawBorder(laImagePlusWidget* img);

static void nextState(laImagePlusWidget* img)
{
    switch(img->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(img->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                img->widget.drawState = DRAW_BACKGROUND;
                img->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            if(img->image != NULL)
            {
                img->widget.drawState = DRAW_IMAGE;
                img->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawImage;

                return;
            }
        }
        case DRAW_IMAGE:
        {            
            if(img->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                img->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                img->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {           
            
            img->widget.drawState = DONE;
            img->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laImagePlusWidget* img)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)img);
    
    nextState(img);
}

static void drawImage(laImagePlusWidget* img)
{
    GFX_Rect imgRect, imgSrcRect;
	GFX_Bool alphaEnable;

    _laImagePlusWidget_GetImageRect(img, &imgRect, &imgSrcRect);

    GFX_Set(GFXF_DRAW_RESIZE_MODE, img->resizeFilter);
                        
	GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
	GFX_Set(GFXF_DRAW_ALPHA_ENABLE, img->widget.alphaEnabled);

	GFX_Set(GFXF_DRAW_MASK_ENABLE, (img->image->flags & GFXU_IMAGE_USE_MASK) > 0);
	GFX_Set(GFXF_DRAW_MASK_VALUE, img->image->mask);

	GFX_DrawStretchBlit(&img->buffer,
                        imgSrcRect.x,
                        imgSrcRect.y,
                        imgSrcRect.width,
                        imgSrcRect.height,
                        imgRect.x,
                        imgRect.y,
                        imgRect.width,
                        imgRect.height);
    
	GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
	GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);

	nextState(img);
}

static void drawBorder(laImagePlusWidget* img)
{
    if(img->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)img);
    else if(img->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)img);
    
    nextState(img);
}

void _laImagePlusWidget_Paint(laImagePlusWidget* img)
{
    
    laContext* context = laContext_GetActive();
    
    if(img->widget.scheme == NULL)
    {
        img->widget.drawState = DONE;
        
        return;
    }
    
    if(img->widget.drawState == NOT_STARTED)
        nextState(img);
    
    while(img->widget.drawState != DONE)
    {
        img->widget.drawFunc((laWidget*)img);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_IMAGEPLUS_WIDGET_ENABLED