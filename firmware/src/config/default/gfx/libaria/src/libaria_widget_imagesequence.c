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


#include "gfx/libaria/inc/libaria_widget_imagesequence.h"

#if LA_IMAGESEQUENCE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

#define DEFAULT_DELAY           1000

void _laImageSequenceWidget_Constructor(laImageSequenceWidget* img)
{
    _laWidget_Constructor((laWidget*)img);
    
    img->widget.destructor = (laWidget_Destructor_FnPtr)&_laImageSequenceWidget_Destructor;

    img->widget.type = LA_WIDGET_IMAGESEQUENCE;

    // override base class methods
    img->widget.update = (laWidget_Update_FnPtr)&_laImageSequenceWidget_Update;
    img->widget.paint = (laWidget_Paint_FnPtr)&_laImageSequenceWidget_Paint;

    img->widget.rect.width = DEFAULT_WIDTH;
    img->widget.rect.height = DEFAULT_HEIGHT;

    img->widget.borderType = LA_WIDGET_BORDER_NONE;
    img->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;

    img->count = 0;
    img->images = NULL;
    img->activeIdx = 0;
    
    img->playing = LA_FALSE;
    img->time = 0;
    img->repeat = LA_FALSE;
    
    img->cb = NULL;
}

void _laImageSequenceWidget_Destructor(laImageSequenceWidget* img)
{
    if(img->images != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(img->images);
        img->images = NULL;
    }
    
    _laWidget_Destructor((laWidget*)img);
}

laWidgetUpdateState _laImageSequenceWidget_Update(laImageSequenceWidget* img, uint32_t dt)
{
    // no need to update
    if(img->playing == LA_FALSE || img->count <= 1)
        return LA_WIDGET_UPDATE_STATE_DONE;
        
    // initial position
    if(img->activeIdx == LA_IMAGESEQ_RESTART)
    {
        img->activeIdx = 0;
        img->time = 0;
        
        laWidget_Invalidate((laWidget*)img);
        
        //printf("invalidating\n");
    }
    // normal update
    else
    {
        img->time += dt;
        
        // advance frame?
        if(img->time >= img->images[img->activeIdx].delay)
        {
            img->activeIdx++;
            img->time = 0;
            
            // end of sequence?
            if(img->activeIdx >= (int32_t)img->count)
            {
                // repeat?
                if(img->repeat == LA_TRUE)
                {
                    img->activeIdx = 0;
                    
                    if(img->cb != NULL)
                        img->cb(img);
                    
                    laWidget_Invalidate((laWidget*)img);
                    
                    //printf("invalidating\n");
                }
                else
                {
                    // just stay on last image, no redraw necessary
                    img->playing = LA_FALSE;
                }
            }
            else
            {
                if(img->cb != NULL)
                    img->cb(img);
        
                laWidget_Invalidate((laWidget*)img);
                
                //printf("invalidating\n");
            }
        }
    }
    
    return LA_WIDGET_UPDATE_STATE_DONE;
}

laImageSequenceWidget* laImageSequenceWidget_New()
{
    laImageSequenceWidget* img = laContext_GetActive()->memIntf.heap.calloc(1,
                                                                            sizeof(laImageSequenceWidget));
    
    if(img == NULL)
        return NULL;

    _laImageSequenceWidget_Constructor(img);

    return img;
}

uint32_t laImageSequenceWidget_GetImageCount(laImageSequenceWidget* img)
{
    if(img == NULL)
        return 0;
        
    return img->count;
}

laResult laImageSequenceWidget_SetImageCount(laImageSequenceWidget* img,
                                             uint32_t count)
{
    uint32_t i;
    
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->count == count)
        return LA_SUCCESS;
        
    img->images = laContext_GetActive()->memIntf.heap.realloc(img->images,
                                                              sizeof(laImageSequenceEntry) * count);
                      
    if(count > img->count)
    {
        for(i = img->count; i < count; i++)
        {
            img->images[i].image = NULL;
            img->images[i].halign = LA_HALIGN_CENTER;
            img->images[i].valign = LA_VALIGN_MIDDLE;
            img->images[i].delay = DEFAULT_DELAY;
        }
    }
                                                
    img->count = count;
    img->activeIdx = 0; // just restart
        
    return LA_SUCCESS;
}

GFXU_ImageAsset* laImageSequenceWidget_GetImage(laImageSequenceWidget* img,
                                                uint32_t idx)
{
    if(img == NULL || idx >= img->count)
        return NULL;
        
    return img->images[idx].image;
}

laResult laImageSequenceWidget_SetImage(laImageSequenceWidget* img,
                                        uint32_t idx,
                                        GFXU_ImageAsset* imgAst)
{
    if(img == NULL || idx >= img->count)
        return LA_FAILURE;
        
    if(img->images[idx].image == imgAst)
        return LA_SUCCESS;
        
    img->images[idx].image = imgAst;
        
    if(img->activeIdx == idx)
        laWidget_Invalidate((laWidget*)img);
        
    return LA_SUCCESS;
}
                                              
uint32_t laImageSequenceWidget_GetImageDelay(laImageSequenceWidget* img,
                                             uint32_t idx)
{
    if(img == NULL || idx >= img->count)
        return 0;
        
    return img->images[idx].delay;
}

laResult laImageSequenceWidget_SetImageDelay(laImageSequenceWidget* img,
                                             uint32_t idx,
                                             uint32_t delay)
{
    if(img == NULL || idx >= img->count)
        return LA_FAILURE;
        
    img->images[idx].delay = delay;
        
    return LA_SUCCESS;
}

laHAlignment laImageSequenceWidget_GetImageHAlignment(laImageSequenceWidget* img,
                                                      uint32_t idx)
{
    if(img == NULL || idx >= img->count)
        return 0;
        
    return img->images[idx].halign;
}

laResult laImageSequenceWidget_SetImageHAlignment(laImageSequenceWidget* img,
                                                  uint32_t idx,
                                                  laHAlignment align)
{
    if(img == NULL || idx >= img->count)
        return LA_FAILURE;
        
    if(img->images[idx].halign == align)
        return LA_SUCCESS;
        
    img->images[idx].halign = align;
        
    if(img->activeIdx == idx)
        laWidget_Invalidate((laWidget*)img);
        
    return LA_SUCCESS;
}

laVAlignment laImageSequenceWidget_GetImageVAlignment(laImageSequenceWidget* img,
                                                      uint32_t idx)
{
    if(img == NULL || idx >= img->count)
        return 0;
        
    return img->images[idx].halign;
}

laResult laImageSequenceWidget_SetImageVAlignment(laImageSequenceWidget* img,
                                                  uint32_t idx,
                                                  laVAlignment align)
{
    if(img == NULL || idx >= img->count)
        return LA_FAILURE;
        
    if(img->images[idx].valign == align)
        return LA_SUCCESS;
        
    img->images[idx].valign = align;
        
    if(img->activeIdx == idx)
        laWidget_Invalidate((laWidget*)img);
        
    return LA_SUCCESS;
}

laResult laImageSequenceWidget_Stop(laImageSequenceWidget* img)
{
    if(img == NULL)
        return LA_FAILURE;
        
    img->playing = LA_FALSE;
    
    return LA_SUCCESS;
}

laResult laImageSequenceWidget_Play(laImageSequenceWidget* img)
{
    if(img == NULL)
        return LA_FAILURE;
        
    img->playing = LA_TRUE;
    
    return LA_SUCCESS;
}

laResult laImageSequenceWidget_Rewind(laImageSequenceWidget* img)
{
    if(img == NULL)
        return LA_FAILURE;
        
    img->activeIdx = LA_IMAGESEQ_RESTART;
    img->time = 0;
    
    return LA_SUCCESS;
}

laBool laImageSequenceWidget_IsPlaying(laImageSequenceWidget* img)
{
    if(img == NULL)
        return LA_FALSE;
        
    return img->playing;
}

laBool laImageSequenceWidget_GetRepeat(laImageSequenceWidget* img)
{
    if(img == NULL)
        return LA_FALSE;
        
    return img->repeat;
}

laResult laImageSequenceWidget_SetRepeat(laImageSequenceWidget* img,
                                         laBool repeat)
{
    if(img == NULL)
        return LA_FAILURE;
        
    img->repeat = repeat;
    
    return LA_SUCCESS;
}

laResult laImageSequenceWidget_ShowImage(laImageSequenceWidget* img,
                                         uint32_t idx)
{
    if(img == NULL || idx >= img->count)
        return LA_FAILURE;
        
    img->activeIdx = idx;
    img->time = 0;
    
    if(img->cb != NULL)
        img->cb(img);

    laWidget_Invalidate((laWidget*)img);
    
    return LA_SUCCESS;
}

laResult laImageSequenceWidget_ShowNextImage(laImageSequenceWidget* img)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->activeIdx == img->count - 1)
    {
        if(img->repeat == LA_TRUE)
            laImageSequenceWidget_ShowImage(img, 0);
    }
    else
    {
        laImageSequenceWidget_ShowImage(img, img->activeIdx + 1);
    }
    
    return LA_SUCCESS;
}

laResult laImageSequenceWidget_ShowPreviousImage(laImageSequenceWidget* img)
{
    if(img == NULL)
        return LA_FAILURE;
        
    if(img->activeIdx == 0)
    {
        if(img->repeat == LA_TRUE)
            laImageSequenceWidget_ShowImage(img, img->count - 1);
    }
    else
    {
        laImageSequenceWidget_ShowImage(img, img->activeIdx - 1);
    }
    
    return LA_SUCCESS;
}

laImageSequenceImageChangedEvent_FnPtr laImageSequenceWidget_GetImageChangedEventCallback(laImageSequenceWidget* img)
{
    if(img == NULL)
        return NULL;
        
    return img->cb;
}

laResult laImageSequenceWidget_SetImageChangedEventCallback(laImageSequenceWidget* img,
                                                            laImageSequenceImageChangedEvent_FnPtr cb)
{
    if(img == NULL)
        return LA_FAILURE;
        
    img->cb = cb;
    
    return LA_SUCCESS;
}

#endif // LA_IMAGESEQUENCE_WIDGET_ENABLED
