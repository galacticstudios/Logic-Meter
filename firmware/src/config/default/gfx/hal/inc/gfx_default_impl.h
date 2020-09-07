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

// DOM-IGNORE-BEGIN

/*******************************************************************************
 Module for Microchip Graphics Library - Hardware Abstraction Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_default_impl.h

  Summary:
    Defines MPLAB Harmony Graphics Hardware Abstraction default software
    function implementations 

  Description:
    Drawing and display timing functions.
*******************************************************************************/

#ifndef GFX_DEFAULT_IMPL_H
#define GFX_DEFAULT_IMPL_H

#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_pixel_buffer.h"

// empty function, does nothing
GFX_Result defUnsupported(void);

// initializes a context pointer to all of the default 
// function pointers
GFX_Result contextDefInitialization(GFX_Context* context);

// performs a generic initialization of the context
GFX_Result defInitialize(GFX_Context* context);

// default begin and end
GFX_Result defBegin(void);
void defEnd(void);

// empty update function, always returns success
GFX_Result defUpdate(void);

// generic destroy function
void defDestroy(GFX_Context* context);

// brightness functions
GFX_Result defBrightnessRangeGet(uint32_t* low, uint32_t* high);

uint32_t   defBrightnessGet(void);
GFX_Result defBrightnessSet(uint32_t val);      

// default vsync and hsync callback handlers
GFX_SyncCallback_FnPtr defVSyncCallbackGet(void);
GFX_Result defVSyncCallbackSet(GFX_SyncCallback_FnPtr cb);                         

GFX_SyncCallback_FnPtr defHSyncCallbackGet(void);
GFX_Result defHSyncCallbackSet(GFX_SyncCallback_FnPtr cb);

// default orientation and mirroring
GFX_Orientation defLayerOrientationGet(void);
GFX_Result      defLayerOrientationSet(GFX_Orientation ori);

GFX_Bool   defLayerMirroringGet(void);
GFX_Result defLayerMirroringSet(GFX_Bool mirr);    

// default color mode functions
GFX_ColorMode defColorModeGet(void);
GFX_Result defColorModeSet(GFX_ColorMode mode);

// default global palette functions
GFX_GlobalPalette defGlobalPaletteGet(void);
GFX_Result defGlobalPaletteSet(GFX_GlobalPalette pal);

// default layer active handlers
uint32_t   defLayerActiveGet(void);
GFX_Result defLayerActiveSet(uint32_t idx);

// default layer visibility and enabling functions
GFX_Bool   defLayerEnabledGet(void);
GFX_Result defLayerEnabledSet(GFX_Bool val);

GFX_Bool   defLayerVisibleGet(void);
GFX_Result defLayerVisibleSet(GFX_Bool val);

// default layer position functions
GFX_Result defLayerPositionGet(int32_t* x, int32_t* y);
GFX_Result defLayerPositionSet(int32_t x, int32_t y);

// default layer size functions
GFX_Result defLayerSizeGet(int32_t* width, int32_t* height);
GFX_Result defLayerSizeSet(int32_t width, int32_t height);

// buffer count and memory management
uint32_t   defLayerBufferCountGet(void);
GFX_Result defLayerBufferCountSet(uint32_t count);

GFX_Buffer defLayerBufferAddressGet(uint32_t idx);
GFX_Result defLayerBufferAddressSet(uint32_t idx, GFX_Buffer address);

GFX_Bool defLayerBufferCoherentGet(uint32_t idx);
GFX_Result defLayerBufferCoherentSet(uint32_t idx, GFX_Bool coherent);

GFX_Result defLayerBufferAllocate(uint32_t idx);
GFX_Result defLayerBufferFree(uint32_t idx);

// default vsync option handler
GFX_Bool   defLayerVsyncGet(void);
GFX_Result defLayerVsyncSet(GFX_Bool vsync);

// default invalid option handler
GFX_Bool   defLayerInvalidGet(void);
GFX_Result defLayerInvalidSet(GFX_Bool invalid);

// default layer swap sync option handler
GFX_Bool   defLayerSwapSyncGet(void);
GFX_Result defLayerSwapSyncSet(GFX_Bool swapSync);

// default swap functions
GFX_Bool   defLayerSwapGet(void);
GFX_Result defLayerSwapSet(GFX_Bool val);

void defLayerSwapPending(GFX_Layer* layer);
void defLayerSwapped(GFX_Layer* layer);

// default layer blend and masking functions
GFX_Bool defLayerAlphaEnableGet(void);
GFX_Result defLayerAlphaEnableSet(GFX_Bool enable, GFX_Bool wait);

uint32_t defLayerAlphaAmountGet(void);
GFX_Result defLayerAlphaAmountSet(uint32_t amount, GFX_Bool wait);

GFX_Bool defLayerMaskEnableGet(void);
GFX_Result defLayerMaskEnableSet(GFX_Bool enable, GFX_Bool wait);

GFX_Color defLayerMaskColorGet(void);
GFX_Result defLayerMaskColorSet(GFX_Color color, GFX_Bool wait);

GFX_Point defOrientPoint(const GFX_Point* point,
                         const GFX_Rect* rect,
                         GFX_Orientation ori);
                              
GFX_Point defMirrorPoint(const GFX_Point* point,
                         const GFX_Rect* rect,
                         GFX_Orientation ori);                             
                         
GFX_Point defMirrorPointEmpty(const GFX_Point* point,
                              const GFX_Rect* rect,
                              GFX_Orientation ori);                       

// draw pipeline functions
#if GFX_DRAW_PIPELINE_ENABLED

GFX_PipelineMode defDrawPipelineModeGet(void);
GFX_Result defDrawPipelineModeSet(GFX_PipelineMode mode);

GFX_DrawMode defDrawModeGet(void);
GFX_Result defDrawModeSet(GFX_DrawMode mode);

GFX_Color  defDrawColorGet(void);
GFX_Result defDrawColorSet(GFX_Color color);

void defDrawGradientColorGet(GFX_Color* tl,
                             GFX_Color* tr,
                             GFX_Color* bl,
                             GFX_Color* br);
                                   
GFX_Result defDrawGradientColorSet(GFX_Color tl,
                                   GFX_Color tr,
                                   GFX_Color bl,
                                   GFX_Color br);

GFX_Result defDrawPaletteGet(GFX_PixelBuffer* pb);
GFX_Result defDrawPaletteSet(const GFX_PixelBuffer* pb);

GFX_Result defDrawTargetGet(GFX_PixelBuffer** pb);
GFX_Result defDrawGargetSet(const GFX_PixelBuffer* pb);

GFX_BlendMode defDrawBlendModeGet(void);
GFX_Result    defDrawBlendModeSet(GFX_BlendMode mode);

GFX_ResizeMode defDrawResizeModeGet(void);
GFX_Result     defDrawResizeModeSet(GFX_ResizeMode mode);

GFX_Bool   defDrawAlphaEnableGet(void);
GFX_Result defDrawAlphaEnableSet(GFX_Bool enable);

uint32_t   defDrawAlphaValueGet(void);
GFX_Result defDrawAlphaValueSet(uint32_t value);

uint32_t   defDrawMaskValueGet(void);
GFX_Result defDrawMaskValueSet(uint32_t value);

GFX_Bool defMaskColor(const GFX_DrawState* state,
                      GFX_Color color);
                        
GFX_Bool defMaskEmpty(const GFX_DrawState* state,
                      GFX_Color color);

uint32_t   defDrawThicknessGet(void);
GFX_Result defDrawThicknessSet(uint32_t value);

GFX_Bool   defDrawClipEnabledGet(void);
GFX_Result defDrawClipEnabledSet(GFX_Bool value);

GFX_Result defDrawClipRectGet(GFX_Rect* rect);
GFX_Result defDrawClipRectSet(const GFX_Rect* rect);

GFX_Color  defAlphaGetPoint(const GFX_PixelBuffer* buffer,
                            const GFX_Point* pnt,
                            const GFX_DrawState* state);
                            
GFX_Color  defAlphaEmptyPoint(const GFX_PixelBuffer* buffer,
                              const GFX_Point* pnt,
                              const GFX_DrawState* state);

GFX_Color  defAlphaChannelBlend(GFX_Color source,
                                GFX_Color dest,
                                GFX_ColorMode mode);
                              
GFX_Color  defGlobalAlphaBlend(GFX_Color source,
                               GFX_Color dest,
                               GFX_ColorMode mode);                             

GFX_Color  defEmptyAlphaBlend(GFX_Color source,
                              GFX_Color dest,
                              GFX_ColorMode mode);

GFX_Result defSwap(void);

GFX_Color  defPixelGet(const GFX_PixelBuffer* buffer,
                       const GFX_Point* pnt);
                       
GFX_Result defPixelArrayGet(GFX_BufferSelection source,
                            const GFX_Rect* rect,
                            GFX_PixelBuffer* result);

GFX_Result defPixelSet(const GFX_PixelBuffer* buffer,
                       const GFX_Point* pnt,
                       GFX_Color color);
                       
#endif // GFX_DRAW_PIPELINE_ENABLED
#endif /* GFX_DEFAULT_IMPL_H */

//DOM-IGNORE-END