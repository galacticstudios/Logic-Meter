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


#include "gfx/hal/inc/gfx_interface.h"

#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_display.h"
#include "gfx/hal/inc/gfx_driver_interface.h"

GFX_Result set_unimplemented(va_list args)
{
    args = args;
    
    return GFX_FAILURE;
}

GFX_Result set_brightness(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t val = va_arg(args, uint32_t);

    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.brightnessSet(val);
}

GFX_Result set_vsyncCallback(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_SyncCallback_FnPtr ptr = (GFX_SyncCallback_FnPtr) va_arg(args, int);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.vsyncCallbackSet(ptr);
}

GFX_Result set_hsyncCallback(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_SyncCallback_FnPtr ptr = (GFX_SyncCallback_FnPtr) va_arg(args, int);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.hsyncCallbackSet(ptr);
}

GFX_Result set_orientation(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Orientation ori = (GFX_Orientation) va_arg(args, int);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.orientationSet(ori);
}

GFX_Result set_mirroring(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool mirr = (GFX_Bool) va_arg(args, uint32_t);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.mirroringSet(mirr);
}

GFX_Result set_colorMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_ColorMode mode = (GFX_ColorMode) va_arg(args, int);
            
    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif
            
    return context->hal.colorModeSet(mode);
}

GFX_Result set_globalPalette(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_GlobalPalette palette = (GFX_GlobalPalette) va_arg(args, int);
            
    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif
            
    return context->hal.globalPaletteSet(palette);
}

GFX_Result set_layerActive(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t idx = va_arg(args, uint32_t);

#ifdef GFX_DEBUG
    if(context == NULL || idx >= context->layer.count)
        return GFX_FAILURE;
#endif

    return context->hal.layerActiveSet(idx);
}

GFX_Result set_layerEnabled(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool enabled = (GFX_Bool) va_arg(args, int);

    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.layerEnabledSet(enabled);
}

GFX_Result set_layerVisible(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool visible = (GFX_Bool) va_arg(args, int);

    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.layerVisibleSet(visible);
}

GFX_Result set_layerInvalid(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool invalid = va_arg(args, GFX_Bool);

    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.layerInvalidSet(invalid);
}

GFX_Result set_layerVsync(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool vsync = (GFX_Bool) va_arg(args, int);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.layerVsyncSet(vsync);
}

GFX_Result set_layerSwapSync(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool swapSync = va_arg(args, GFX_Bool);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.layerSwapSyncSet(swapSync);
}

GFX_Result set_layerSwap(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool swap = (GFX_Bool) va_arg(args, int);

    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.layerSwapSet(swap);
}

GFX_Result set_layerPosition(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    int32_t x = va_arg(args, int32_t);
    int32_t y = va_arg(args, int32_t);
            
    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif
            
    return context->hal.layerPositionSet(x, y);
}

GFX_Result set_layerSize(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t width = va_arg(args, uint32_t);
    uint32_t height = va_arg(args, uint32_t);
            
    GFX_ASSERT(context != NULL);
            
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif
            
    return context->hal.layerSizeSet(width, height);
}

GFX_Result set_layerAlphaEnable(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool enable = (GFX_Bool) va_arg(args, uint32_t);
    GFX_Bool wait = (GFX_Bool) va_arg(args, uint32_t);
    
    if(wait != GFX_TRUE)
        wait = GFX_FALSE;
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif   
    
    return context->hal.layerAlphaEnableSet(enable, wait);
}

GFX_Result set_layerAlphaAmount(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t amount = va_arg(args, uint32_t);
    GFX_Bool wait = (GFX_Bool) va_arg(args, uint32_t);
    
    if(wait != GFX_TRUE)
        wait = GFX_FALSE;
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif   
    
    return context->hal.layerAlphaAmountSet(amount, wait);
}

GFX_Result set_layerMaskEnable(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool enable = (GFX_Bool) va_arg(args, int);
    GFX_Bool wait = (GFX_Bool) va_arg(args, uint32_t);
    
    if(wait != GFX_TRUE)
        wait = GFX_FALSE;
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif   
    
    return context->hal.layerMaskEnableSet(enable, wait);
}

GFX_Result set_layerMaskColor(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Color color = (GFX_Color) va_arg(args, int);
    GFX_Bool wait = (GFX_Bool) va_arg(args, uint32_t);
    
    if(wait != GFX_TRUE)
        wait = GFX_FALSE;
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.layerMaskColorSet(color, wait);
}

GFX_Result set_layerBufferCount(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t count = va_arg(args, uint32_t);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif
    
    return context->hal.layerBufferCountSet(count);
}

GFX_Result set_layerBufferAddress(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t idx = va_arg(args, uint32_t);
    GFX_Buffer address = (GFX_Buffer) va_arg(args, int);
        
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif   
    
    return context->hal.layerBufferAddressSet(idx, address);
}

GFX_Result set_layerBufferCoherent(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t idx = va_arg(args, uint32_t);
    GFX_Bool coherent = (GFX_Bool) va_arg(args, int);
        
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif   
    
    return context->hal.layerBufferCoherentSet(idx, coherent);
}

GFX_Result set_layerBufferAllocated(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t idx = va_arg(args, uint32_t);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif  
    
    return context->hal.layerBufferAllocate(idx);
}

GFX_Result set_layerBufferFree(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t idx = va_arg(args, uint32_t);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif  
    
    return context->hal.layerBufferFree(idx);
}

#if GFX_DRAW_PIPELINE_ENABLED

GFX_Result set_drawPipelineMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_PipelineMode mode = (GFX_PipelineMode) va_arg(args, int);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.drawPipelineModeSet(mode);
}

GFX_Result set_drawMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_DrawMode mode = (GFX_DrawMode) va_arg(args, int);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.drawModeSet(mode);
}

GFX_Result set_drawColor(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Color color = (GFX_Color) va_arg(args, int);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.drawColorSet(color);
}

GFX_Result set_drawGradientColor(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Color tl = (GFX_Color) va_arg(args, int);
    GFX_Color tr = (GFX_Color) va_arg(args, int);
    GFX_Color bl = (GFX_Color) va_arg(args, int);
    GFX_Color br = (GFX_Color) va_arg(args, int);
    
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.drawGradientColorSet(tl, tr, bl, br);
}

GFX_Result set_drawPalette(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_PixelBuffer* pb = (GFX_PixelBuffer*) va_arg(args, int);
       
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.drawPaletteSet(pb);
}

GFX_Result set_drawTarget(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_PixelBuffer* pb = (GFX_PixelBuffer*) va_arg(args, int);
       
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif                                                               

    return context->hal.drawTargetSet(pb);
}

GFX_Result set_drawBlendMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_BlendMode mode = (GFX_BlendMode) va_arg(args, int);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif 
    
    return context->hal.drawBlendModeSet(mode);
}

GFX_Result set_drawResizeMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_ResizeMode mode = (GFX_ResizeMode) va_arg(args, int);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif 
    
    return context->hal.drawResizeModeSet(mode);
}

GFX_Result set_drawAlphaEnable(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool enable = (GFX_Bool) va_arg(args, int);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif 
    
    return context->hal.drawAlphaEnableSet(enable);
}

GFX_Result set_drawAlphaValue(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t alpha = va_arg(args, uint32_t);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif 
    
    return context->hal.drawAlphaValueSet(alpha);
}

GFX_Result set_drawMaskEnable(va_list args)
{
#if GFX_COLOR_MASKING_ENABLED
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool enable = (GFX_Bool) va_arg(args, int);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif 
    
    return context->hal.drawMaskEnableSet(enable);
#else
    return GFX_UNSUPPORTED;
#endif
}

GFX_Result set_drawMaskValue(va_list args)
{
#if GFX_COLOR_MASKING_ENABLED
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t mask = va_arg(args, uint32_t);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif 
    
    return context->hal.drawMaskValueSet(mask);
#else
    return GFX_UNSUPPORTED;
#endif
}

GFX_Result set_drawAntialias(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool aa = (GFX_Bool) va_arg(args, int);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif 
    
    return context->hal.drawAntialiasSet(aa);
}

GFX_Result set_drawThickness(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();
    
    uint32_t thickness = va_arg(args, uint32_t);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif 
    
    return context->hal.drawThicknessSet(thickness);
}

GFX_Result set_drawClipEnable(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool enabled = (GFX_Bool) va_arg(args, int);
            
    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif  
    
    return context->hal.drawClipEnableSet(enabled);
}

GFX_Result set_drawClipRect(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Rect* rect = (GFX_Rect*) va_arg(args, int);

    GFX_ASSERT(context != NULL);
    
#ifdef GFX_DEBUG
    if(context == NULL)
        return GFX_FAILURE;
#endif  

    return context->hal.drawClipRectSet(rect);
}

#endif // GFX_DRAW_PIPELINE_ENABLED