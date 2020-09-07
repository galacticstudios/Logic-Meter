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
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

static void touchDown(laImagePlusWidget* img, laInput_TouchDownEvent* evt);
static void touchUp(laImagePlusWidget* img, laInput_TouchUpEvent* evt);
static void touchMoved(laImagePlusWidget* img, laInput_TouchMovedEvent* evt);

static void invalidateImageRect(laImagePlusWidget* img)
{
    GFX_Rect imgRect, imgSrcRect;
    
    _laImagePlusWidget_GetImageRect(img, &imgRect, &imgSrcRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)img),
		                  &imgRect,
						  LA_FALSE);
}

void _laImagePlusWidget_Constructor(laImagePlusWidget* img)
{
    _laWidget_Constructor((laWidget*)img);
    
    img->widget.destructor = (laWidget_Destructor_FnPtr)&_laImagePlusWidget_Destructor;

    img->widget.type = LA_WIDGET_IMAGE;

    // override base class methods
    img->widget.paint = (laWidget_Paint_FnPtr)&_laImagePlusWidget_Paint;

    img->widget.rect.width = DEFAULT_WIDTH;
    img->widget.rect.height = DEFAULT_HEIGHT;

    img->widget.borderType = LA_WIDGET_BORDER_NONE;
    img->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;
    
    img->transformX = 0;
    img->transformY = 0;
    img->transformWidth = 00;
    img->transformHeight = 0;
    
    img->resizeToFit = LA_FALSE;
    img->preserveAspect = LA_FALSE;
    img->resizeFilter = LA_IMAGEFILTER_NEARESTNEIGHBOR;
    img->inputEnabled = LA_FALSE;
    
    img->image = NULL;
    
    img->touch0.x = 0;
    img->touch0.y = 0;
    img->touch0_down = LA_FALSE;
    
    img->touch1.x = 0;
    img->touch1.y = 0;
    img->touch1_down = LA_FALSE;
    
    img->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&touchDown;
    img->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&touchUp;
    img->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&touchMoved;
}

void _laImagePlusWidget_Destructor(laImagePlusWidget* img)
{
    _laWidget_Destructor((laWidget*)img);
}

laImagePlusWidget* laImagePlusWidget_New()
{
    laImagePlusWidget* img = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    img = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laImagePlusWidget));

    if(img == NULL)
        return NULL;
    
    _laImagePlusWidget_Constructor(img);

    return img;
}

GFXU_ImageAsset* laImagePlusWidget_GetImage(laImagePlusWidget* img)
{
    if(img == NULL)
        return NULL;

    return img->image;
}

laResult laImagePlusWidget_SetImage(laImagePlusWidget* img,
                                    GFXU_ImageAsset* imgAst)
{
    if(img == NULL)
        return LA_FAILURE;

    if(img->image == imgAst)
        return LA_SUCCESS;
        
    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->image = NULL;
        
    // image plus only supports raw images in local memory for now
    if(imgAst->header.dataLocation != 0 ||
       imgAst->format != GFXU_IMAGE_FORMAT_RAW)
       return LA_FAILURE;

    img->image = imgAst;

    if(img->image != NULL)
        invalidateImageRect(img);
    
    GFX_PixelBufferCreate(img->image->bufferWidth,
                          img->image->bufferHeight,
                          img->image->colorMode,
                          img->image->header.dataAddress,
                          &img->buffer);
    
    return LA_SUCCESS;
}

int32_t laImagePlusWidget_GetTransformX(laImagePlusWidget* img)
{
    if(img == NULL)
        return 0;

    return img->transformX;
}

laResult laImagePlusWidget_SetTransformX(laImagePlusWidget* img, int32_t x)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->transformX == x)
        return LA_SUCCESS;

    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->transformX = x;

    if(img->image != NULL)
        invalidateImageRect(img);
        
    return LA_SUCCESS;
}

int32_t laImagePlusWidget_GetTransformY(laImagePlusWidget* img)
{
    if(img == NULL)
        return 0;

    return img->transformY;
}

laResult laImagePlusWidget_SetTransformY(laImagePlusWidget* img, int32_t y)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->transformY == y)
        return LA_SUCCESS;

    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->transformY = y;
    
    if(img->image != NULL)
        invalidateImageRect(img);
        
    return LA_SUCCESS;
}

int32_t laImagePlusWidget_GetTransformWidth(laImagePlusWidget* img)
{
    if(img == NULL)
        return 0;

    return img->transformWidth;
}

laResult laImagePlusWidget_SetTransformWidth(laImagePlusWidget* img, int32_t width)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->transformWidth == width)
        return LA_SUCCESS;

    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->transformWidth = width;
    
    if(img->image != NULL)
        invalidateImageRect(img);
        
    return LA_SUCCESS;
}

int32_t laImagePlusWidget_GetTransformHeight(laImagePlusWidget* img)
{
    if(img == NULL)
        return 0;

    return img->transformHeight;
}

laResult laImagePlusWidget_SetTransformHeight(laImagePlusWidget* img, int32_t height)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->transformHeight == height)
        return LA_SUCCESS;

    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->transformHeight = height;
    
    if(img->image != NULL)
        invalidateImageRect(img);
        
    return LA_SUCCESS;
}

laResult laImagePlusWidget_ResetTransform(laImagePlusWidget* img)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->transformX = 0;
    img->transformY = 0;
    img->transformWidth = 0;
    img->transformHeight = 0;
    
    if(img->image != NULL)
        invalidateImageRect(img);
        
    return LA_SUCCESS;
}

laBool laImagePlusWidget_GetStretchEnabled(laImagePlusWidget* img)
{
    if(img == NULL)
        return LA_FALSE;
        
    return img->resizeToFit;
}

laResult laImagePlusWidget_SetStretchEnabled(laImagePlusWidget* img,
                                             laBool stretch)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->resizeToFit == stretch)
        return LA_SUCCESS;
        
    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->resizeToFit = stretch;
    
    if(img->image != NULL)
        invalidateImageRect(img);
        
    return LA_SUCCESS;
}

laBool laImagePlusWidget_GetPreserveAspectEnabled(laImagePlusWidget* img)
{
    if(img == NULL)
        return LA_FALSE;
        
    return img->preserveAspect;
}

laResult laImagePlusWidget_SetPreserveAspectEnabled(laImagePlusWidget* img,
                                                    laBool preserve)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->preserveAspect == preserve)
        return LA_SUCCESS;
        
    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->preserveAspect = preserve;
    
    if(img->image != NULL)
        invalidateImageRect(img);
        
    return LA_SUCCESS;
}

laImagePlusWidget_ResizeFilter laImagePlusWidget_GetResizeFilter(laImagePlusWidget* img)
{
    if(img == NULL)
        return LA_FALSE;
        
    return img->resizeFilter;
}

laResult laImagePlusWidget_SetResizeFilter(laImagePlusWidget* img,
                                           laImagePlusWidget_ResizeFilter filter)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->resizeFilter == filter)
        return LA_SUCCESS;
        
    if(img->image != NULL)
        invalidateImageRect(img);
        
    img->resizeFilter = filter;
    
    if(img->image != NULL)
        invalidateImageRect(img);
        
    return LA_SUCCESS;
}

laBool laImagePlusWidget_GetInteractive(laImagePlusWidget* img)
{
    if(img == NULL)
        return LA_FALSE;
        
    return img->inputEnabled;
}

laResult laImagePlusWidget_SetInteractive(laImagePlusWidget* img,
                                          laBool interactive)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->inputEnabled == interactive)
        return LA_SUCCESS;
        
    img->inputEnabled = interactive;
        
    return LA_SUCCESS;
}

static void touchDown(laImagePlusWidget* img, laInput_TouchDownEvent* evt)
{    
    if(img->inputEnabled == LA_FALSE)
        return;
    
    evt->event.accepted = LA_TRUE;
    
    if(evt->touchID == 0)
    {
        img->touch0.x = evt->x;
        img->touch0.y = evt->y;
        img->touch0_down = LA_TRUE;
    }
    else if(evt->touchID == 1)
    {
        img->touch1.x = evt->x;
        img->touch1.y = evt->y;
        img->touch1_down = LA_TRUE;
    }

    //printf("img touch down, evt->touchID\n");
}

static void touchUp(laImagePlusWidget* img, laInput_TouchUpEvent* evt)
{
    if(img->inputEnabled == LA_FALSE)
        return;
        
    evt->event.accepted = LA_TRUE;
    
    if(evt->touchID == 0)
    {
        img->touch0_down = LA_FALSE;
        img->touch1_down = LA_FALSE;
    }
    else if(evt->touchID == 1)
    {
        img->touch1_down = LA_FALSE;
    }

    //printf("btn touch up %i\n", evt->touchID);
}

static void touchMoved(laImagePlusWidget* img, laInput_TouchMovedEvent* evt)
{    
    if(img->inputEnabled == LA_FALSE)
        return;

    evt->event.accepted = LA_TRUE;
    
    if(img->image != NULL)
        invalidateImageRect(img);
    
    switch(evt->touchID)
    {
        case 0:
        {
            // image translate
            if(img->touch1_down == LA_FALSE)
            {
                img->transformX += evt->x - img->touch0.x;
                img->transformY += evt->y - img->touch0.y;
            }
            else
            {
                img->transformWidth -= evt->x - img->touch0.x;
                img->transformHeight -= evt->y - img->touch0.y;
            }
            
            img->touch0.x = evt->x;
            img->touch0.y = evt->y;
            
            break;
        }
        case 1:
        {
            img->transformWidth += evt->x - img->touch1.x;
            img->transformHeight += evt->y - img->touch1.y;
                
            img->touch1.x = evt->x;
            img->touch1.y = evt->y;
            
            break;
        }
    }
    
    if(img->image != NULL)
        invalidateImageRect(img);
}

#endif // LA_IMAGEPLUS_WIDGET_ENABLED