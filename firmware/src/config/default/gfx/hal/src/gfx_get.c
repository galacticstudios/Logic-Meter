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

GFX_Result get_unimplemented(va_list args)
{
    return GFX_FAILURE;
}

GFX_Result get_displayCount(va_list args)
{
    uint32_t* count = va_arg(args, uint32_t*);

    GFX_ASSERT(count != NULL);

#ifdef GFX_DEBUG
    if(count == NULL)
        return GFX_FAILURE;
#endif

    *count = GFX_DISPLAY_COUNT;

    return GFX_SUCCESS;
}

GFX_Result get_displayInfo(va_list args)
{
    uint32_t display = va_arg(args, uint32_t);
    void* info = va_arg(args, void*);

    GFX_ASSERT(display < GFX_DISPLAY_COUNT);
    GFX_ASSERT(info != NULL);

#ifdef GFX_DEBUG
    if(display >= GFX_DISPLAY_COUNT || info == NULL)
        return GFX_FAILURE;
#endif

    memcpy(info, GFX_DisplayInfoPtrGet(display), sizeof(GFX_DisplayInfo));

    return GFX_SUCCESS;
}

GFX_Result get_driverCount(va_list args)
{
    uint32_t* count = va_arg(args, uint32_t*);

    GFX_ASSERT(count != NULL);

#ifdef GFX_DEBUG
    if(count == NULL)
        return GFX_FAILURE;
#endif

    *count = GFX_DRIVER_COUNT;

    return GFX_SUCCESS;
}

GFX_Result get_driverInfo(va_list args)
{
    uint32_t driver = va_arg(args, uint32_t);
    void* info = va_arg(args, void*);

    GFX_ASSERT(driver < GFX_DISPLAY_COUNT);
    GFX_ASSERT(info != NULL);

#ifdef GFX_DEBUG
    if(driver >= GFX_DRIVER_COUNT ||
       info == NULL)
        return GFX_FAILURE;
#endif

    return GFX_DriverIntfPtrGet(driver)->infoGet(info);
}

GFX_Result get_brightnessRange(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* low = va_arg(args, uint32_t*);
    uint32_t* high = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && low != NULL && high != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || low == NULL || high == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.brightnessRangeGet(low, high);
}

GFX_Result get_brightness(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* count = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && count != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || count == NULL)
        return GFX_FAILURE;
#endif

    *count = context->hal.brightnessGet();

    return GFX_SUCCESS;
}

GFX_Result get_vsyncCallback(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_SyncCallback_FnPtr* cb = (GFX_SyncCallback_FnPtr*) va_arg(args, int);

    GFX_ASSERT(context != NULL && cb != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || cb == NULL)
        return GFX_FAILURE;
#endif

    *cb = context->hal.vsyncCallbackGet();

    return GFX_SUCCESS;
}

GFX_Result get_hsyncCallback(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_SyncCallback_FnPtr* cb = (GFX_SyncCallback_FnPtr*) va_arg(args, int);

    GFX_ASSERT(context != NULL && cb != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || cb == NULL)
        return GFX_FAILURE;
#endif

    *cb = context->hal.hsyncCallbackGet();

    return GFX_SUCCESS;
}

GFX_Result get_orientation(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Orientation* ori = (GFX_Orientation*) va_arg(args, int);

    GFX_ASSERT(context != NULL && ori != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || ori == NULL)
        return GFX_FAILURE;
#endif

    *ori = context->hal.orientationGet();

    return GFX_SUCCESS;
}

GFX_Result get_mirroring(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* mirr = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && mirr != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || mirr == NULL)
        return GFX_FAILURE;
#endif

    *mirr = context->hal.mirroringGet();

    return GFX_SUCCESS;
}

GFX_Result get_colorMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_ColorMode* mode = (GFX_ColorMode*) va_arg(args, int);

    GFX_ASSERT(context != NULL && mode != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || mode == NULL)
        return GFX_FAILURE;
#endif

    *mode = context->hal.colorModeGet();

    return GFX_SUCCESS;
}

GFX_Result get_globalPalette(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_GlobalPalette* pal = (GFX_GlobalPalette*) va_arg(args, int);

    GFX_ASSERT(context != NULL && pal != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || pal == NULL)
        return GFX_FAILURE;
#endif

    *pal = context->hal.globalPaletteGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerCount(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* count = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && count != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || count == NULL)
        return GFX_FAILURE;
#endif

    *count = context->layer.count;

    return GFX_SUCCESS;
}

GFX_Result get_layerActive(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* active = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && active != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || active == NULL)
        return GFX_FAILURE;
#endif

    *active = context->layer.active_idx;

    return GFX_SUCCESS;
}

GFX_Result get_layerEnabled(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* enabled = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && enabled != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || enabled == NULL)
        return GFX_FAILURE;
#endif

    *enabled = context->hal.layerEnabledGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerVisible(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* visible = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && visible != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || visible == NULL)
        return GFX_FAILURE;
#endif

    *visible = context->hal.layerVisibleGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerInvalid(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* invalid = va_arg(args, GFX_Bool*);

    GFX_ASSERT(context != NULL && invalid != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || invalid == NULL)
        return GFX_FAILURE;
#endif

    *invalid = context->hal.layerInvalidGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerVsync(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* vsync = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && vsync != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || vsync == NULL)
        return GFX_FAILURE;
#endif

    *vsync = context->hal.layerVsyncGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerSwapSync(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* swapSync = va_arg(args, GFX_Bool*);

    GFX_ASSERT(context != NULL && swapSync != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || swapSync == NULL)
        return GFX_FAILURE;
#endif

    *swapSync = context->hal.layerSwapSyncGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerSwap(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* swap = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && swap != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || swap == NULL)
        return GFX_FAILURE;
#endif

    *swap = context->hal.layerSwapGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerPosition(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    int32_t* x = va_arg(args, int32_t*);
    int32_t* y = va_arg(args, int32_t*);

    GFX_ASSERT(context != NULL && x != NULL && y != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || x == NULL || y == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.layerPositionGet(x, y);
}

GFX_Result get_layerSize(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    int32_t* width = va_arg(args, int32_t*);
    int32_t* height = va_arg(args, int32_t*);

    GFX_ASSERT(context != NULL && width != NULL && height != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || width == NULL || height == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.layerSizeGet(width, height);
}

GFX_Result get_layerAlphaEnable(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* enable = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && enable != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || enable == NULL)
        return GFX_FAILURE;
#endif

    *enable = context->hal.layerAlphaEnableGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerAlphaAmount(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* amount = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && amount != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || amount == NULL)
        return GFX_FAILURE;
#endif

    *amount = context->hal.layerAlphaAmountGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerMaskEnable(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* enable = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && enable != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || enable == NULL)
        return GFX_FAILURE;
#endif

    *enable = context->hal.layerMaskEnableGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerMaskColor(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Color* color = (GFX_Color*) va_arg(args, int);

    GFX_ASSERT(context != NULL && color != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || color == NULL)
        return GFX_FAILURE;
#endif

    *color = context->hal.layerMaskColorGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerBufferCount(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* count = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && count != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || count == NULL)
        return GFX_FAILURE;
#endif

    *count = context->hal.layerBufferCountGet();

    return GFX_SUCCESS;
}

GFX_Result get_layerBufferAddress(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t idx = va_arg(args, uint32_t);
    GFX_Buffer* address = (GFX_Buffer*) va_arg(args, int);

    GFX_ASSERT(context != NULL && address != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || address == NULL)
        return GFX_FAILURE;
#endif

    *address = context->hal.layerBufferAddressGet(idx);

    return GFX_SUCCESS;
}

GFX_Result get_layerBufferCoherent(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t idx = va_arg(args, uint32_t);
    GFX_Bool* coherent = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && coherent != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || coherent == NULL)
        return GFX_FAILURE;
#endif

    *coherent = context->hal.layerBufferCoherentGet(idx);

    return GFX_SUCCESS;
}

GFX_Result get_layerBufferAllocated(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t idx = va_arg(args, uint32_t);
    GFX_Bool* allocated = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && allocated != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || allocated == NULL)
        return GFX_FAILURE;
#endif

    *allocated = context->hal.layerBufferIsAllocated(idx);

    return GFX_SUCCESS;
}

#if GFX_DRAW_PIPELINE_ENABLED

GFX_Result get_drawPipelineMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_PipelineMode* mode = (GFX_PipelineMode*) va_arg(args, int);

    GFX_ASSERT(context != NULL && mode != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || mode == NULL)
        return GFX_FAILURE;
#endif

    *mode = context->hal.drawPipelineModeGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_DrawMode* mode = (GFX_DrawMode*) va_arg(args, int);

    GFX_ASSERT(context != NULL && mode != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || mode == NULL)
        return GFX_FAILURE;
#endif

    *mode = context->hal.drawModeGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawColor(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Color* color = (GFX_Color*) va_arg(args, int);

    GFX_ASSERT(context != NULL && color != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || color == NULL)
        return GFX_FAILURE;
#endif

    *color = context->hal.drawColorGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawGradientColor(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Color* tl = (GFX_Color*) va_arg(args, int);
    GFX_Color* tr = (GFX_Color*) va_arg(args, int);
    GFX_Color* bl = (GFX_Color*) va_arg(args, int);
    GFX_Color* br = (GFX_Color*) va_arg(args, int);

    GFX_ASSERT(context != NULL &&
           tl != NULL &&
           tr != NULL &&
           bl != NULL &&
           br != NULL);

#ifdef GFX_DEBUG
    if(context == NULL ||
       tl == NULL ||
       tr == NULL ||
       bl == NULL ||
       br == NULL)
        return GFX_FAILURE;
#endif

    context->hal.drawGradientColorGet(tl, tr, bl, br);

    return GFX_SUCCESS;
}

GFX_Result get_drawPalette(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_PixelBuffer* pb = (GFX_PixelBuffer*) va_arg(args, int);

    GFX_ASSERT(context != NULL && pb != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || pb == NULL)
        return GFX_FAILURE;
#endif

    context->hal.drawPaletteGet(pb);

    return GFX_SUCCESS;
}

GFX_Result get_drawTarget(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_PixelBuffer** pb = (GFX_PixelBuffer**) va_arg(args, int);

    GFX_ASSERT(context != NULL && pb != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || pb == NULL)
        return GFX_FAILURE;
#endif

    context->hal.drawTargetGet(pb);

    return GFX_SUCCESS;
}

GFX_Result get_drawBlendMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_BlendMode* mode = (GFX_BlendMode*) va_arg(args, int);

    GFX_ASSERT(context != NULL && mode != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || mode == NULL)
        return GFX_FAILURE;
#endif

    *mode = context->hal.drawBlendModeGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawResizeMode(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_ResizeMode* mode = (GFX_ResizeMode*) va_arg(args, int);

    GFX_ASSERT(context != NULL && mode != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || mode == NULL)
        return GFX_FAILURE;
#endif

    *mode = context->hal.drawResizeModeGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawAlphaEnable(va_list args)
{
#if GFX_ALPHA_BLENDING_ENABLED
    GFX_Context* context = GFX_ActiveContext();
    
    GFX_Bool* enabled = (GFX_Bool*) va_arg(args, int);
            
    GFX_ASSERT(context != NULL && enabled != NULL);
    
    *enabled = context->hal.drawAlphaEnableGet();
    
    return GFX_SUCCESS;
#else
    return GFX_UNSUPPORTED;
#endif
}

GFX_Result get_drawAlphaValue(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* value = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && value != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || value == NULL)
        return GFX_FAILURE;
#endif

    *value = context->hal.drawAlphaValueGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawMaskEnable(va_list args)
{
#if GFX_COLOR_MASKING_ENABLED
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* enabled = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && enabled != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || enabled == NULL)
        return GFX_FAILURE;
#endif

    *enabled = context->hal.drawMaskEnableGet();

    return GFX_SUCCESS;
#else
    return GFX_UNSUPPORTED;
#endif
}

GFX_Result get_drawMaskValue(va_list args)
{
#if GFX_COLOR_MASKING_ENABLED
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* value = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && value != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || value == NULL)
        return GFX_FAILURE;
#endif

    *value = context->hal.drawMaskValueGet();

    return GFX_SUCCESS;
#else
    return GFX_UNSUPPORTED;
#endif
}


GFX_Result get_drawAntialias(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* aa = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && aa != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || aa == NULL)
        return GFX_FAILURE;
#endif

    *aa = context->hal.drawAntialiasGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawThickness(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    uint32_t* thickness = va_arg(args, uint32_t*);

    GFX_ASSERT(context != NULL && thickness != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || thickness == NULL)
        return GFX_FAILURE;
#endif

    *thickness = context->hal.drawThicknessGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawClipEnable(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Bool* clip = (GFX_Bool*) va_arg(args, int);

    GFX_ASSERT(context != NULL && clip != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || clip == NULL)
        return GFX_FAILURE;
#endif

    *clip = context->hal.drawClipEnableGet();

    return GFX_SUCCESS;
}

GFX_Result get_drawClipRect(va_list args)
{
    GFX_Context* context = GFX_ActiveContext();

    GFX_Rect* rect = (GFX_Rect*) va_arg(args, int);

    GFX_ASSERT(context != NULL && rect != NULL);

#ifdef GFX_DEBUG
    if(context == NULL || rect == NULL)
        return GFX_FAILURE;
#endif

    return context->hal.drawClipRectGet(rect);
}

#endif // GFX_DRAW_PIPELINE_ENABLED