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
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

static void invalidateImageRect(laImageWidget* img)
{
    GFX_Rect imgRect, imgSrcRect;
    
    _laImageWidget_GetImageRect(img, &imgRect, &imgSrcRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)img),
		                  &imgRect,
						  LA_FALSE);
}

void _laImageWidget_Constructor(laImageWidget* img)
{
    _laWidget_Constructor((laWidget*)img);
    
    img->widget.destructor = (laWidget_Destructor_FnPtr)&_laImageWidget_Destructor;

    img->widget.type = LA_WIDGET_IMAGE;

    // override base class methods
    img->widget.paint = (laWidget_Paint_FnPtr)&_laImageWidget_Paint;

    img->widget.rect.width = DEFAULT_WIDTH;
    img->widget.rect.height = DEFAULT_HEIGHT;

    img->widget.borderType = LA_WIDGET_BORDER_NONE;
    img->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;

    img->halign = LA_HALIGN_CENTER;
    img->valign = LA_VALIGN_MIDDLE;
    
    img->image = NULL;
}

void _laImageWidget_Destructor(laImageWidget* img)
{
    // free any existing memory reader
    if(img->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(img->reader);
        
        img->reader = NULL;
    }
    
    _laWidget_Destructor((laWidget*)img);
}

laImageWidget* laImageWidget_New()
{
    laImageWidget* img = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    img = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laImageWidget));

    if(img == NULL)
        return NULL;
    
    _laImageWidget_Constructor(img);

    return img;
}

laHAlignment laImageWidget_GetHAlignment(laImageWidget* img)
{
    if(img == NULL)
        return 0;

    return img->halign;
}

laResult laImageWidget_SetHAlignment(laImageWidget* img,
                                     laHAlignment align)
{
    if(img == NULL)
        return LA_FAILURE;

    if(img->halign == align)
        return LA_SUCCESS;

    invalidateImageRect(img);

    img->halign = align;

    invalidateImageRect(img);
    
    return LA_SUCCESS;
}

laVAlignment laImageWidget_GetVAlignment(laImageWidget* img)
{
    if(img == NULL)
        return 0;

    return img->valign;
}

laResult laImageWidget_SetVAlignment(laImageWidget* img,
                                     laVAlignment align)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->valign == align)
        return LA_SUCCESS;
     
    invalidateImageRect(img);
        
    img->valign = align;

    invalidateImageRect(img);
    
    return LA_SUCCESS;
}

GFXU_ImageAsset* laImageWidget_GetImage(laImageWidget* img)
{
    if(img == NULL)
        return NULL;

    return img->image;
}

laResult laImageWidget_SetImage(laImageWidget* img,
                                GFXU_ImageAsset* imgAst)
{
    if(img == NULL)
        return LA_FAILURE;

    if(img->image == imgAst)
        return LA_SUCCESS;
        
    if(img->image != NULL)
        invalidateImageRect(img);

    img->image = imgAst;

    if(img->image != NULL)
        invalidateImageRect(img);
    
    return LA_SUCCESS;
}

void laImageWidget_SetCallBackStart(laImageWidget* image, laImageWidget_DrawEventCallback cb)
{
    image->ImageDrawStart = cb;
    
}

void laImageWidget_SetCallBackEnd(laImageWidget* image, laImageWidget_DrawEventCallback cb)
{
    image->ImageDrawEnd = cb;
}

#endif // LA_IMAGE_WIDGET_ENABLED