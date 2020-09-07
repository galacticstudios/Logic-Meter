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

#define GFX_GET_INTF(fn_name)         {&get_##fn_name,     &set_unimplemented}
#define GFX_SET_INTF(fn_name)         {&get_unimplemented, &set_##fn_name}  
#define GFX_INTF(fn_name)             {&get_##fn_name,     &set_##fn_name}

/* interface function pointer table - must match order of GFX_Flag table */
GFX_Interface_FnPtr GFX_InterfaceTable[GFX_NUM_FLAGS][2] =
{
    GFX_INTF(unimplemented),           // GFXF_NONE
    
    GFX_GET_INTF(displayCount),        // GFXF_DISPLAY_COUNT
    GFX_GET_INTF(displayInfo),         // GFXF_DISPLAY_INFO
    
    GFX_GET_INTF(driverCount),         // GFXF_DRIVER_COUNT
    GFX_GET_INTF(driverInfo),          // GFXF_DRIVER_INFO
    
    GFX_GET_INTF(brightnessRange),     // GFXF_BRIGHTNESS_RANGE
    GFX_INTF(brightness),              // GFXF_BRIGHTNESS
    
    GFX_INTF(vsyncCallback),           // GFXF_VSYNC_CALLBACK
    GFX_INTF(hsyncCallback),           // GFXF_HSYNC_CALLBACK
    
    GFX_INTF(orientation),             // GFXF_ORIENTATION
    GFX_INTF(mirroring),               // GFXF_MIRRORED
    GFX_INTF(colorMode),               // GFXF_COLOR_MODE
    GFX_INTF(globalPalette),           // GFXF_GLOBAL_PALETTE
    
    GFX_GET_INTF(layerCount),          // GFXF_LAYER_COUNT
    GFX_INTF(layerActive),             // GFXF_LAYER_ACTIVE
    GFX_INTF(layerEnabled),            // GFXF_LAYER_ENABLED
    GFX_INTF(layerVisible),            // GFXF_LAYER_VISIBLE
    GFX_INTF(layerVsync),              // GFXF_LAYER_VSYNC
    GFX_INTF(layerInvalid),            // GFXF_LAYER_INVALID
    GFX_INTF(layerSwapSync),           // GFXF_LAYER_SWAP_SYNC
    GFX_INTF(layerSwap),               // GFXF_LAYER_SWAP
    GFX_INTF(layerPosition),           // GFXF_LAYER_POSITION
    GFX_INTF(layerSize),               // GFXF_LAYER_SIZE
    GFX_INTF(layerAlphaEnable),        // GFXF_LAYER_ALPHA_ENABLE
    GFX_INTF(layerAlphaAmount),        // GFXF_LAYER_ALPHA_AMOUNT
    GFX_INTF(layerMaskEnable),         // GFXF_LAYER_MASK_ENABLE
    GFX_INTF(layerMaskColor),          // GFXF_LAYER_MASK_COLOR
    GFX_INTF(layerBufferCount),        // GFXF_LAYER_BUFFER_COUNT
    GFX_INTF(layerBufferAddress),      // GFXF_LAYER_BUFFER_ADDRESS
    GFX_INTF(layerBufferCoherent),     // GFXF_LAYER_BUFFER_COHERENT
    GFX_INTF(layerBufferAllocated),    // GFXF_LAYER_BUFFER_ALLOCATE
    GFX_SET_INTF(layerBufferFree),     // GFXF_LAYER_BUFFER_FREE
    
#if GFX_DRAW_PIPELINE_ENABLED
    
    GFX_INTF(drawPipelineMode),        // GFXF_DRAW_PIPELINE_MODE
    GFX_INTF(drawMode),                // GFXF_DRAW_MODE
    GFX_INTF(drawColor),               // GFXF_DRAW_COLOR
    GFX_INTF(drawGradientColor),       // GFXF_DRAW_GRADIENT_COLOR
    GFX_INTF(drawPalette),             // GFXF_DRAW_PALETTE
    GFX_INTF(drawTarget),              // GFXF_DRAW_TARGET
    GFX_INTF(drawThickness),           // GFXF_DRAW_THICKNESS
    GFX_INTF(drawBlendMode),           // GFXF_DRAW_BLEND_MODE,
    GFX_INTF(drawResizeMode),          // GFXF_DRAW_RESIZE_MODE,
    GFX_INTF(drawAlphaEnable),         // GFXF_DRAW_ALPHA_ENABLE
    GFX_INTF(drawAlphaValue),          // GFXF_DRAW_ALPHA_VALUE
    GFX_INTF(drawMaskEnable),          // GFXF_DRAW_MASK_ENABLED,
    GFX_INTF(drawMaskValue),           // GFXF_DRAW_MASK_VALUE
    GFX_INTF(drawClipEnable),          // GFXF_DRAW_CLIP_ENABLE
    GFX_INTF(drawClipRect),            // GFXF_DRAW_CLIP_RECT
#endif // GFX_DRAW_PIPELINE_ENABLED
};