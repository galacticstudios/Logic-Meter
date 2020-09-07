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


#include "gfx/hal/inc/gfx_default_impl.h"

#include "gfx/hal/inc/gfx_color.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_draw.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_pixel_buffer.h"
#include "gfx/hal/inc/gfx_util.h"

/*void *aligned_malloc(size_t size, size_t align_size)
{
    char *ptr,*ptr2,*aligned_ptr;
    int align_mask = align_size - 1;

    ptr=(char *)__pic32_alloc_coherent(size + align_size + sizeof(int));
    if(ptr==NULL) return(NULL);

    ptr2 = ptr + sizeof(int);
    aligned_ptr = ptr2 + (align_size - ((size_t)ptr2 & align_mask));

    ptr2 = aligned_ptr - sizeof(int);
    *((int *)ptr2)=(int)(aligned_ptr - ptr);

    return(aligned_ptr);
}

void aligned_free(void *ptr)
{
    int *ptr2=(int *)ptr - 1;
    ptr -= *ptr2;
    __pic32_free_coherent(ptr);
}*/

static GFX_Result layerBufferAllocate(GFX_Context* context,
                                      GFX_Layer* layer,
                                      uint32_t idx)
{
    void* pixelData;
    //void* mem;
    //void* ptr;
    
    if(layer->buffers[idx].state != GFX_BS_NONE)
        return GFX_FAILURE;
        
    // create pixel buffer construct
    GFX_PixelBufferCreate(layer->rect.display.width,
                          layer->rect.display.height,
                          context->colorMode,
                          NULL,
                          &layer->buffers[idx].pb);
    
    // allocate memory
    if(layer->buffers[idx].coherent == GFX_TRUE)
    {
        pixelData = context->memory.coherent_alloc(layer->buffers[idx].pb.buffer_length);
        //pixelData = frameBuffer;
        
        //pixelData = aligned_malloc(size, 16);
        
        //pixelData = malloc(size);
        
        //mem = context->memory.coherent_alloc(size+15);
        //pixelData = (void*)(((uintptr_t)mem+15) & ~ (uintptr_t)0x0F);
        //memset_16aligned(ptr, 0, 1024);
        //free(mem);
        
        //pixelData = frameBuffer;
        
    }
    else
        pixelData = context->memory.malloc(layer->buffers[idx].pb.buffer_length);
    
    if(pixelData == NULL)
        return GFX_FAILURE;
        
    // clear memory
    memset(pixelData, 0, layer->buffers[idx].pb.buffer_length);
    
    // set pixel data pointer and set allocated status
    layer->buffers[idx].pb.pixels = pixelData;
    layer->buffers[idx].state = GFX_BS_MALLOC;
    
    return GFX_SUCCESS;
}

static GFX_Result layerBufferFree(GFX_Context* context,
                                  GFX_Layer* layer,
                                  uint32_t idx)
{
    GFX_FrameBuffer* buf;
    
    buf = &layer->buffers[idx];
    
    // shouldn't free memory that wasn't allocated with buffer allocate
    if(buf->state != GFX_BS_MALLOC)
        return GFX_FAILURE;
    
    // free the memory
    if(buf->pb.pixels != GFX_NULL)
    {
        if(buf->coherent == GFX_TRUE)
            context->memory.coherent_free(buf->pb.pixels);
        else
            context->memory.free(buf->pb.pixels);
        
        buf->pb.pixels = GFX_NULL;
    }
    
    // change pixel buffer allocated status
    buf->state = GFX_BS_NONE;
    
    return GFX_SUCCESS;
}

GFX_Result defUnsupported(void)
{
    return GFX_UNSUPPORTED;
}

GFX_Result defInitialize(GFX_Context* context)
{
    uint32_t i;
    
    // initialize all layers
    for(i = 0; i < context->layer.count; i++)
    {
        // enables the layer
        context->layer.layers[i].enabled = GFX_TRUE;
    
        // sets all layer rectangles to the size of the display
        context->layer.layers[i].rect.local.x = 0;
        context->layer.layers[i].rect.local.y = 0;
        context->layer.layers[i].rect.local.width = context->display_info->rect.width;
        context->layer.layers[i].rect.local.height = context->display_info->rect.height;

        context->layer.layers[i].rect.display = context->layer.layers[i].rect.local;

        // disables alpha blending and sets opacity to full
        context->layer.layers[i].alphaEnable = GFX_FALSE;
        context->layer.layers[i].alphaAmount = 255;
        
        // disables masking and sets mask color to black
        context->layer.layers[i].maskEnable = GFX_FALSE;
        context->layer.layers[i].maskColor = 0;
    }
    
    return GFX_SUCCESS;
}

GFX_Result defBegin(void)
{
    GFX_Context* context = GFX_ActiveContext();
    
    // return false if active layer has a pending swap operation
    if(context->layer.active->locked == GFX_TRUE ||
       context->layer.active->swap == GFX_TRUE)
        return GFX_FAILURE;
        
    context->layer.active->locked = GFX_TRUE;
    
    return GFX_SUCCESS;
}

void defEnd(void)
{
    GFX_ActiveContext()->layer.active->locked = GFX_FALSE;
}

GFX_Result defUpdate(void)
{
    return GFX_SUCCESS;
}

void defDestroy(GFX_Context* context)
{ 
    uint32_t i, j;
    
    // attempt to free all layer buffers
    for(i = 0; i < context->layer.count; i++)
    {
        for(j = 0; j < context->layer.layers[i].buffer_count; j++)
        {
            if(context->layer.layers[i].buffers[j].coherent == GFX_TRUE)
                context->memory.coherent_free(context->layer.layers[i].buffers[j].pb.pixels);
            else
                context->memory.free(context->layer.layers[i].buffers[j].pb.pixels);
                
            context->layer.layers[i].buffers[j].pb.pixels = NULL;
        }
    }
}

GFX_Result defBrightnessRangeGet(uint32_t* low, uint32_t* high)
{
    *low = 0;
    *high = 1;
    
    return GFX_SUCCESS;
}
       
uint32_t defBrightnessGet(void)
{
    return GFX_ActiveContext()->brightness;
}

GFX_Result defBrightnessSet(uint32_t val)
{
    if(val > 1)
        return GFX_FAILURE;
        
    GFX_ActiveContext()->brightness = val;
    
    return GFX_SUCCESS;
}  

GFX_SyncCallback_FnPtr defVSyncCallbackGet(void)
{
    return GFX_ActiveContext()->vsyncCB;
}

GFX_Result defVSyncCallbackSet(GFX_SyncCallback_FnPtr cb)
{
    if(cb == NULL)
        cb = _GFX_DefaultSyncCallback;
    
    GFX_ActiveContext()->vsyncCB = cb;
    
    return GFX_SUCCESS;
}

GFX_SyncCallback_FnPtr defHSyncCallbackGet(void)
{
    return GFX_ActiveContext()->hsyncCB;
}

GFX_Result defHSyncCallbackSet(GFX_SyncCallback_FnPtr cb)
{
    if(cb == NULL)
        cb = _GFX_DefaultSyncCallback;
    
    GFX_ActiveContext()->hsyncCB = cb;
    
    return GFX_SUCCESS;
}

GFX_Orientation defOrientationGet(void)
{
    return GFX_ActiveContext()->orientation;
}

GFX_Result defOrientationSet(GFX_Orientation ori)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Orientation oldOri;
    GFX_Layer* layer;
    //GFX_Size size;
    //GFX_Point pnt;
    uint32_t i;
       
    if(context->orientation == ori)
        return GFX_FAILURE;
    
    oldOri = GFX_ActiveContext()->orientation;
    
    GFX_ActiveContext()->orientation = ori;

    /* convert layer positions from old orientation to new */
    for(i = 0; i < context->layer.count; i++)
    {
        layer = &context->layer.layers[i];

        //pnt.x = layer->rect.display.x;
        //pnt.y = layer->rect.display.y;

        //size.width = layer->rect.display.width;
        //size.height = layer->rect.display.height;

        //GFX_LayerFromOrientedSpace(layer, oldOri);

        GFX_LayerFromOrientedSpace(&context->display_info->rect,
                                   layer,
                                   oldOri,
                                   context->mirrored);

        /*GFX_LayerSizeToOrientedSpace(&size, ori);

        _GFX_LayerSizeSet(layer, size.width, size.height);*/

        GFX_LayerToOrientedSpace(&context->display_info->rect,
                                 layer,
                                 ori,
                                 context->mirrored);

        //layer->rect.display.x = pnt.x;
        //layer->rect.display.y = pnt.y;
    }
    
    // convert clipping rectangle from old orientation to new
    // todo
    
    return GFX_SUCCESS;
}
 
GFX_Bool defMirroringGet(void)
{
    return GFX_ActiveContext()->mirrored;
}

GFX_Result defMirroringSet(GFX_Bool mirr)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Layer* layer;
    //GFX_Point pnt;
    uint32_t i;

    if(context->mirrored == mirr)
        return GFX_SUCCESS;

    /* convert layer positions from old mirror position to new */
    for(i = 0; i < context->layer.count; i++)
    {
        layer = &context->layer.layers[i];

        GFX_LayerFromOrientedSpace(&context->display_info->rect,
                                   layer,
                                   context->orientation,
                                   context->mirrored);

        GFX_LayerToOrientedSpace(&context->display_info->rect,
                                 layer,
                                 context->orientation,
                                 mirr);
    }

    context->mirrored = mirr;
    
    if(mirr == GFX_TRUE)
        context->hal.mirrorPoint = defMirrorPoint;
    else
        context->hal.mirrorPoint = defMirrorPointEmpty;
        
    return GFX_SUCCESS;
}

GFX_ColorMode defColorModeGet(void)
{
    return GFX_ActiveContext()->colorMode;
}

GFX_Result defColorModeSet(GFX_ColorMode mode)
{
    uint32_t i, j;
    GFX_Layer* layer;
    GFX_Bool reallocate = GFX_FALSE;
    
    if(GFX_ActiveContext()->colorMode == mode)
        return GFX_SUCCESS;
        
    reallocate = GFX_ColorInfo[GFX_ActiveContext()->colorMode].bpp ==
                 GFX_ColorInfo[mode].bpp;
    
    for(i = 0; i < GFX_ActiveContext()->layer.count; i++)
    {
        layer = &GFX_ActiveContext()->layer.layers[i];
        
        for(j = 0; j < layer->buffer_count; j++)
        {
            // if color bpp is equal then buffers don't need to be reallocated
            if(reallocate == GFX_FALSE)
            {
                layer->buffers[j].pb.mode = mode; // just set new pixel mode
            }
            else if(layer->buffers[j].state == GFX_BS_MALLOC)
            {
                // reallocate buffer memory and pixel buffer construct
                layerBufferFree(GFX_ActiveContext(), layer, j);
                layerBufferAllocate(GFX_ActiveContext(), layer, j);
            }
        }
    }
        
    // set new global color mode
    GFX_ActiveContext()->colorMode = mode;
#if GFX_DRAW_PIPELINE_ENABLED
    GFX_ActiveContext()->draw.colorMode = mode;
#endif
    
#if GFX_ALPHA_BLENDING_ENABLED
    // turn off channel-based alpha blending if color mode does not have
    // an alpha channel
    if(GFX_COLOR_MODE_IS_ALPHA(mode) == GFX_FALSE)
        defDrawBlendModeSet(GFX_ActiveContext()->draw.blendMode & ~(GFX_BLEND_CHANNEL));
#endif
       
    return GFX_SUCCESS;
}

GFX_GlobalPalette defGlobalPaletteGet(void)
{
    return GFX_ActiveContext()->globalPalette;
}

GFX_Result defGlobalPaletteSet(GFX_GlobalPalette pal)
{
    GFX_ActiveContext()->globalPalette = pal;
    
    return GFX_SUCCESS;
}

uint32_t defLayerActiveGet(void)
{
    return GFX_ActiveContext()->layer.active_idx;
}

GFX_Result defLayerActiveSet(uint32_t idx)
{
    GFX_Context* context = GFX_ActiveContext();
    
    if(idx >= context->layer.count)
        return GFX_FAILURE;
        
    context->layer.active = &context->layer.layers[idx];
    
    return GFX_SUCCESS;
}

GFX_Bool defLayerEnabledGet(void)
{
    return GFX_ActiveContext()->layer.active->enabled;
}

GFX_Result defLayerEnabledSet(GFX_Bool val)
{
    GFX_ActiveContext()->layer.active->enabled = val;

    return GFX_SUCCESS;
}

GFX_Bool defLayerVisibleGet(void)
{
    return GFX_ActiveContext()->layer.active->visible;
}

GFX_Result defLayerVisibleSet(GFX_Bool value)
{
    GFX_ActiveContext()->layer.active->visible = value;
    
    return GFX_SUCCESS;
}

GFX_Result defLayerPositionGet(int32_t* x, int32_t* y)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point pnt;

    pnt.x = context->layer.active->rect.display.x;
    pnt.y = context->layer.active->rect.display.y;

    GFX_UtilPointFromOrientedSpace(&context->display_info->rect,
                                   &context->layer.active->rect.display,
                                   &pnt,
                                   context->orientation,
                                   context->mirrored);

    return GFX_SUCCESS;
}

GFX_Result defLayerPositionSet(int32_t x, int32_t y)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point pnt;
    
    pnt.x = x;
    pnt.y = y;

    GFX_UtilPointToOrientedSpace(&context->display_info->rect,
                                 &context->layer.active->rect.display,
                                 &pnt,
                                 context->orientation,
                                 context->mirrored);

    context->layer.active->rect.display.x = pnt.x;
    context->layer.active->rect.display.y = pnt.y;
    
    return GFX_SUCCESS;
}

GFX_Result defLayerSizeGet(int32_t* width, int32_t* height)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Size size;

    // transform from oriented space
    size.width = context->layer.active->rect.local.width;
    size.height = context->layer.active->rect.local.height;

    GFX_UtilSizeFromOrientedSpace(&size, context->orientation);

    *width  = size.width;
    *height = size.height;
    
    return GFX_SUCCESS;
}

GFX_Result defLayerSizeSet(int32_t width, int32_t height)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Size size;

    // transform to oriented space
    size.width = width;
    size.height = height;

    GFX_UtilSizeFromOrientedSpace(&size, context->orientation);

    _GFX_LayerSizeSet(context->layer.active, size.width, size.height);

    // TODO 
    // reallocate buffers if necessary

    //context->layer.active->rect.local.width = size.width;
    //context->layer.active->rect.local.height = size.height;

    //context->layer.active->rect.display.width = size.width;
    //context->layer.active->rect.display.height = size.height;
    
    return GFX_SUCCESS;
}

uint32_t defLayerBufferCountGet(void)
{
    return GFX_ActiveContext()->layer.active->buffer_count;
}

GFX_Result defLayerBufferCountSet(uint32_t count)
{
    uint32_t i;
    GFX_Context* context = GFX_ActiveContext();
    GFX_Layer* layer = context->layer.active;
    
    if(layer->buffer_count == count)
        return GFX_SUCCESS;
    
    // delete buffers past buffer count
    if(count < layer->buffer_count)
    {
        for(i = count; i < layer->buffer_count; i++)
            layerBufferFree(context, layer, i);
    }
    else
    {
        // set up buffer struct but don't allocate memory
        for(i = layer->buffer_count; i < count; i++)
        {
            layer->buffers[i].coherent = GFX_FALSE;             
            layer->buffers[i].state = GFX_BS_NONE;
            layer->buffers[i].driver_data = NULL;
            
            GFX_PixelBufferCreate(layer->rect.display.width,
                                  layer->rect.display.height,
                                  context->colorMode,
                                  NULL,
                                  &layer->buffers[i].pb);
        }        
    }
        
    layer->buffer_count = count;
    layer->buffer_read_idx = 0;
    
    if (layer->buffer_count > 1)
        layer->buffer_write_idx = 1;
    else
        layer->buffer_write_idx = 0;

    return GFX_SUCCESS;
}

GFX_Buffer defLayerBufferAddressGet(uint32_t idx)
{
    return GFX_ActiveContext()->layer.active->buffers[idx].pb.pixels;
}

GFX_Result defLayerBufferAddressSet(uint32_t idx, GFX_Buffer address)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Layer* layer = context->layer.active;
    
    if(layer->buffers[idx].state != GFX_BS_NONE)
        return GFX_FAILURE;
    
    if(address != NULL)
    {
        layer->buffers[idx].pb.pixels = address;
        layer->buffers[idx].state = GFX_BS_ADDRESS;
    
        GFX_PixelBufferCreate(layer->rect.display.width,
                              layer->rect.display.height,
                              context->colorMode,
                              address,
                              &layer->buffers[idx].pb);
    }
    else
    {
        layer->buffers[idx].pb.pixels = NULL;
        
        GFX_PixelBufferDestroy(&layer->buffers[idx].pb, &context->memory);
        
        layer->buffers[idx].state = GFX_BS_NONE;
    }
    
    return GFX_SUCCESS;
}

GFX_Bool defLayerBufferCoherentGet(uint32_t idx)
{
    return GFX_ActiveContext()->layer.active->buffers[idx].coherent;
}

GFX_Result defLayerBufferCoherentSet(uint32_t idx, GFX_Bool coherent)
{
    GFX_ActiveContext()->layer.active->buffers[idx].coherent = coherent;
    
    return GFX_SUCCESS;
}

GFX_Result defLayerBufferAllocate(uint32_t idx)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Layer* layer = context->layer.active;
    
    return layerBufferAllocate(context, layer, idx);
}

GFX_Bool defLayerBufferIsAllocated(uint32_t idx)
{
    return GFX_ActiveContext()->layer.active->buffers[idx].state == GFX_BS_MALLOC;
}

GFX_Result defLayerBufferFree(uint32_t idx)
{
    GFX_Context* context = GFX_ActiveContext();
    
    return layerBufferFree(context, context->layer.active, idx);
}

GFX_Bool defLayerVsyncGet(void)
{
    return GFX_ActiveContext()->layer.active->vsync;
}

GFX_Result defLayerVsyncSet(GFX_Bool vsync)
{
    GFX_ActiveContext()->layer.active->vsync = vsync;
    
    return GFX_SUCCESS;
}

GFX_Bool defLayerInvalidGet(void)
{
    return GFX_ActiveContext()->layer.active->invalid;
}

GFX_Result defLayerInvalidSet(GFX_Bool invalid)
{
    GFX_ActiveContext()->layer.active->invalid = invalid;
    
    return GFX_SUCCESS;
}

GFX_Bool defLayerSwapSyncGet(void)
{
    return GFX_ActiveContext()->layerSwapSync;
}

GFX_Result defLayerSwapSyncSet(GFX_Bool swapSync)
{
    GFX_ActiveContext()->layerSwapSync = swapSync;
    
    return GFX_SUCCESS;
}

GFX_Bool defLayerSwapGet(void)
{
    return GFX_ActiveContext()->layer.active->swap;
}

GFX_Result defLayerSwapSet(GFX_Bool value)
{
    GFX_Context* context = GFX_ActiveContext();
    
    if(context->layer.active->locked == GFX_TRUE)
        return GFX_FAILURE;
    
    context->layer.active->swap = value;
    
    if(context->layer.active->vsync == GFX_TRUE &&
       context->layer.active->buffer_count > 1)
    {
        context->hal.layerSwapPending(context->layer.active);
    }
    else
    {
        GFX_LayerSwap(context->layer.active);
    }
    
    return GFX_SUCCESS;
}

void defLayerSwapPending(GFX_Layer* layer)
{ }

void defLayerSwapped(GFX_Layer* layer)
{ }

GFX_Bool defLayerAlphaEnableGet(void)
{
    return GFX_ActiveContext()->layer.active->alphaEnable;
}

GFX_Result defLayerAlphaEnableSet(GFX_Bool enable, GFX_Bool wait)
{
    GFX_ActiveContext()->layer.active->alphaEnable = enable;
    
    return GFX_SUCCESS;
}

uint32_t defLayerAlphaAmountGet(void)
{
    return GFX_ActiveContext()->layer.active->alphaAmount;
}

GFX_Result defLayerAlphaAmountSet(uint32_t amount, GFX_Bool wait)
{
    if(amount > 255)
        amount = 255;
        
    GFX_ActiveContext()->layer.active->alphaAmount = amount;
    
    return GFX_SUCCESS;
}

GFX_Bool defLayerMaskEnableGet(void)
{
    return GFX_ActiveContext()->layer.active->maskEnable;
}

GFX_Result defLayerMaskEnableSet(GFX_Bool enable, GFX_Bool wait)
{
    GFX_ActiveContext()->layer.active->maskEnable = enable;
    
    return GFX_SUCCESS;
}

GFX_Color defLayerMaskColorGet(void)
{
    return GFX_ActiveContext()->layer.active->maskColor;
}

GFX_Result defLayerMaskColorSet(GFX_Color color, GFX_Bool wait)
{
    GFX_ActiveContext()->layer.active->maskColor = color;
    
    return GFX_SUCCESS;
}

GFX_Point defOrientPoint(const GFX_Point* point,
                         const GFX_Rect* rect,
                         GFX_Orientation ori)
{
    return GFX_UtilOrientPoint(point, rect, ori);
}

GFX_Point defMirrorPoint(const GFX_Point* pnt,
                         const GFX_Rect* rect,
                         GFX_Orientation ori)
{
    return GFX_UtilMirrorPoint(pnt, rect, ori);    
}

GFX_Point defMirrorPointEmpty(const GFX_Point* pnt,
                              const GFX_Rect* rect,
                              GFX_Orientation ori)
{
    GFX_Point res = *pnt;
    
    return res;    
}

#if GFX_DRAW_PIPELINE_ENABLED

GFX_PipelineMode defDrawPipelineModeGet(void)
{
    return GFX_ActiveContext()->draw.pipelineMode;
}

GFX_Result defDrawPipelineModeSet(GFX_PipelineMode mode)
{
    GFX_ActiveContext()->draw.pipelineMode = mode;
    GFX_ActiveContext()->draw.pipeline = &GFX_ActiveContext()->hal.drawPipeline[mode];
    
    return GFX_SUCCESS;
}

GFX_DrawMode defDrawModeGet(void)
{
    return GFX_ActiveContext()->draw.mode;
}

GFX_Result defDrawModeSet(GFX_DrawMode mode)
{    
    GFX_ActiveContext()->draw.mode = mode;
    
    return GFX_SUCCESS;
}

GFX_Color defDrawColorGet(void)
{
    return GFX_ActiveContext()->draw.color;
}

GFX_Result defDrawColorSet(GFX_Color color)
{   
    GFX_ActiveContext()->draw.color = color;
    
    return GFX_SUCCESS;
}

void defDrawGradientColorGet(GFX_Color* c0,
                             GFX_Color* c1,
                             GFX_Color* c2,
                             GFX_Color* c3)
{
    GFX_Context* context = GFX_ActiveContext();
    
    *c0 = context->draw.gradient.c0;
    *c1 = context->draw.gradient.c1;
    *c2 = context->draw.gradient.c2;
    *c3 = context->draw.gradient.c3;
}

GFX_Result defDrawGradientColorSet(GFX_Color c0,
                                   GFX_Color c1,
                                   GFX_Color c2,
                                   GFX_Color c3)
{   
    GFX_Context* context = GFX_ActiveContext();
    
    context->draw.gradient.c0 = c0;
    context->draw.gradient.c1 = c1;
    context->draw.gradient.c2 = c2;
    context->draw.gradient.c3 = c3;
    
    return GFX_SUCCESS;
}

GFX_Result defDrawPaletteGet(GFX_PixelBuffer* pb)
{
    *pb = GFX_ActiveContext()->draw.palette;
    
    return GFX_SUCCESS;
}

GFX_Result defDrawPaletteSet(const GFX_PixelBuffer* pb)
{
    GFX_Context* context = GFX_ActiveContext();
    
    context->draw.palette.buffer_length = 0;
    context->draw.palette.mode = 0;
    context->draw.palette.pixels = GFX_NULL;
    context->draw.palette.size.width = 0;
    context->draw.palette.size.height = 0;
    
    // null palette is OK
    if(pb == GFX_NULL)
    {
        return GFX_SUCCESS;
    }
    // palettes must be a valid actual color format (not indexed)
    else if(pb->mode < 0 || pb->mode > GFX_COLOR_MODE_LAST_COLOR)
    {
        return GFX_FAILURE;
    }
    else
    {
        context->draw.palette = *pb;
        
        return GFX_SUCCESS;
    }
}

GFX_Result defDrawTargetGet(GFX_PixelBuffer** pb)
{
    *pb = (GFX_PixelBuffer*)GFX_ActiveContext()->draw.target;
    
    return GFX_SUCCESS;
}

GFX_Result defDrawTargetSet(const GFX_PixelBuffer* pb)
{
    GFX_Context* context = GFX_ActiveContext();
    
    context->draw.target = pb;
    
    if(pb != NULL)
    {
        context->draw.targetClipRect.x = 0;
        context->draw.targetClipRect.y = 0;
        context->draw.targetClipRect.width = pb->size.width;
        context->draw.targetClipRect.height = pb->size.height;
    }
    
    return GFX_SUCCESS;
}

GFX_BlendMode defDrawBlendModeGet(void)
{    
    return GFX_ActiveContext()->draw.blendMode;
}

GFX_Result defDrawBlendModeSet(GFX_BlendMode mode)
{
    GFX_Context* context = GFX_ActiveContext();
    
    context->draw.blendMode = mode;
    
#if GFX_ALPHA_BLENDING_ENABLED
    if((mode & GFX_BLEND_CHANNEL) > 0)
        GFX_ActiveContext()->hal.alphaChannelBlend = &defAlphaChannelBlend;
    else
        GFX_ActiveContext()->hal.alphaChannelBlend = &defEmptyAlphaBlend;
    
    if((mode & GFX_BLEND_GLOBAL) > 0)
        GFX_ActiveContext()->hal.globalAlphaBlend = &defGlobalAlphaBlend;
    else
        GFX_ActiveContext()->hal.globalAlphaBlend = &defEmptyAlphaBlend;
        
    if(GFX_ActiveContext()->hal.alphaChannelBlend != &defEmptyAlphaBlend ||
       GFX_ActiveContext()->hal.globalAlphaBlend != &defEmptyAlphaBlend)
        GFX_ActiveContext()->hal.alphaGetPoint = &defAlphaGetPoint;
    else
        GFX_ActiveContext()->hal.alphaGetPoint = &defAlphaEmptyPoint;
#else
    context->draw.blendMode = 0;
#endif
    
    return GFX_SUCCESS;
}

GFX_ResizeMode defDrawResizeModeGet(void)
{
    return GFX_ActiveContext()->draw.resizeMode;
}

GFX_Result defDrawResizeModeSet(GFX_ResizeMode mode)
{
    GFX_ActiveContext()->draw.resizeMode = mode;
    
    return GFX_SUCCESS;
}

uint32_t defDrawAlphaEnableGet(void)
{
    return GFX_ActiveContext()->draw.alphaEnable;
}

GFX_Result defDrawAlphaEnableSet(GFX_Bool enable)
{    
    GFX_ActiveContext()->draw.alphaEnable = enable;
    
    return GFX_SUCCESS;
}

uint32_t defDrawAlphaValueGet(void)
{
    return GFX_ActiveContext()->draw.globalAlphaValue;
}

GFX_Result defDrawAlphaValueSet(uint32_t value)
{    
    GFX_ActiveContext()->draw.globalAlphaValue = value;
    
    return GFX_SUCCESS;
}

#if GFX_COLOR_MASKING_ENABLED
GFX_Bool defDrawMaskEnableGet(void)
{
    return GFX_ActiveContext()->draw.maskEnable;
}

GFX_Result defDrawMaskEnableSet(GFX_Bool enabled)
{    
    GFX_ActiveContext()->draw.maskEnable = enabled;
    
    if(enabled == GFX_TRUE)
        GFX_ActiveContext()->hal.maskColor = &defMaskColor;
    else
        GFX_ActiveContext()->hal.maskColor = &defMaskEmpty;
    
    return GFX_SUCCESS;
}

uint32_t defDrawMaskValueGet(void)
{
    return GFX_ActiveContext()->draw.maskValue;
}

GFX_Result defDrawMaskValueSet(uint32_t value)
{    
    GFX_ActiveContext()->draw.maskValue = value;
    
    return GFX_SUCCESS;
}

GFX_Bool defMaskColor(const GFX_DrawState* state,
                      GFX_Color color)
{
    return state->maskValue == color;
}
                        
GFX_Bool defMaskEmpty(const GFX_DrawState* state,
                      GFX_Color color)
{
    return GFX_FALSE;
}
#endif

uint32_t defDrawThicknessGet(void)
{
    return GFX_ActiveContext()->draw.thickness;
}

GFX_Result defDrawThicknessSet(uint32_t value)
{    
    GFX_ActiveContext()->draw.thickness = value;
    
    return GFX_SUCCESS;
}

GFX_Bool defDrawClipEnabledGet(void)
{
    return GFX_ActiveContext()->draw.clipEnable;
}

GFX_Result defDrawClipEnabledSet(GFX_Bool value)
{
    GFX_ActiveContext()->draw.clipEnable = value;
    
    return GFX_SUCCESS;
}

GFX_Result defDrawClipRectGet(GFX_Rect* rect)
{
    *rect = GFX_ActiveContext()->draw.clipRect;
    
    return GFX_SUCCESS;
}

GFX_Result defDrawClipRectSet(const GFX_Rect* rect)
{
    GFX_ActiveContext()->draw.clipRect = *rect;
    
    return GFX_SUCCESS;
}

#if GFX_ALPHA_BLENDING_ENABLED

GFX_Color  defAlphaGetPoint(const GFX_PixelBuffer* buffer,
                            const GFX_Point* pnt,
                            const GFX_DrawState* state)
{
    return state->pipeline->pixelGet(buffer, pnt);
}
                            
GFX_Color  defAlphaEmptyPoint(const GFX_PixelBuffer* buffer,
                              const GFX_Point* pnt,
                              const GFX_DrawState* state)
{
    return 0;
}

GFX_Color defAlphaChannelBlend(GFX_Color source,
                               GFX_Color dest,
                               GFX_ColorMode mode)
{
    GFX_Color rgba_source = GFX_ColorConvert(mode, GFX_COLOR_MODE_RGBA_8888, source);
    GFX_Color rgba_dest   = GFX_ColorConvert(mode, GFX_COLOR_MODE_RGBA_8888, dest);
    GFX_Color result;
       
    result = GFX_ColorBlend_RGBA_8888(rgba_source, rgba_dest);
    
    return GFX_ColorConvert(GFX_COLOR_MODE_RGBA_8888, mode, result);
}
                              
GFX_Color defGlobalAlphaBlend(GFX_Color source,
                              GFX_Color dest,
                              GFX_ColorMode mode)
{
    GFX_Color rgba_source = GFX_ColorConvert(mode, GFX_COLOR_MODE_RGBA_8888, source);
    GFX_Color rgba_dest   = GFX_ColorConvert(mode, GFX_COLOR_MODE_RGBA_8888, dest);
    GFX_Color result;
    
    rgba_source &= ~(RGBA_8888_ALPHA_MASK);
    rgba_source |= GFX_ActiveContext()->draw.globalAlphaValue;
    
    rgba_dest |= RGBA_8888_ALPHA_MASK;
    
    result =  GFX_ColorBlend_RGBA_8888(rgba_source, rgba_dest);
    
    return GFX_ColorConvert(GFX_COLOR_MODE_RGBA_8888, mode, result);
}

GFX_Color defEmptyAlphaBlend(GFX_Color source,
                             GFX_Color dest,
                             GFX_ColorMode mode)
{
    return source; // do nothing
}

#endif

#endif // GFX_DRAW_PIPELINE_ENABLED

GFX_Result defSwap(void)
{
    GFX_Context* context;
    GFX_Layer* layer;
    GFX_Bool l;
    GFX_Result res = GFX_SUCCESS;

    context = GFX_ActiveContext();
    
    // layer swap sync gate
    if(context->layerSwapSync == GFX_TRUE)
    {
        for(l = 0; l < context->layer.count; l++)
        {
            if(context->layer.layers[l].invalid == GFX_TRUE &&
               context->layer.layers[l].swap == GFX_FALSE)
               return GFX_FAILURE;
        }
    }
    
    // iterate over each layer and decide whether the context
    // needs to swap at the next opportunity
    for(l = 0; l < context->layer.count; l++)
    {
        layer = &context->layer.layers[l];
     
        if(layer->enabled == GFX_TRUE &&
           layer->visible == GFX_TRUE &&
           layer->swap == GFX_TRUE &&
           layer->locked == GFX_FALSE)
        {
            // turn on swap flag
            if(layer->vsync == GFX_TRUE)
            {                
                //printf("defswap: layer swap = TRUE\n");
                
                res = GFX_FAILURE;

                break;
            }
            // vsync is off, swap immediately
            else
            {
                GFX_LayerSwap(layer);
                
                //printf("defswap: swap immediately\n");
            }
        }
    }

    return res;
}

GFX_Result defInterrupt(uint32_t id)
{
    return GFX_SUCCESS;
}

GFX_Color defPixelGet(const GFX_PixelBuffer* buffer, const GFX_Point* pnt)
{
    return GFX_PixelBufferGet(buffer, pnt);
}

GFX_Result defPixelArrayGet(GFX_BufferSelection source,
                            const GFX_Rect* rect,
                            GFX_PixelBuffer* result)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Layer* layer;
    GFX_PixelBuffer* sourceBuffer;
    
    layer = context->layer.active;
    
    if(source == GFX_BUFFER_READ)
        sourceBuffer = &layer->buffers[layer->buffer_read_idx].pb;
    else
        sourceBuffer = &layer->buffers[layer->buffer_write_idx].pb;
        
    return GFX_PixelBufferAreaGet(sourceBuffer,
                                  rect,
                                  &context->memory,
                                  result);
}

#if GFX_DRAW_PIPELINE_ENABLED
GFX_Result defPixelSet(const GFX_PixelBuffer* buffer,
                       const GFX_Point* pnt,
                       GFX_Color color)
{
    return GFX_PixelBufferSet_Unsafe(buffer, pnt, color);
}
#endif // GFX_DRAW_PIPELINE_ENABLED


GFX_Result contextDefInitialization(GFX_Context* context)
{
#if GFX_DRAW_PIPELINE_ENABLED
    uint32_t i;
#endif
    
    if(context == NULL)
        return GFX_FAILURE;

    // set HAL interfaces
    context->hal.initialize = &defInitialize;
    context->hal.begin = &defBegin;
    context->hal.end = &defEnd;
    context->hal.update = &defUpdate;
    context->hal.destroy = &defDestroy;
    context->hal.brightnessRangeGet = &defBrightnessRangeGet;
    context->hal.brightnessGet = &defBrightnessGet;
    context->hal.brightnessSet = &defBrightnessSet;
    context->hal.vsyncCallbackGet = &defVSyncCallbackGet;
    context->hal.vsyncCallbackSet = &defVSyncCallbackSet;
    context->hal.hsyncCallbackGet = &defHSyncCallbackGet;
    context->hal.hsyncCallbackSet = &defHSyncCallbackSet;
    
    context->hal.orientationGet = &defOrientationGet;
    context->hal.orientationSet = &defOrientationSet;
    context->hal.mirroringGet = &defMirroringGet;
    context->hal.mirroringSet = &defMirroringSet;
    
    context->hal.colorModeGet = &defColorModeGet;
    context->hal.colorModeSet = &defColorModeSet;
    
    context->hal.globalPaletteGet = &defGlobalPaletteGet;
    context->hal.globalPaletteSet = &defGlobalPaletteSet;
    
    context->hal.layerEnabledGet = &defLayerEnabledGet;
    context->hal.layerEnabledSet = &defLayerEnabledSet;
    context->hal.layerBufferCountGet = &defLayerBufferCountGet;
    context->hal.layerBufferCountSet = &defLayerBufferCountSet;
    context->hal.layerBufferAddressGet = &defLayerBufferAddressGet;
    context->hal.layerBufferAddressSet = &defLayerBufferAddressSet;
    context->hal.layerBufferCoherentGet = &defLayerBufferCoherentGet;
    context->hal.layerBufferCoherentSet = &defLayerBufferCoherentSet;
    context->hal.layerBufferAllocate = &defLayerBufferAllocate;
    context->hal.layerBufferIsAllocated = &defLayerBufferIsAllocated;
    context->hal.layerBufferFree = &defLayerBufferFree;
    context->hal.layerVisibleGet = &defLayerVisibleGet;
    context->hal.layerVisibleSet = &defLayerVisibleSet;
    context->hal.layerVsyncGet = &defLayerVsyncGet;
    context->hal.layerVsyncSet = &defLayerVsyncSet;
    context->hal.layerInvalidGet = &defLayerInvalidGet;
    context->hal.layerInvalidSet = &defLayerInvalidSet;
    context->hal.layerSwapSyncGet = &defLayerSwapSyncGet;
    context->hal.layerSwapSyncSet = &defLayerSwapSyncSet;
    context->hal.layerSwapGet = &defLayerSwapGet;
    context->hal.layerSwapSet = &defLayerSwapSet;
    context->hal.layerSwapPending = &defLayerSwapPending;
    context->hal.layerSwapped = &defLayerSwapped;
    context->hal.layerActiveGet = &defLayerActiveGet;
    context->hal.layerActiveSet = &defLayerActiveSet;
    context->hal.layerPositionGet = &defLayerPositionGet;
    context->hal.layerPositionSet = &defLayerPositionSet;
    context->hal.layerSizeGet = &defLayerSizeGet;
    context->hal.layerSizeSet = &defLayerSizeSet;
    context->hal.layerAlphaEnableGet = &defLayerAlphaEnableGet;
    context->hal.layerAlphaEnableSet = &defLayerAlphaEnableSet;
    context->hal.layerAlphaAmountGet = &defLayerAlphaAmountGet;
    context->hal.layerAlphaAmountSet = &defLayerAlphaAmountSet;
    context->hal.layerMaskEnableGet = &defLayerMaskEnableGet;
    context->hal.layerMaskEnableSet = &defLayerMaskEnableSet;
    context->hal.layerMaskColorGet = &defLayerMaskColorGet;
    context->hal.layerMaskColorSet = &defLayerMaskColorSet;
        
#if GFX_DRAW_PIPELINE_ENABLED
    context->hal.orientPoint = &defOrientPoint;
    context->hal.mirrorPoint = &defMirrorPointEmpty;
    
    context->hal.drawPipelineModeGet = &defDrawPipelineModeGet;
    context->hal.drawPipelineModeSet = &defDrawPipelineModeSet;
    
    context->hal.drawModeGet = &defDrawModeGet;
    context->hal.drawModeSet = &defDrawModeSet;
    context->hal.drawColorGet = &defDrawColorGet;
    context->hal.drawColorSet = &defDrawColorSet;
    context->hal.drawGradientColorGet = &defDrawGradientColorGet;
    context->hal.drawGradientColorSet = &defDrawGradientColorSet;
    context->hal.drawPaletteGet = &defDrawPaletteGet;
    context->hal.drawPaletteSet = &defDrawPaletteSet;
    context->hal.drawTargetGet = &defDrawTargetGet;
    context->hal.drawTargetSet = &defDrawTargetSet;
    context->hal.drawBlendModeGet = &defDrawBlendModeGet;
    context->hal.drawBlendModeSet = &defDrawBlendModeSet;
    context->hal.drawResizeModeGet = &defDrawResizeModeGet;
    context->hal.drawResizeModeSet = &defDrawResizeModeSet;
    context->hal.drawAlphaEnableGet = &defDrawAlphaEnableGet;
    context->hal.drawAlphaEnableSet = &defDrawAlphaEnableSet;
    context->hal.drawAlphaValueGet = &defDrawAlphaValueGet;
    context->hal.drawAlphaValueSet = &defDrawAlphaValueSet;
    
#if GFX_COLOR_MASKING_ENABLED
    context->hal.drawMaskEnableGet = &defDrawMaskEnableGet;
    context->hal.drawMaskEnableSet = &defDrawMaskEnableSet;
    context->hal.drawMaskValueGet = &defDrawMaskValueGet;
    context->hal.drawMaskValueSet = &defDrawMaskValueSet;

    context->hal.maskColor = &defMaskEmpty;
#endif

    context->hal.drawThicknessGet = &defDrawThicknessGet;
    context->hal.drawThicknessSet = &defDrawThicknessSet;
    context->hal.drawClipEnableGet = &defDrawClipEnabledGet;
    context->hal.drawClipEnableSet = &defDrawClipEnabledSet;
    context->hal.drawClipRectGet = &defDrawClipRectGet;
    context->hal.drawClipRectSet = &defDrawClipRectSet;
    
#if GFX_ALPHA_BLENDING_ENABLED
    context->hal.alphaGetPoint = &defAlphaEmptyPoint;
    context->hal.alphaChannelBlend = &defEmptyAlphaBlend;
    context->hal.globalAlphaBlend = &defEmptyAlphaBlend;
#endif
    
    for(i = 0; i < GFX_PIPELINE_MODE_COUNT; i++)
    {
        context->hal.drawPipeline[i].drawPixel[GFX_ANTIALIAS_OFF] = &cpuDrawPixel;
        context->hal.drawPipeline[i].drawPixel[GFX_ANTIALIAS_ON] = &cpuDrawPixel;
        
        context->hal.drawPipeline[i].drawLine[GFX_DRAW_LINE][GFX_ANTIALIAS_OFF] = &cpuDrawLine;
        context->hal.drawPipeline[i].drawLine[GFX_DRAW_FILL][GFX_ANTIALIAS_OFF] = &cpuDrawLine;
        context->hal.drawPipeline[i].drawLine[GFX_DRAW_GRADIENT_LEFT_RIGHT][GFX_ANTIALIAS_OFF] = &cpuDrawLine;
        context->hal.drawPipeline[i].drawLine[GFX_DRAW_GRADIENT_TOP_BOTTOM][GFX_ANTIALIAS_OFF] = &cpuDrawLine;
        
        context->hal.drawPipeline[i].drawHorzLine[GFX_DRAW_LINE][GFX_ANTIALIAS_OFF] = &cpuDrawLine_Horz;
        context->hal.drawPipeline[i].drawHorzLine[GFX_DRAW_FILL][GFX_ANTIALIAS_OFF] = &cpuDrawLine_Horz;
        context->hal.drawPipeline[i].drawHorzLine[GFX_DRAW_GRADIENT_LEFT_RIGHT][GFX_ANTIALIAS_OFF] = &cpuDrawLine_Horz;
        context->hal.drawPipeline[i].drawHorzLine[GFX_DRAW_GRADIENT_TOP_BOTTOM][GFX_ANTIALIAS_OFF] = &cpuDrawLine_Horz;
        
        context->hal.drawPipeline[i].drawVertLine[GFX_DRAW_LINE][GFX_ANTIALIAS_OFF] = &cpuDrawLine_Vert;
        context->hal.drawPipeline[i].drawVertLine[GFX_DRAW_FILL][GFX_ANTIALIAS_OFF] = &cpuDrawLine_Vert;
        context->hal.drawPipeline[i].drawVertLine[GFX_DRAW_GRADIENT_LEFT_RIGHT][GFX_ANTIALIAS_OFF] = &cpuDrawLine_Vert;
        context->hal.drawPipeline[i].drawVertLine[GFX_DRAW_GRADIENT_TOP_BOTTOM][GFX_ANTIALIAS_OFF] = &cpuDrawLine_Vert;
        
        context->hal.drawPipeline[i].drawLine[GFX_DRAW_LINE][GFX_ANTIALIAS_ON] = &cpuDrawLine_AA;
        context->hal.drawPipeline[i].drawLine[GFX_DRAW_FILL][GFX_ANTIALIAS_ON] = &cpuDrawLine_AA;
        context->hal.drawPipeline[i].drawLine[GFX_DRAW_GRADIENT_LEFT_RIGHT][GFX_ANTIALIAS_ON] = &cpuDrawLine_AA;
        context->hal.drawPipeline[i].drawLine[GFX_DRAW_GRADIENT_TOP_BOTTOM][GFX_ANTIALIAS_ON] = &cpuDrawLine_AA;
        
        context->hal.drawPipeline[i].drawRect[GFX_DRAW_LINE][GFX_ANTIALIAS_OFF] = &cpuDrawRect_Line;
        context->hal.drawPipeline[i].drawRect[GFX_DRAW_FILL][GFX_ANTIALIAS_OFF] = &cpuDrawRect_Fill;
        context->hal.drawPipeline[i].drawRect[GFX_DRAW_GRADIENT_LEFT_RIGHT][GFX_ANTIALIAS_OFF] = &cpuDrawRect_Gradient_LeftRight;
        context->hal.drawPipeline[i].drawRect[GFX_DRAW_GRADIENT_TOP_BOTTOM][GFX_ANTIALIAS_OFF] = &cpuDrawRect_Gradient_TopBottom;
        
        context->hal.drawPipeline[i].drawRect[GFX_DRAW_LINE][GFX_ANTIALIAS_ON] = &cpuDrawRect_Line_AA;
        context->hal.drawPipeline[i].drawRect[GFX_DRAW_FILL][GFX_ANTIALIAS_ON] = &cpuDrawRect_Fill_AA;
        
        context->hal.drawPipeline[i].drawCircle[GFX_DRAW_LINE][GFX_ANTIALIAS_OFF] = &cpuDrawCircle_Line;
        
#if GFX_DRAW_ARC_ENABLED
		context->hal.drawPipeline[i].drawArc[GFX_DRAW_LINE][GFX_ANTIALIAS_OFF] = &cpuDrawArc_Fill;
        context->hal.drawPipeline[i].drawArc[GFX_DRAW_FILL][GFX_ANTIALIAS_OFF] = &cpuDrawArc_Fill;
        context->hal.drawPipeline[i].drawArc[GFX_DRAW_FILL][GFX_ANTIALIAS_ON] = &cpuDrawArc_Fill;
#endif

#if GFX_DRAW_ELLIPSE_ENABLED
		context->hal.drawPipeline[i].drawEllipse[GFX_DRAW_LINE][GFX_ANTIALIAS_OFF] = &cpuDrawEllipse_Line;
        context->hal.drawPipeline[i].drawEllipse[GFX_DRAW_FILL][GFX_ANTIALIAS_OFF] = &cpuDrawEllipse_Line;
        context->hal.drawPipeline[i].drawEllipse[GFX_DRAW_FILL][GFX_ANTIALIAS_ON] = &cpuDrawEllipse_Line;
#endif

        context->hal.drawPipeline[i].drawBlit = &cpuDrawBlit;
        
        context->hal.drawPipeline[i].drawStretchBlit[GFX_RESIZE_NEARESTNEIGHBOR] = &cpuDrawStretchBlit_NearestNeighbor;
        context->hal.drawPipeline[i].drawStretchBlit[GFX_RESIZE_BILINEAR] = &cpuDrawStretchBlit_Bilinear;
        
        context->hal.drawPipeline[i].drawDirectBlit = &cpuDrawDirectBlit;
        
        context->hal.drawPipeline[i].pixelGet = &defPixelGet;
        context->hal.drawPipeline[i].pixelArrayGet = &defPixelArrayGet;
        context->hal.drawPipeline[i].pixelSet = &defPixelSet;
    }

#endif // GFX_DRAW_PIPELINE_ENABLED

    context->hal.interrupt = &defInterrupt;
    
    return GFX_SUCCESS;
}