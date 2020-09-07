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
    gfx_layer.h

  Summary:
    Defines the graphics layer construct

  Description:
    Layer and buffer management.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_LAYER_H
#define GFX_LAYER_H
//DOM-IGNORE-END

#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_pixel_buffer.h"
#include "gfx/hal/inc/gfx_rect.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    GFX_FrameBuffer_t

  Summary:
    A frame buffer is a wrapper around a pixel buffer construct that is used by
    display drivers to manage frame buffers.

  Description:
    pb - The pixel buffer description of the frame buffer.
    state - The state of the frame buffer.
    coherent - Indicates if the frame buffer is allocated from coherent dynamic
               memory

  Remarks:
    None.
*/
typedef struct GFX_FrameBuffer_t
{
    GFX_PixelBuffer pb;

    GFX_BufferState state;
    GFX_Bool coherent;

    void* driver_data;
} GFX_FrameBuffer;

// *****************************************************************************
/* Structure:
    GFX_Layer_t

  Summary:
    Layers describe basic display drawing areas and are meant to map directly to
    graphics controller hardware layers.

  Description:
    Graphics controllers will typically offer at least one drawing layer for
    drawing purposes.  More advanced controllers may offer several.  Layers are
    often configurable, offering independent positioning, sizing, color formats,
    and drawing features.

    uint32_t id - the unique id of the layer

    struct
    {
        GFX_Rect display - represents area in display space
        GFX_Rect local - represents position in local space
    } rect;

    uint32_t pixel_size - size of a layer pixel in bytes

    GFX_Bool alphaEnable - indicates if layer alpha blending is enabled
    uint32_t alphaAmount - indicates the amount of alpha blending

    GFX_Bool maskEnable - indicates if layer masking/transparency is enabled
    uint32_t maskColor - the color to mask

    uint32_t buffer_count - the number of buffers this layer owns
    uint32_t buffer_read_idx - the index of the current read buffer
    uint32_t buffer_write_idx - the index of the current write buffer
    GFX_FrameBuffer buffers[GFX_MAX_BUFFER_COUNT] - the layer buffer array

    GFX_Bool enabled - indicates if the layer is enabled
    GFX_Bool visible - indicates if the layer is visible

    GFX_Bool swap - indicates if the layer is waiting to advance its buffer
                    chain

    GFX_Bool locked - indicates if the layer's buffers are locked for
                      manipulation.  this is typically meant to prevent things
                      like swapping before drawing operations have been
                      completed

    GFX_Bool vsync - indicates if this layer should swap during the display
                     driver's blanking period.  if this is true then it is the
                     responsibility of the display driver to call
                     GFX_LayerSwap on this layer during vblank.  If this is false
                     then the layer will swap immediately upon application request.

    void* driver_data - this is a pointer that may be allocated by the display
                        driver to store driver-specific per layer data.  the
                        driver is responsible for the management of this pointer
                        during the application life cycle

  Remarks:
    None.
*/
typedef struct GFX_Layer_t
{
    uint32_t id;

    struct
    {
        // sizes are redunant between these rects but it makes for cleaner code
        // when calling functions that take GFX_Rect as arguments
        GFX_Rect display; // represents area in display space
        GFX_Rect local;   // represents position in local space
    } rect;

    uint32_t pixel_size;

    GFX_Bool alphaEnable;
    uint32_t alphaAmount;

    GFX_Bool maskEnable;
    uint32_t maskColor;

    uint32_t buffer_count;
    uint32_t buffer_read_idx;
    uint32_t buffer_write_idx;
    GFX_FrameBuffer buffers[GFX_MAX_BUFFER_COUNT];

    GFX_Bool enabled;
    GFX_Bool visible;

    GFX_Bool swap;
    uint32_t swapCount;
    GFX_Bool locked;
    GFX_Bool invalid;

    GFX_Bool vsync;

    void* driver_data;
} GFX_Layer;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    GFX_PixelBuffer* GFX_LayerReadBuffer(GFX_Layer* layer)

  Summary:
    Gets the pointer to the layer's current read pixel buffer.

  Parameters:
    GFX_Layer* - the pointer to the layer

  Returns:
    GFX_PixelBuffer* - the pointer to the read pixel buffer
*/
GFX_PixelBuffer* GFX_LayerReadBuffer(GFX_Layer* layer);

// *****************************************************************************
/* Function:
    GFX_PixelBuffer* GFX_LayerWriteBuffer(GFX_Layer* layer)

  Summary:
    Gets the pointer to the layer's current write pixel buffer.

  Parameters:
    GFX_Layer* - the pointer to the layer

  Returns:
    GFX_PixelBuffer* - the pointer to the write pixel buffer
*/
GFX_PixelBuffer* GFX_LayerWriteBuffer(GFX_Layer* layer);

// *****************************************************************************
/* Function:
    void GFX_LayerRotate(GFX_Layer* layer)

  Summary:
    Swaps the width and height dimensions of a layer.  Can be used for run-time
    display orientation

  Parameters:
    GFX_Layer* - the layer to operate on
*/
void GFX_LayerRotate(GFX_Layer* layer);

// *****************************************************************************
/* Function:
    void GFX_LayerSwap(GFX_Layer* layer)

  Summary:
    Performs a swap operation on the given layer.  This advances the pointers of
    layer's buffer chain.  The current write buffer becomes the new read buffer
    and a new buffer is chosen as the new write buffer.  Has no effect in
    single buffer environments.

  Parameters:
    GFX_Layer* - the layer to operate on
*/
void GFX_LayerSwap(GFX_Layer* layer);

// DOM-IGNORE-BEGIN
// internal use only
void _GFX_LayerSizeSet(GFX_Layer* layer, int32_t width, int32_t height);
// DOM-IGNORE-END


// *****************************************************************************
/* Function:
    void GFX_LayerFromOrientedSpace(GFX_Rect* displayRect,
                                    GFX_Layer* layer,
                                    GFX_Orientation ori,
                                    GFX_Bool mirrored)

  Summary:
    Transforms a layer oriented space to screen space.

  Parameters:
    GFX_Rect* dispayRect - the rectangle of the display
    GFX_Layert* layer - the layer
    GFX_Orientation - the orientation setting
    GFX_Bool - the mirroring setting

  Returns:
    void
*/
void GFX_LayerFromOrientedSpace(GFX_Rect* displayRect,
                                GFX_Layer* layer,
                                GFX_Orientation ori,
                                GFX_Bool mirrored);

// *****************************************************************************
/* Function:
    void GFX_LayerToOrientedSpace(GFX_Rect* displayRect,
                                  GFX_Layer* layer,
                                  GFX_Orientation ori,
                                  GFX_Bool mirrored)

  Summary:
    Transforms a layer from screen space to oriented space.

  Parameters:
    GFX_Rect* dispayRect - the rectangle of the display
    GFX_Rect* layer - the layer
    GFX_Orientation - the orientation setting
    GFX_Bool - the mirroring setting

  Returns:
    void
*/
void GFX_LayerToOrientedSpace(GFX_Rect* displayRect,
                              GFX_Layer* layer,
                              GFX_Orientation ori,
                              GFX_Bool mirrored);

// *****************************************************************************
/* Function:
    GFX_Point GFX_LayerPointFromOrientedSpace(const GFX_Layer* layer,
                                              const GFX_Point* point,
                                              GFX_Orientation ori,
                                              GFX_Bool mirrored)

  Summary:
    Transforms a point from oriented space to screen space given
    a layer, a display orientation, and a mirroring setting.

  Parameters:
    const GFX_Layer* layer - the layer
    const GFX_Point* point - the point
    GFX_Orientation - the orientation setting
    GFX_Bool - the mirroring setting

  Returns:
    GFX_Point
*/
LIB_EXPORT GFX_Point GFX_LayerPointFromOrientedSpace(const GFX_Layer* layer,
                                                     const GFX_Point* point,
                                                     GFX_Orientation ori,
                                                     GFX_Bool mirrored);

// *****************************************************************************
/* Function:
    GFX_Point GFX_LayerPointToOrientedSpace(const GFX_Rect* layerRect,
                                            const GFX_Point* point,
                                            GFX_Orientation ori,
                                            GFX_Bool mirrored)

  Summary:
    Transforms a point from screen space to oriented space given
    a layer, a display orientation, and a mirroring setting.

  Parameters:
    const GFX_Layer* layer - the layer
    const GFX_Point* point - the point to transform
    GFX_Orientation - the orientation setting
    GFX_Bool - the mirroring setting

  Returns:
    GFX_Point
*/
LIB_EXPORT GFX_Point GFX_LayerPointToOrientedSpace(const GFX_Layer* layer,
                                                   const GFX_Point* point,
                                                   GFX_Orientation ori,
                                                   GFX_Bool mirrored);

// *****************************************************************************
/* Function:
    GFX_Rect GFX_LayerRectFromOrientedSpace(const GFX_Layer* layer,
                                            const GFX_Rect* rect,
                                            GFX_Orientation ori,
                                            GFX_Bool mirrored)

  Summary:
    Transforms a layer point from oriented space to screen space given
    a layer, a display orientation, and a mirroring setting.

  Parameters:
    const GFX_Layer* layer - the layer
    const GFX_Rect* rect - the rectangle to transform
    GFX_Orientation - the orientation setting
    GFX_Bool - the mirroring setting

  Returns:
    GFX_Point
*/
LIB_EXPORT GFX_Rect GFX_LayerRectFromOrientedSpace(const GFX_Layer* layer,
                                                   const GFX_Rect* rect,
                                                   GFX_Orientation ori,
                                                   GFX_Bool mirrored);

// *****************************************************************************
/* Function:
    GFX_Rect GFX_LayeRectToOrientedSpace(const GFX_Rect* layerRect,
                                         const GFX_Rect* rect,
                                         GFX_Orientation ori,
                                         GFX_Bool mirrored)

  Summary:
    Transforms a rectangle from screen space to oriented space given
    a layer, a display orientation, and a mirroring setting.

  Parameters:
    const GFX_Layer* layer - the layer
    const GFX_Rect* rect - the rectangle to transform
    GFX_Orientation - the orientation setting
    GFX_Bool - the mirroring setting

  Returns:
    GFX_Point
*/
LIB_EXPORT GFX_Rect GFX_LayerRectToOrientedSpace(const GFX_Layer* layer,
                                                 const GFX_Rect* rect,
                                                 GFX_Orientation ori,
                                                 GFX_Bool mirrored);


#endif /* GFX_LAYER_H */