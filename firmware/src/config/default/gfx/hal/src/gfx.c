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


#include "gfx/hal/gfx.h"

#include <stdlib.h>
#include <string.h>

#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_default_impl.h"
#include "gfx/hal/inc/gfx_display.h"
#include "gfx/hal/inc/gfx_driver_interface.h"
#include "gfx/hal/inc/gfx_hal.h"
#include "gfx/hal/inc/gfx_interface.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_processor_interface.h"

GFX_Result GFX_Initialize(void)
{
    if(GFX_DriverIntfInitialize() == GFX_FAILURE)
        return GFX_FAILURE;

    return GFX_SUCCESS;
}

GFX_Handle GFX_Open(GFX_Driver driver,
                    GFX_Display display,
                    GFX_Processor processor,
                    GFX_MemoryIntf* memintf)
{
    GFX_Context* context;
    GFX_Result res = GFX_FAILURE;
    GFX_DriverInfo info;
    GFX_DriverIntf drvIface;
#if GFX_PROCESSOR_COUNT > 0
    GFX_ProcessorIntf procIface;
#endif
    GFX_MemoryIntf mem;
    uint32_t i;

#if GFX_DRAW_PIPELINE_ENABLED
    GFX_ColorMode minDisplayMode;
#endif

    GFX_ASSERT(driver < GFX_DRIVER_COUNT);
    GFX_ASSERT(display < GFX_DISPLAY_COUNT);

    if(driver >= GFX_DRIVER_COUNT || display >= GFX_DISPLAY_COUNT)
        return NULL;

    // create a default memory interface if needed
    if(memintf == GFX_NULL)
    {
        mem.malloc = (GFX_Malloc_FnPtr) &malloc;
#if defined(WIN32)
        mem.coherent_alloc = (GFX_Malloc_FnPtr) &malloc;
#else
        //mem.coherent_alloc = &__pic32_alloc_coherent;
        mem.coherent_alloc = (GFX_Malloc_FnPtr) &malloc; // FIXME for H3
#endif
        mem.calloc = (GFX_Calloc_FnPtr) &calloc;
        mem.realloc = (GFX_Realloc_FnPtr) &realloc;
        mem.free = (GFX_Free_FnPtr) &free;
#if defined(WIN32)
        mem.coherent_free = (GFX_Free_FnPtr) &free;
#else
        //mem.coherent_free = (GFX_Free_FnPtr) &__pic32_free_coherent;
        mem.coherent_free = (GFX_Free_FnPtr) &free; // FIXME for H3
#endif
        mem.memset = (GFX_Memset_FnPtr) &memset;
        mem.memcpy = (GFX_Memcpy_FnPtr) &memcpy;
    }
    else
        mem = *memintf;

    // get the driver interface
    GFX_DriverIntfGet(driver, &drvIface);
    drvIface.infoGet(&info);

    context = mem.calloc(1, sizeof(GFX_Context));

    if(context == NULL)
        return NULL;

    // set context memory interface
    context->memory = mem;

    // set display info
    context->display_idx = display;
    context->display_info = GFX_DisplayInfoPtrGet(display);

#if GFX_DRAW_PIPELINE_ENABLED
    minDisplayMode = 0;


    // determine minimum display mode format
    for(i = 0; i < GFX_COLOR_MODE_LAST_COLOR; i++)
    {
        if((context->display_info->color_formats & (1 << i)) != 0)
        {
            minDisplayMode = i;

            break;
        }
    }

    context->draw.colorMode = minDisplayMode;
#endif

    context->brightness = 0;

    // allocate layers
    context->layer.count = info.layer_count;
    context->layer.layers = context->memory.calloc(info.layer_count, sizeof(GFX_Layer));

    if(context->layer.layers == NULL)
        return NULL;

    // set layer IDs
    for(i = 0; i < context->layer.count; i++)
    {
        context->layer.layers[i].id = i;
        context->layer.layers[i].vsync = GFX_TRUE;
    }

    // set layer 0 as active by default
    context->layer.active = &context->layer.layers[0];

    // set layer 0 size to the screen size by default
    context->layer.layers[0].rect.display = context->display_info->rect;
    context->layer.layers[0].rect.local = context->layer.layers[0].rect.display;

    // default sync callbacks
    context->vsyncCB = &_GFX_DefaultSyncCallback;
    context->hsyncCB = &_GFX_DefaultSyncCallback;

#if GFX_DRAW_PIPELINE_ENABLED
    // set initial draw state
    context->draw.mode = GFX_DRAW_LINE;
    context->draw.color = 0x0;
    context->draw.gradient.c0 = 0x0;
    context->draw.gradient.c1 = 0x0;
    context->draw.gradient.c2 = 0x0;
    context->draw.gradient.c3 = 0x0;
    context->draw.blendMode = GFX_BLEND_NONE;
    context->draw.alphaEnable = GFX_TRUE;
    context->draw.globalAlphaValue = 255;
    context->draw.maskEnable = GFX_FALSE;
    context->draw.maskValue = 0;
    context->draw.thickness = 1;
#if GFX_PROCESSOR_COUNT == 0
    context->draw.pipelineMode = GFX_PIPELINE_GCU;
    context->draw.pipeline = &context->hal.drawPipeline[GFX_PIPELINE_GCU];
#else
    context->draw.pipelineMode = GFX_PIPELINE_GCUGPU;
    context->draw.pipeline = &context->hal.drawPipeline[GFX_PIPELINE_GCUGPU];
#endif

    // set initial clip space (entire display)
    context->draw.clipRect.width = context->display_info->rect.width;
    context->draw.clipRect.height = context->display_info->rect.height;
#endif // GFX_DRAW_PIPELINE_ENABLED

    // default driver hal initialization
    res = contextDefInitialization(context);

    if(res == GFX_FAILURE)
    {
        GFX_Close(context);

        return NULL;
    }

    // driver-specific context initialization
    res = drvIface.contextInitialize(context);

    if(res == GFX_FAILURE)
    {
        GFX_Close(context);

        return NULL;
    }

    // driver initialize
    res = context->hal.initialize(context);

    if(res == GFX_FAILURE)
    {
        GFX_Close(context);

        return NULL;
    }

#if GFX_PROCESSOR_COUNT > 0
    // processor initialize
    if(processor >= 0 && processor < GFX_PROCESSOR_COUNT)
    {
        GFX_InitializeProcessorList();

        res = GFX_ProcessorIntfGet(processor, &procIface);

        if(res == GFX_FAILURE)
            return NULL;

        res = procIface.contextInitialize(context);

        if(res == GFX_FAILURE)
        {
            GFX_Close(context);

            return NULL;
        }
    }
#endif

    // set as active context if no exists
    if(GFX_ActiveContext() == NULL)
        GFX_ContextActiveSet(context);

    return (GFX_Handle*)context;
}

GFX_Result GFX_Update()
{
    GFX_Context* context = GFX_ActiveContext();

    if(context == NULL)
        return GFX_FAILURE;

    return context->hal.update();
}

void GFX_Close()
{
    GFX_Context* context;
    uint32_t i, j;

    context = GFX_ActiveContext();

    if(context == NULL)
        return;

    // driver destroy
    context->hal.destroy(context);

    // free driver data
    if(context->driver_data != GFX_NULL)
    {
        free(context->driver_data);
        context->driver_data = GFX_NULL;
    }

    // free driver layers
    if(context->layer.layers != NULL)
    {
        for(i = 0; i < context->layer.count; i++)
        {
            for(j = 0; j < GFX_MAX_BUFFER_COUNT; j++)
            {
                if(context->layer.layers[i].buffers[i].pb.pixels == NULL)
                    continue;

                // only free layer buffers that have been explicitely malloced
                // by the application
                if(context->layer.layers[i].buffers[j].state == GFX_BS_MALLOC)
                {
                    if(context->layer.layers[i].buffers[j].pb.pixels != GFX_NULL)
                    {
                        context->memory.free(context->layer.layers[i].buffers[j].pb.pixels);
                        context->layer.layers[i].buffers[j].pb.pixels = GFX_NULL;
                    }
                }
            }
        }

        if(context->layer.layers != GFX_NULL)
        {
            context->memory.free(context->layer.layers);
            context->layer.layers = GFX_NULL;
        }
    }

    // free context
    context->memory.free(context);
}

GFX_Result GFX_Begin()
{
    GFX_Context* context;

    context = GFX_ActiveContext();

    if(context == NULL)
        return GFX_FAILURE;

    return context->hal.begin();
}

void GFX_End()
{
    GFX_Context* context = GFX_ActiveContext();

    if(context == NULL)
        return;

    context->hal.end();
}

GFX_Result GFX_Get(GFX_Flag flag, ...)
{
    GFX_Result result = GFX_FAILURE;
    va_list args;

    if(flag > GFXF_LAST_FLAG)
        return GFX_FAILURE;

    va_start(args, flag);

    result = GFX_InterfaceTable[flag][GFX_GET](args);

    va_end(args);

    return result;
}

GFX_Result GFX_Set(GFX_Flag flag, ...)
{
    GFX_Result result = GFX_FAILURE;
    va_list args;

    GFX_Context* context = GFX_ActiveContext();

    if(flag > GFXF_LAST_FLAG ||
       context == NULL)
        return GFX_FAILURE;

    va_start(args, flag);

    result = GFX_InterfaceTable[flag][GFX_SET](args);

    va_end(args);

    return result;
}

GFX_Result GFX_HandleInterrupt(uint32_t id)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_ASSERT(context != NULL);

    if(context == NULL)
        return GFX_FAILURE;

    return context->hal.interrupt(id);
}

#if GFX_DRAW_PIPELINE_ENABLED
GFX_Result GFX_ReadPixel(GFX_BufferSelection source,
                         int32_t x,
                         int32_t y,
                         GFX_Color* color)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Layer* layer;
    GFX_PixelBuffer* sourceBuffer;
    GFX_Point pnt;

    GFX_ASSERT(context != NULL);

    if(context == NULL || color == NULL)
        return GFX_FAILURE;

    layer = context->layer.active;

    if(source == GFX_BUFFER_READ)
        sourceBuffer = &layer->buffers[layer->buffer_read_idx].pb;
    else
        sourceBuffer = &layer->buffers[layer->buffer_write_idx].pb;

    pnt.x = x;
    pnt.y = y;

    if(pnt.x < 0 || pnt.x >= sourceBuffer->size.width ||
       pnt.y < 0 || pnt.y >= sourceBuffer->size.height)
       return GFX_FAILURE;

    *color = context->hal.drawPipeline[context->draw.pipelineMode].pixelGet(sourceBuffer, &pnt);

    return GFX_SUCCESS;
}

GFX_Result GFX_ReadPixelArray(GFX_BufferSelection source,
                              const GFX_Rect* rect,
                              GFX_PixelBuffer* result)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_ASSERT(context != NULL);

    if(context == NULL || rect == NULL || result == NULL)
        return GFX_FAILURE;

    return context->hal.drawPipeline[context->draw.pipelineMode].pixelArrayGet(source, rect, result);
}
#endif
