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

/*******************************************************************************
 Module for Microchip Graphics Library - Hardware Abstraction Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx.h

  Summary:
    Main include file for the MPLAB Harmony Graphics Hardware Abstraction
    Layer

  Description:
    
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef GFX_DRIVER_H
#define GFX_DRIVER_H

#include "gfx/hal/inc/gfx_color.h"
#include "gfx/hal/inc/gfx_display.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_pixel_buffer.h"
#include "gfx/hal/inc/gfx_rect.h"

#if GFX_DRAW_PIPELINE_ENABLED
#include "gfx/hal/inc/gfx_draw.h"
#endif

typedef struct GFX_Context_t GFX_Context;

// *****************************************************************************
/* Function:
    GFX_Result GFX_Initialize(void);

  Summary:
    Initializes the hardware abstraction layer global state.  Should be called 
    before any other HAL function.   
*/
LIB_EXPORT GFX_Result GFX_Initialize(void);

// *****************************************************************************
/* Function:
    GFX_Result GFX_Initialize(void);

  Summary:
    Creates a hardware abstraction layer context instance.
    
  Parameters:
    GFX_Driver - integer index of the driver to use for this context
    GFX_Display - integer index of the display to use for this context
    GFX_Processor - integer index of the accelerator to use for this context
    GFX_MemoryIntf* - pointer to a custom memory interface definition or NULL.  
                      If NULL, standard library memory interfaces will be used
  
  Returns:
    GFX_Handle - A pointer to the created context or NULL;
*/
LIB_EXPORT GFX_Handle GFX_Open(GFX_Driver driver,
                               GFX_Display display,
                               GFX_Processor processor,
                               GFX_MemoryIntf* intf);
                               
// *****************************************************************************
/* Function:
    void GFX_Close();

  Summary:
    Closes the currently active hardware abstraction layer context.
*/                                                             
LIB_EXPORT void GFX_Close();

// *****************************************************************************
/* Function:
    void GFX_Update();

  Summary:
    Updates the currently active hardware abstraction layer context.  Typically
    calls an update loop the display driver.
*/     
LIB_EXPORT GFX_Result GFX_Update();

// *****************************************************************************
/* Function:
    GFX_Result GFX_Begin();

  Summary:
    Indicates that the application wants to issue draw calls to the frame buffer.
    
  Description:
    This function attempts to lock the frame buffer for modification.  The display
    driver may not allow updates for various reason such as a pending frame buffer
    swap action.

  Returns:
    GFX_Result - Returns GFX_TRUE if the context is ready to draw, otherwise
                 returns GFX_FALSE;
*/   
LIB_EXPORT GFX_Result GFX_Begin();

// *****************************************************************************
/* Function:
    GFX_Result GFX_End();

  Summary:
    Indicates that the application has finished making updates to the frame
    buffer.
*/ 
LIB_EXPORT void GFX_End();

// *****************************************************************************
/* Function:
    GFX_Result GFX_Get(GFX_Flag flag, ...);

  Summary:
    Abstract state data retreival interface.
    
  Description:
    This function is a generic data 'getter' function.  Its first argument is
    always a flag from the 'GFX_Flag's table.  The flag indicates the
    desired data to retrieve.  Flags will often require additional arguments
    to be passed by reference to hold the returned data.
    
    Below is a table describing the function of each flag and its 
    associated arguments:
    
    GFX_Get(GFXF_DISPLAY_COUNT, uint32_t*);
    Gets the number of displays supported

    GFX_Get(GFXF_DISPLAY_INFO, GFX_DisplayInfo*);
    Gets a copy of the display information struct for a display index.

    GFX_Get(GFXF_DRIVER_COUNT, uint32_t*);
    Gets the number of drivers supported by the HAL.

    GFX_Get(GFXF_DRIVER_INFO, GFX_DriverInfo*);
    Gets a copy of the driver information struct for a driver index.

    GFX_Get(GFXF_BRIGHTNESS_RANGE, uint32_t*, uint32_t*);
    Gets the valid range of brightness values supported by a display.

    GFX_Get(GFXF_BRIGHTNESS, uint32_t*);
    Gets the current display brightness setting.

    GFX_Get(GFXF_VSYNC_CALLBACK, GFX_SyncCallback_FnPtr*);
    Gets the function pointer that a driver may call to indicate that it has 
    entered VSYNC state.

    GFX_Get(GFXF_HSYNC_CALLBACK, GFX_SyncCallback_FnPtr*);
    Gets the function pointer that a driver may call to indicate that it has 
    entered HSYNC state.

    GFX_Get(GFXF_ORIENTATION, GFX_Orientation*);
    Gets the orientation mode for a given context.  Raster operation points are
    orthogonally rotated by a right angle multiple in the display space.

    GFX_Get(GFXF_MIRRORED, GFX_Bool*);
    Gets mirroring mode for a given context.  Raster operation points are
    mirrored to the opposite X coordinate in relative space.

    GFX_Get(GFXF_COLOR_MODE, GFX_COLOR_MODE*);
    Gets the currently set global context color mode.
    
    GFX_Get(GFXF_GLOBAL_PALETTE, GFX_GlobalPalette*);
    Gets the currently set global color palette pointer.

    GFX_Get(GFXF_LAYER_COUNT, uint32_t*);
    Gets the number of available driver layers.
    
    GFX_Get(GFXF_LAYER_ACTIVE, uint32_t*);
    Gets the active layer index.

    GFX_Get(GFXF_LAYER_ENABLED, GFX_Bool*);
    Gets the active layer enabled state.

    GFX_Get(GFXF_LAYER_VISIBLE, GFX_Bool*);
    Gets the active layer visible state.
    
    GFX_Get(GFXF_LAYER_VSYNC, GFX_Bool*);
    Gets the vsync swap mode of the active layer.  Only relevant for 
    multi-buffer layers.
    
    GFX_Get(GFXF_LAYER_INVALID, GFX_Bool*);
    Gets the active layer invalid state.
    
    GFX_Get(GFXF_LAYER_SWAP_SYNC, GFX_Bool*);
    Gets the layer swap sync mode for a given context.  Layer swap 
    syncing ensures that all dirty layers will swap at the same
    time rather than individually as needed.
    
    GFX_Get(GFXF_LAYER_SWAP, GFX_Bool*);
    Gets the swap flag for the active layer.

    GFX_Get(GFXF_LAYER_POSITION, uint32_t*, uint32_t*);
    Gets the X and Y coordinate of the active layer.  Arguments are x and y.

    GFX_Get(GFXF_LAYER_SIZE, uint32_t*, uint32_t*);
    Gets the width and height of the active layer. Arguments are width
    and height.

    GFX_Get(GFXF_LAYER_ALPHA_ENABLE, GFX_Bool*);
    Gets the layer-based alpha blending state for the active layer.

    GFX_Get(GFXF_LAYER_ALPHA_AMOUNT, uint32_t*);
    Gets the layer-based alpha blending amount of the active layer.

    GFX_Get(GFXF_LAYER_MASK_ENABLE, GFX_Bool*);
    Gets the layer-based masking state for the active layer.

    GFX_Get(GFXF_LAYER_MASK_COLOR, GFX_Color*);
    Gets the layer-based masking color for the active layer.

    GFX_Get(GFXF_LAYER_BUFFER_COUNT, uint32_t*);
    Gets the buffer count for the active layer.

    GFX_Get(GFXF_LAYER_BUFFER_ADDRESS, GFX_BUFFER*);
    Gets the pixel data address for a buffer of the active layer.

    GFX_Get(GFXF_LAYER_BUFFER_COHERENT, GFX_Bool*);
    Gets the coherent allocation flag for a buffer of the active layer.

    GFX_Get(GFXF_DRAW_PIPELINE_MODE, GFX_PipelineMode*);
    Gets the draw pipeline mode for the active context.
    
    GFX_Get(GFXF_DRAW_MODE, GFX_DrawMode*);
    Gets the draw mode for the active context.

    GFX_Get(GFXF_DRAW_COLOR, GFX_Color*);
    Gets the draw color for the active context.

    GFX_Get(GFXF_DRAW_GRADIENT_COLOR, GFX_Color*, GFX_Color*, GFX_Color*, GFX_Color*);
    Gets the gradient draw colors for the active context.

    GFX_Get(GFXF_DRAW_PALETTE, GFX_PixelBuffer*);
    Gets the reference palette to be used when performing indexed blit
    operations.
    
    GFX_Get(GFXF_DRAW_TARGET, GFX_PixelBuffer*);
    Gets the currently set scratch buffer draw target.

    GFX_Get(GFXF_DRAW_THICKNESS, uint32_t*);
    Gets the draw hint for thickness.

    GFX_Get(GFXF_DRAW_BLEND_MODE, GFX_BlendMode*);
    Gets the draw alpha blending mode for the context.
    
    GFX_Get(GFXF_DRAW_RESIZE_MODE, GFX_ResizeMode*);
    Gets the resize mode for stretch blitting operations.

    GFX_Get(GFXF_DRAW_ALPHA_VALUE, uint32_t*);
    Gets the global per-pixel alpha blending amount for the context.

    GFX_Get(GFXF_DRAW_MASK_ENABLE, GFX_Bool*);
    Gets the per-pixel color masking enabled flag.

    GFX_Get(GFXF_DRAW_MASK_VALUE, GFX_Color*);
    Gets the per-pixel color mask value. 

    GFX_Get(GFXF_DRAW_CLIP_ENABLE, GFX_Bool*);
    Gets the clipping enabled flag for the context.  

    GFX_Get(GFXF_DRAW_CLIP_RECT, GFX_Rect*);
    Gets the clip rectangle for the context.

  Returns:
    GFX_Result - Returns GFX_TRUE if the data was retrieved successfully.
                 Otherwise returns GFX_FALSE.
*/ 
LIB_EXPORT GFX_Result GFX_Get(GFX_Flag flag, ...);




// *****************************************************************************
/* Function:
    GFX_Result GFX_Set(GFX_Flag flag, ...);

  Summary:
    Abstract state data modification interface.
    
  Description:
    This function is a generic data 'setter' function.  Its first argument is
    always a flag from the 'GFX_Flag's table.  The flag indicates the
    desired operation to perform.  Operations will often require additional
    arguments to be passed to the function.
    
    Below is a table describing the function of each flag and its 
    associated arguments:
    
    GFX_Set(GFXF_BRIGHTNESS, uint32_t);
    Sets the current display brightness setting.  Must be in the range provided
    by GFXF_BRIGHTNESS_RANGE.

    GFX_Set(GFXF_VSYNC_CALLBACK, GFX_SyncCallback_FnPtr);
    sets the function pointer that a driver may call to indicate that it has
    entered VSYNC state.

    GFX_Set(GFXF_HSYNC_CALLBACK, GFX_SyncCallback_FnPtr);
    Sets the function pointer that a driver may call to indicate that it has
    entered HSYNC state.

    GFX_Set(GFXF_ORIENTATION, GFX_Orientation);
    Sets the orientation mode for a given context.  Raster operation points
    are orthogonally rotated by a right angle multiple in the display space.

    GFX_Set(GFXF_MIRRORED, GFX_Bool);
    Sets the mirroring mode for a given context.  Raster operation points
    are mirrored to the opposite X coordinate in relative space.
    
    GFX_Set(GFXF_COLOR_MODE, GFX_COLOR_MODE);
    Sets the global context color mode.  This operation may trigger a
    reallocation of all existing layer buffers to properly contain
    the new pixel sizes.
    
    GFX_Set(GFXF_GLOBAL_PALETTE, GFX_GlobalPalette);
    Sets the global palette pointer for the context.  Drivers can use this
    lookup table to upscale colors to a higher bit depth while taking
    advantage of lower frame buffer sizes.  Should be used in conjunction
    with an 8-bit color mode.

    GFX_Set(GFXF_LAYER_ACTIVE, uint32_t);
    Sets the active layer index.  All subsequent layer operations will be
    performed on the active layer.

    GFX_Set(GFXF_LAYER_ENABLED, GFX_Bool);
    Sets the active layer enabled state.  This might mean that a hardware
    layer needs to be enabled by the graphics controller.  Drivers may
    need this to manage hardware features.

    GFX_Set(GFXF_LAYER_VISIBLE, GFX_Bool);
    Sets the active layer visible state.  This might mean that a hardware
    layer needs to be set to visible by the graphics controller.  Drivers
    may need this to manage hardware features.
    
    GFX_Set(GFXF_LAYER_INVALID, GFX_Bool);
    Sets the active layer invalid state.  This indicates that there are
    pending changes to the layer that will happen in the future.

    GFX_Set(GFXF_LAYER_VSYNC, GFX_Bool);
    Sets the vsync swap mode of the active layer.  Only relevant for 
    multi-buffer layers.  In this mode the driver becomes responsible for
    managing the layer swap chain and should perform layer swapping during
    its VSYNC period.  If this option is off, the layer swaps immediately
    upon request.
    
    GFX_Set(GFXF_LAYER_SWAP_SYNC, GFX_Bool);
    Sets the layer sync mode for a given context.  Layer syncing ensures
    that all dirty layers will swap at the same time rather than
    individually as needed.
    
    GFX_Set(GFXF_LAYER_SWAP);
    Indicates that the active layer should advance its swap chain to the
    next valid state.  This is typically done after performing raster
    operations.  If GFXF_LAYER_VSYNC is off then the swap happens
    immediately.  Otherwise it happens at the drivers discretion which
    is most likely during its next VSYNC period.

    GFX_Set(GFXF_LAYER_POSITION, uint32_t, uint32_t);
    Sets the X and Y coordinate of the active layer.  Arguments are x and y.

    GFX_Set(GFXF_LAYER_SIZE, uint32_t, uint32_t);
    Sets the width and height of the active layer. Arguments are width
    and height.  Changing the size of the layer may cause all frame buffers
    owned by that layer to be reallocated.

    GFX_Set(GFXF_LAYER_ALPHA_ENABLE, GFX_Bool);
    Sets the layer-based alpha blending state for the active layer. This is
    mainly a hardware feature and must be supported by the graphics driver.

    GFX_Set(GFXF_LAYER_ALPHA_AMOUNT, uint32_t);
    Sets the layer-based alpha blending amount of the active layer. This is
    mainly a hardware feature and must be supported by the graphics driver.

    GFX_Set(GFXF_LAYER_MASK_ENABLE, GFX_Bool);
    Sets the layer-based masking state for the active layer. This is mainly
    a hardware feature and must be supported by the graphics driver.

    GFX_Set(GFXF_LAYER_MASK_COLOR, GFX_Color);
    Sets the layer-based masking color for the active layer. This is mainly
    a hardware feature and must be supported by the graphics driver.

    GFX_Set(GFXF_LAYER_BUFFER_COUNT, uint32_t);
    Sets the buffer count for the active layer.  Drivers may restrict
    this value.

    GFX_Set(GFXF_LAYER_BUFFER_ADDRESS, GFX_BUFFER);
    Sets the pixel data address for a buffer of the active layer.

    GFX_Set(GFXF_LAYER_BUFFER_COHERENT, GFX_Bool);
    Sets the coherent allocation flag for a buffer of the active layer.
    Coherent buffers will be dynamically allocated from coherent memory
    instead of regular memory if able.

    GFX_Set(GFXF_LAYER_BUFFER_ALLOCATE, uint32_t);
    Indicates that a buffer of the active layer should attempt to
    dynamically allocate its pixel data buffer.  This will return failure
    if the driver has ownership of the buffer or it is already allocated.
    
    GFX_Set(GFXF_LAYER_BUFFER_FREE, uint32_t);
    Indicates that a buffer of the active layer should attempt to free its
    dynamically allocated pixel buffer.  This will return failure if the
    buffer has not been dynamically allocated, or the driver has ownership
    of the buffer.
    
    GFX_Set(GFXF_DRAW_PIPELINE_MODE, GFX_PipelineMode);
    Sets the draw pipeline mode for the active context.  This determines
    the draw path that will be used for draw operations.  The choices are
    are pure software path, a graphics controller/driver path, a graphics
    accelerator path, or a combined GCU/GPU path.  This is useful as a means
    to quickly turn off acceleration features for demonstration purporses, or
    for disabling custom graphics controller driver draw functions for draw
    operations on a scratch buffer.

    GFX_Set(GFXF_DRAW_MODE, GFX_DrawMode);
    Sets the draw mode for the active context.  This includes drawing shape
    outlines, drawing filled shapes, or drawing gradient filled shapes.

    GFX_Set(GFXF_DRAW_COLOR, GFX_Color);
    Gets the draw color for the active context.  All simple raster operations
    use this color when writing to the write buffer.  The color value is
    expected to be in the same color format as the context.  Mismatching
    will result in rendering errors.

    GFX_Set(GFXF_DRAW_GRADIENT_COLOR, GFX_Color, GFX_Color, GFX_Color, GFX_Color);
    Gets the gradient draw colors for the active context.  Gradient-based
    raster operations will use these colors for drawing.  The first two are
    used for liniear gradient modes and all four are used for bilinear
    operations.

    GFX_Set(GFXF_DRAW_PALETTE, GFX_PixelBuffer*);
    Sets the reference palette to be used when performing indexed blit
    operations.  Applications decoding palette-based images must use this
    to set the palette before performing the blit operation.
    
    GFX_Set(GFXF_DRAW_TARGET, GFX_PixelBuffer*);
    Indicates that the given pointer is to be used as the target for all
    subsequent draw operations.  This redirects all draw operations away
    from the active write frame buffer.  Passing NULL to this function
    will disable an active draw target and refocus draw operations to the
    active write frame buffer.  This may need to be used in conjunction with
    GFXF_PIPELINE_MODE in Software or GPU only mode.    

    GFX_Set(GFXF_DRAW_THICKNESS, uint32_t);
    Sets the draw hint for thickness.  Raster operation algorithms may
    reference this value if they support drawing thicker primitives.

    GFX_Set(GFXF_DRAW_BLEND_MODE, GFX_BlendMode);
    Gets the combined draw feature mask for the context.  This includes
    masking and alpha blending.
    
    GFX_Set(GFXF_DRAW_RESIZE_MODE, GFX_ResizeMode);
    Sets the resize mode that will be used by stretch blit operations.
    Nearest neighbor is a faster but less-accurate algorithm and Bilinear
    is slower but more accurate.

    GFX_Set(GFXF_DRAW_ALPHA_VALUE, uint32_t);
    Sets the global per-pixel alpha blending amount for the context.  This
    value acts as an alpha channel for every pixel drawn while enabled.
    Valid values are 0-255.

    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_Bool);
    Sets the per-pixel color masking (transparency) enabled flag.  This
    enables the color masking state of the per-pixel transformation
    pipeline.

    GFX_Set(GFXF_DRAW_MASK_VALUE, GFX_Color);
    Sets the per-pixel color mask value.  During raster operations, the
    source pixel color will be compared to this.  If it matches the pixel
    will be discarded.  This comparison happens before any blending is
    applied and before the pixel is converted to the destination format.

    GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_Bool);
    Sets the clipping enabled flag for the context.  

    GFX_Set(GFXF_DRAW_CLIP_RECT, GFX_Rect*);
    Any pixel that falls outside of this rectangle will be discarded.

  Returns:
    GFX_Result - Returns GFX_TRUE if the operation was processed
                 successfully. Otherwise returns GFX_FALSE.
*/ 
LIB_EXPORT GFX_Result GFX_Set(GFX_Flag flag, ...);

// *****************************************************************************
/* Function:
    GFX_Result GFX_HandleInterrupt(uint32_t id);

  Summary:
    A utility function to allow a driver to react to some external event.
    
  Parameters:
    id - the id of the interrupt that occurred
    
  Returns:
    GFX_Result - GFX_TRUE if the driver handled the interrupt as expected.
                 GFX_FALSE otherwise
*/
LIB_EXPORT GFX_Result GFX_HandleInterrupt(uint32_t id);

#if GFX_DRAW_PIPELINE_ENABLED
// *****************************************************************************
/* Function:
    GFX_Result GFX_ReadPixel(GFX_BufferSelection source,
                             int32_t x,
                             int32_t y,
                             GFX_Color* color);

  Summary:
    Reads a pixel from a framebuffer owned by the active layer.
    
  Parameters:
    GFX_BufferSelection source - the buffer to read from, either the read or
                                 write buffer of the active layer
    int32_t x - the x coordinate to read
    int32_t y - the y coordinate to read
    GFX_Color* color - a pointer to a color variablethat will hold the read
                       value
    
  Returns:
    GFX_Result - GFX_TRUE if the color was read correctly.  
                 GFX_FALSE otherwise
*/
LIB_EXPORT GFX_Result GFX_ReadPixel(GFX_BufferSelection source,
                                    int32_t x,
                                    int32_t y,
                                    GFX_Color* color);

// *****************************************************************************
/* Function:
    GFX_Result GFX_ReadPixelArray(GFX_BufferSelection source,
                                  const GFX_Rect* rect,
                                  GFX_PixelBuffer* result);

  Summary:
    Reads a pixel array from a framebuffer owned by the active layer.  The
    default version of this function will automatically allocate the memory
    necessary to hold the copy of the array.  If this function completes 
    successfully the caller must then own and manage the memory.  
    
  Parameters:
    GFX_BufferSelection source - the buffer to read from, either the read or
                                 write buffer of the active layer
    const GFX_Rect* rect - a pointer to a rectangle to use as the bounds for
                           the array read
    GFX_PixelBuffer* result - a pointer to a frame buffer that will contain
                              the result of the array read operation.
    
  Returns:
    GFX_Result - GFX_TRUE if the array was read in full correctly.  
                 GFX_FALSE otherwise
*/
LIB_EXPORT GFX_Result GFX_ReadPixelArray(GFX_BufferSelection source,
                                         const GFX_Rect* rect,
                                         GFX_PixelBuffer* result);
#endif

#endif /* GFX_DRIVER_H */
