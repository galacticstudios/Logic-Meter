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


#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_context.h"

GFX_PixelBuffer* GFX_LayerReadBuffer(GFX_Layer* layer)
{
    return &layer->buffers[layer->buffer_read_idx].pb;
}

GFX_PixelBuffer* GFX_LayerWriteBuffer(GFX_Layer* layer)
{
    return &layer->buffers[layer->buffer_write_idx].pb;
}

void GFX_LayerRotate(GFX_Layer* layer)
{
    int32_t t;
    
    t = layer->rect.local.width;
    layer->rect.local.width = layer->rect.local.height;
    layer->rect.local.height = t;
    
    t = layer->rect.display.height;
    layer->rect.display.height = layer->rect.display.width;
    layer->rect.display.width = t;
}

//int instance = 0;

void GFX_LayerSwap(GFX_Layer* layer)
{
    layer->swapCount++;
    
    if(layer->swap == GFX_FALSE)
        return;
        
    layer->swap = GFX_FALSE;
    layer->invalid = GFX_FALSE;
    
    //printf("GFX_LayerSwap: layer %u swap = FALSE\n", layer->id);
    
    if(layer->buffer_count <= 1)
        return;
    
    // might be first swap, in this case the first buffer is
    // acting as both read and write
    if(layer->buffer_read_idx != layer->buffer_write_idx)
        layer->buffer_read_idx = layer->buffer_write_idx;

    layer->buffer_write_idx++;
    
    if(layer->buffer_write_idx >= layer->buffer_count)
        layer->buffer_write_idx = 0;
        
    GFX_ActiveContext()->hal.layerSwapped(layer);
    
    //printf("%i - GFX_LayerSwap: layer %u swap, read=%u, write=%u\n", instance++, layer->id, layer->buffer_read_idx, layer->buffer_write_idx);
}

void _GFX_LayerSizeSet(GFX_Layer* layer, int32_t width, int32_t height)
{
    GFX_Context* context = GFX_ActiveContext();

    layer->rect.local.width = width;
    layer->rect.local.height = height;

    if(context->orientation == GFX_ORIENTATION_90 ||
       context->orientation == GFX_ORIENTATION_270)
    {
        layer->rect.display.width = height;
        layer->rect.display.height = width;
    }
    else
    {
        layer->rect.display.width = width;
        layer->rect.display.height = height;
    }
}

void GFX_LayerFromOrientedSpace(GFX_Rect* displayRect,
                                GFX_Layer* layer,
                                GFX_Orientation ori,
                                GFX_Bool mirrored)
{
    int32_t y;
    
    switch(ori)
    {
        case GFX_ORIENTATION_0:
        {
            if(mirrored == GFX_TRUE)
                layer->rect.display.x = displayRect->width - layer->rect.display.width - layer->rect.display.x;
            
            break;
        }
        case GFX_ORIENTATION_90:
        {
            y = displayRect->width - layer->rect.display.x - layer->rect.display.width;
            
            if(mirrored == GFX_TRUE)
                layer->rect.display.x = displayRect->height - layer->rect.display.height - layer->rect.display.y;
            else
                layer->rect.display.x = layer->rect.display.y;
            
            layer->rect.display.y = y;
            
            layer->rect.local.width = layer->rect.display.width;
            layer->rect.local.height = layer->rect.display.height;
            
            break;
        }
        case GFX_ORIENTATION_180:
        {         
            layer->rect.display.y = displayRect->height - (layer->rect.display.height + layer->rect.display.y);

            if(mirrored == GFX_FALSE)
                layer->rect.display.x = displayRect->width - layer->rect.display.width - layer->rect.display.x;

            break;
        }
        case GFX_ORIENTATION_270:
        {
            y = layer->rect.display.x;
            
            if(mirrored == GFX_TRUE)
                layer->rect.display.x = layer->rect.display.y;
            else
                layer->rect.display.x = displayRect->height - (layer->rect.display.height + layer->rect.display.y);

            layer->rect.display.y = y;
            
            layer->rect.local.width = layer->rect.display.width;
            layer->rect.local.height = layer->rect.display.height;
            
            break;
        }
    }
}

void GFX_LayerToOrientedSpace(GFX_Rect* displayRect,
                              GFX_Layer* layer,
                              GFX_Orientation ori,
                              GFX_Bool mirrored)
{
    GFX_Rect rect = layer->rect.display;
    
    switch(ori)
    {
        case GFX_ORIENTATION_0:
        {
            if(mirrored == GFX_TRUE)
                layer->rect.display.x = displayRect->width - 1 - layer->rect.display.x;
            
            break;
        }
        case GFX_ORIENTATION_90:
        {
            layer->rect.display.x = displayRect->width - rect.width - rect.y;
            
            if(mirrored == GFX_TRUE)
                layer->rect.display.y = displayRect->height - rect.height - rect.x;
            else
                layer->rect.display.y = rect.x;
            
            layer->rect.local.width = layer->rect.display.height;
            layer->rect.local.height = layer->rect.display.width;
            
            break;
        }
        case GFX_ORIENTATION_180:
        {
            if(mirrored == GFX_TRUE)
                layer->rect.display.x = rect.y;
            else
                layer->rect.display.x = displayRect->width - 1 - rect.x;

            layer->rect.display.y = displayRect->height - 1 - rect.y;
            
            break;
        }
        case GFX_ORIENTATION_270:
        {
            layer->rect.display.x = rect.y;

            if(mirrored == GFX_TRUE)
                layer->rect.display.y = rect.x;
            else
                layer->rect.display.y = displayRect->height - 1 - rect.x;
            
            layer->rect.local.width = layer->rect.display.height;
            layer->rect.local.height = layer->rect.display.width;
            
            break;
        }
    }
}

GFX_Point GFX_LayerPointFromOrientedSpace(const GFX_Layer* layer,
                                          const GFX_Point* point,
                                          GFX_Orientation ori,
                                          GFX_Bool mirrored)
{
    GFX_Point res = {0};
    
    switch(ori)
    {
        case GFX_ORIENTATION_0:
        {
            if(mirrored == GFX_TRUE)
                res.x = layer->rect.display.x + layer->rect.display.width - 1 - point->x;
            else
                res.x = layer->rect.display.x + point->x;
            
            res.y = layer->rect.display.y + point->y;
            
            break;
        }
        case GFX_ORIENTATION_90:
        {
            res.x = layer->rect.display.x + layer->rect.display.height - 1 - point->y;
            
            if(mirrored == GFX_TRUE)
                res.y = layer->rect.display.y + layer->rect.display.width - 1 - point->x;
            else
                res.y = layer->rect.display.y + point->x;
                        
            break;
        }
        case GFX_ORIENTATION_180:
        {         
            if(mirrored == GFX_TRUE)
                res.x = layer->rect.display.x + point->x;
            else
                res.x = layer->rect.display.x + layer->rect.display.width - 1 - point->x;
            
            res.y = layer->rect.display.y + layer->rect.display.height - point->y;
            
            break;
        }
        case GFX_ORIENTATION_270:
        {
            res.x = layer->rect.display.x + point->y;
            
            if(mirrored == GFX_TRUE)
                res.y = layer->rect.display.y + point->x;
            else
                res.y = layer->rect.display.y - 1 - point->x;
            
            break;
        }
    }
    
    return res;
}

GFX_Point GFX_LayerPointToOrientedSpace(const GFX_Layer* layer,
                                        const GFX_Point* point,
                                        GFX_Orientation ori,
                                        GFX_Bool mirrored)
{
    GFX_Point res = {0};

    switch(ori)
    {
        case GFX_ORIENTATION_0:
        {
            if(mirrored == GFX_TRUE)
                res.x = layer->rect.display.x - layer->rect.display.width - 1 - point->x;
            else
                res.x = layer->rect.display.x - point->x;
            
            res.y = layer->rect.display.y + point->y;
            
            break;
        }
        case GFX_ORIENTATION_90:
        {
            res.x = layer->rect.display.x - layer->rect.display.height - 1 - point->y;
            
            if(mirrored == GFX_TRUE)
                res.y = layer->rect.display.y - layer->rect.display.width - 1 - point->x;
            else
                res.y = layer->rect.display.y - point->x;
            
            break;
        }
        case GFX_ORIENTATION_180:
        {         
            if(mirrored == GFX_TRUE)
                res.x = layer->rect.display.x - point->x;
            else
                res.x = layer->rect.display.x - layer->rect.display.width - 1 - point->x;
            
            res.y = layer->rect.display.y - layer->rect.display.height - point->y;
            
            break;
        }
        case GFX_ORIENTATION_270:
        {
            res.x = layer->rect.display.x - point->y;
            
            if(mirrored == GFX_TRUE)
                res.y = layer->rect.display.y - point->x;
            else
                res.y = layer->rect.display.y - layer->rect.display.width - 1 - point->x;

            break;
        }
    }
    
    return res;
}

GFX_Rect GFX_LayerRectFromOrientedSpace(const GFX_Layer* layer,
                                        const GFX_Rect* rect,
                                        GFX_Orientation ori,
                                        GFX_Bool mirrored)
{
    GFX_Rect res = {0};
    
    switch(ori)
    {
        case GFX_ORIENTATION_0:
        {
            res = *rect;
            
            if(mirrored == GFX_TRUE)
                res.x = layer->rect.local.width - rect->x - rect->width;
            else
                res.x = rect->x;
            
            break;
        }
        case GFX_ORIENTATION_90:
        {
            res.width = rect->height;
            res.height = rect->width;
            
            res.x = layer->rect.local.height - rect->y - rect->height;
            
            if(mirrored == GFX_TRUE)
                res.y = layer->rect.local.width - rect->width - rect->x;
            else
                res.y = rect->x;

            break;
        }
        case GFX_ORIENTATION_180:
        {         
            res.width = rect->width;
            res.height = rect->height;
        
            if(mirrored == GFX_TRUE)
                res.x = rect->x;
            else
                res.x = layer->rect.local.width - rect->width - rect->x;
            
            res.y = layer->rect.local.height - rect->height - rect->y;
            
            break;
        }
        case GFX_ORIENTATION_270:
        {
            res.width = rect->height;
            res.height = rect->width;
            
            res.x = rect->y;
            
            if(mirrored == GFX_TRUE)
                res.y = rect->x;
            else
                res.y = layer->rect.local.width - rect->width - rect->x;

            break;
        }
    }
    
    return res;
}

GFX_Rect GFX_LayerRectToOrientedSpace(const GFX_Layer* layer,
                                      const GFX_Rect* rect,
                                      GFX_Orientation ori,
                                      GFX_Bool mirrored)
{
    GFX_Rect res = {0};

    switch(ori)
    {
        case GFX_ORIENTATION_0:
        {
            /*if(mirrored == GFX_TRUE)
                res.x = layer->rect.display.x - layer->rect.display.width - 1 - point->x;
            else
                res.x = layer->rect.display.x - point->x;
            
            res.y = layer->rect.display.y + point->y;*/
            
            break;
        }
        case GFX_ORIENTATION_90:
        {
            /*res.x = layer->rect.display.x - layer->rect.display.height - 1 - point->y;
            
            if(mirrored == GFX_TRUE)
                res.y = layer->rect.display.y - layer->rect.display.width - 1 - point->x;
            else
                res.y = layer->rect.display.y - point->x;*/
            
            break;
        }
        case GFX_ORIENTATION_180:
        {         
            /*if(mirrored == GFX_TRUE)
                res.x = layer->rect.display.x - point->x;
            else
                res.x = layer->rect.display.x - layer->rect.display.width - 1 - point->x;
            
            res.y = layer->rect.display.y - layer->rect.display.height - point->y;*/
            
            break;
        }
        case GFX_ORIENTATION_270:
        {
            /*res.x = layer->rect.display.x - point->y;
            
            if(mirrored == GFX_TRUE)
                res.y = layer->rect.display.y - point->x;
            else
                res.y = layer->rect.display.y - layer->rect.display.width - 1 - point->x;*/

            break;
        }
    }
    
    return res;
}

/*void GFX_LayerSizeFromOrientedSpace(GFX_Size* layerSize, GFX_Orientation ori)
{
    uint32_t t;

    if(ori == GFX_ORIENTATION_90 || ori == GFX_ORIENTATION_270)
    {
        t = layerSize->width;
        layerSize->width = layerSize->height;
        layerSize->height = t;
    }
}

void GFX_LayerSizeToOrientedSpace(GFX_Size* layerSize, GFX_Orientation ori)
{
    uint32_t t;

    if(ori == GFX_ORIENTATION_90 || ori == GFX_ORIENTATION_270)
    {
        t = layerSize->width;
        layerSize->width = layerSize->height;
        layerSize->height = t;
    }
}*/