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


#include "gfx/libaria/inc/libaria_widget_image.h"

#if LA_IMAGE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

uint8_t startCounting = 0;

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_IMAGE,
    WAIT_IMAGE,
    DRAW_BORDER,
};

void _laImageWidget_GetImageRect(laImageWidget* img,
                                 GFX_Rect* imgRect,
                                 GFX_Rect* imgSrcRect)
{
    GFX_Rect bounds = laUtils_WidgetLocalRect((laWidget*)img);

    imgRect->x = 0;
    imgRect->y = 0;

    if(img->image != NULL)
    {
        imgRect->width = img->image->width;
        imgRect->height = img->image->height;
    }
    else
    {
        imgRect->width = 0;
        imgRect->height = 0;
    }

    *imgSrcRect = *imgRect;

    // arrange image rect
    laUtils_ArrangeRectangle(imgRect,
                             GFX_Rect_Zero,
                             bounds,
                             img->halign,
                             img->valign,
                             0,
                             img->widget.margin.left,
                             img->widget.margin.top,
                             img->widget.margin.right,
                             img->widget.margin.bottom,
                             0);

    *imgRect = GFX_RectClipAdj(imgRect, &bounds, imgSrcRect);

    laUtils_RectToLayerSpace((laWidget*)img, imgRect);
}

static void drawBackground(laImageWidget* img);
static void drawImage(laImageWidget* img);
static void waitImage(laImageWidget* img);
static void drawBorder(laImageWidget* img);

static void nextState(laImageWidget* img)
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

static void drawBackground(laImageWidget* img)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)img);

    nextState(img);
}

static void drawImage(laImageWidget* img)
{
    GFX_Rect imgRect, imgSrcRect, clipRect;
	GFX_Bool alphaEnable;

    laLayer* layer = laUtils_GetLayer((laWidget*)img);


    if(img->ImageDrawStart != NULL && startCounting == 0)
    {
        img->ImageDrawStart(img);
        startCounting = 1;
    }

    _laImageWidget_GetImageRect(img, &imgRect, &imgSrcRect);

    if(GFX_RectIntersects(&layer->clippedDrawingRect, &imgRect) == GFX_TRUE)
    {
        clipRect = GFX_RectClipAdj(&imgRect, &layer->clippedDrawingRect, &imgSrcRect);

		GFX_Get(GFXF_DRAW_ALPHA_ENABLE, &alphaEnable);
		GFX_Set(GFXF_DRAW_ALPHA_ENABLE, img->widget.alphaEnabled);

		GFXU_DrawImage(img->image,
                       imgSrcRect.x,
                       imgSrcRect.y,
                       imgSrcRect.width,
                       imgSrcRect.height,
                       clipRect.x,
                       clipRect.y,
                       &laContext_GetActive()->memIntf,
                       &img->reader);

		GFX_Set(GFXF_DRAW_ALPHA_ENABLE, alphaEnable);
		
		if (img->reader != NULL)
        {
            img->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&waitImage;
            img->widget.drawState = WAIT_IMAGE;

            return;
        }


    }

    if( img->ImageDrawEnd != NULL && startCounting == 1)
    {
        img->ImageDrawEnd(img);
    }
    nextState(img);
}

static void waitImage(laImageWidget* img)
{
    if(img->reader->status != GFXU_READER_STATUS_FINISHED)
    {
        img->reader->run(img->reader);

        return;
    }

    // free the reader
    img->reader->memIntf->heap.free(img->reader);
    img->reader = NULL;

    img->widget.drawState = DRAW_IMAGE;


    if( img->ImageDrawEnd != NULL && startCounting == 1)
    {
        img->ImageDrawEnd(img);
    }
    nextState(img);
}

static void drawBorder(laImageWidget* img)
{
    if(img->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)img);
    else if(img->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)img);

    nextState(img);
}

void _laImageWidget_Paint(laImageWidget* img)
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

        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2 ||
           img->widget.drawState == WAIT_IMAGE)
            break;
    }


}

#endif // LA_IMAGE_WIDGET_ENABLED