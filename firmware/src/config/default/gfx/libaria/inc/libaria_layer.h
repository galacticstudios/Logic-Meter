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
 Module for Microchip Graphics Library - Aria User Interface Library

  Company:
    Microchip Technology Inc.

  File Name:
    libaria_layer.h

  Summary:
    Aria layers map directly to layers provided by the Graphics Hardware
    Abstraction layer.  HAL layers map directly to hardware layers provided
    by graphics hardware.  UI layers are logical containers for widgets
    and provide many of the same features.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_LAYER_H
#define LIBARIA_LAYER_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_rectarray.h"

typedef struct laScreen_t laScreen;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laLayerBufferType_t

  Summary:
    Defines the type of a layer.  If the layer has an explicit address then
    Aria tries to set that through the HAL when the layer is being set up.

  Remarks:
    None.
*/
typedef enum laLayerBufferType_t
{
    LA_BUFFER_TYPE_AUTO,
    LA_BUFFER_TYPE_ADDRESS
} laLayerBufferType;

// *****************************************************************************
/* Structure:
    laLayerBuffer_t

  Summary:
    Structure to maintain the buffer type and track the buffer location for each layer

  Description:
    Structure to maintain the buffer type and track the buffer location for each layer

  Remarks:
    None.
*/
typedef struct laLayerBuffer_t
{
    laLayerBufferType type;
    void* address;
} laLayerBuffer;

// *****************************************************************************
/* Enumeration:
    laLayerFrameState

  Summary:
    Defines the frame state of a layer.  Certain actions must only be performed
    at the start of a new frame and other actions must wait until the end of
    the current frame.

  Remarks:
    None.
*/
typedef enum laLayerFrameState_t
{
    LA_LAYER_FRAME_READY,
    LA_LAYER_FRAME_PREFRAME,
    LA_LAYER_FRAME_IN_PROGRESS,
    LA_LAYER_FRAME_COMPLETE
} laLayerFrameState;

// *****************************************************************************
/* Structure:
    laLayer_t

  Summary:
    Primary definition of a layer.  Builds on base functions of a standard
    widget.  Should never have a direct parent.

  Remarks:
    None.
*/
typedef struct laLayer_t
{
    laWidget widget; // base widget
    laScreen* screen; // owning screen pointer



    laBool deleting; // flag indicating that no changes should be made
                     // to the layer because it is in the process of
                     // being deleted

    uint32_t bufferCount; // number of buffers in the layer
    laLayerBuffer buffers[GFX_MAX_BUFFER_COUNT]; // buffer array

    laBool alphaEnable; // layer-based alpha blending enable flag

    laBool maskEnable;  // layer-based color masking enable flag
    GFX_Color maskColor; // layer-based masking color value

    laBool vsync; // layer vsync flag

    laRectArray prevDamageRects; // previous damaged rectangle list
    laRectArray currentDamageRects; // queued damaged rectangle list
    laRectArray pendingDamageRects; // pending damaged rectangle list
                                    // these are rectangles added during
                                    // a frame in progress

    laRectArray scratchRectList; // used for rectangle culling phase
    laRectArray frameRectList;   // this of rects to draw for a frame
    //GFX_Rect currentDrawingRect; // the current damage rectangle
    uint32_t frameRectIdx;
    GFX_Rect clippedDrawingRect; // the current damage rectangle clipped
                                 // to the currently rendering widget

    laBool drawingPrev;          // indicates if the layer is currently
                                 // drawing from its previous rectangle
                                 // array

    laLayerFrameState frameState; // the current frame render state of the
                                  // layer

    uint32_t layerDrawCount;      // the number of times this layer has drawn

    uint32_t frameDrawCount;      // the number of widgets that have rendered
                                  // on this layer this frame

    GFX_Rect inputRect;             // layer input area
    laBool inputRectLocked;       // input area matches layer dimensions

    laBool allowInputPassThrough; // indicates that input events should
                                  // be propagated through the layer
                                  // node to left siblings

    uint32_t deltaTime;           // stores delta time for updates that happen
                                  // during rendering

} laLayer;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laLayer* laLayer_New()

  Summary:
    Constructor for a new layer

  Description:
    Constructor for a new layer, returns the layer object

  Parameters:
    void

  Returns:
    laLayer*

  Remarks:
    Allocates memory for a layer using the active context memory interface.
    Once added to a screen the it becomes the responsibility of the framework
    to free the memory.
*/
LIB_EXPORT laLayer* laLayer_New();

// *****************************************************************************
/* Function:
    void laLayer_Delete(laLayer* layer)

  Summary:
    Destructor for the layer object

  Description:
    Destructor for the layer object

  Parameters:
    laLayer*

  Returns:
    void

  Remarks:

*/
LIB_EXPORT void laLayer_Delete(laLayer* layer);

// DOM-IGNORE-BEGIN
// internal functions
LIB_EXPORT void _laLayer_Moved(laLayer* layer);
LIB_EXPORT void _laLayer_Resized(laLayer* layer);
// DOM-IGNORE-END

// *****************************************************************************
/* Function:
    laBool laLayer_GetEnabled(const laLayer* layer)

  Summary:
    Returns the boolean value of the layer enabled property

  Description:
    Returns the boolean value of the layer enabled property

  Parameters:
    laLayer* - the layer


  Returns:
    laBool - the flag value

  Remarks:

*/
LIB_EXPORT laBool laLayer_GetEnabled(const laLayer* layer);

// *****************************************************************************
/* Function:
    laResult laLayer_SetEnabled(laLayer* widget, laBool enable)

  Summary:
    Sets the boolean value of the layer enabled property

  Description:
    Sets the boolean value of the layer enabled property

  Parameters:
    laLayer* - the layer
    laBool - the desired enabled value

  Returns:
    laResult - the result of the operation

  Remarks:
    The enabled flag for a layer will often control the hardware setting for
    layer usage, depending on the display driver
*/
LIB_EXPORT laResult laLayer_SetEnabled(laLayer* widget, laBool enable);

// *****************************************************************************
/* Function:
    laBool laLayer_GetAlphaEnable(const laLayer* layer)

  Summary:
    Gets the layer alpha enable flag

  Parameters:
    const laLayer* - the layer

  Returns:
    laBool - the value of the alpha enable flag
*/
LIB_EXPORT laBool laLayer_GetAlphaEnable(const laLayer* layer);

// *****************************************************************************
/* Function:
    laResult laLayer_SetAlphaEnable(laLayer* layer, laBool enable)

  Summary:
    Sets the layer alpha enable flag to the specified value

  Parameters:
    laLayer* layer - the layer
    laBool enable - the desired value of the flag

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetAlphaEnable(laLayer* layer, laBool enable);

// *****************************************************************************
/* Function:
    uint32_t laLayer_GetAlphaAmount(const laLayer* layer)

  Summary:
    Get's the amount of alpha blending for a given layer

  Parameters:
    laLayer* layer - the layer

  Returns:
    uint32_t - an alpha channel value from 0 - 255
*/
LIB_EXPORT uint32_t laLayer_GetAlphaAmount(const laLayer* layer);

// *****************************************************************************
/* Function:
    laResult laLayer_SetAlphaAmount(laLayer* layer, uint32_t amount)

  Summary:
    Set's the amount of alpha blending for a given layer

  Description:
    Set's the amount of alpha blending for a given layer

  Parameters:
    laLayer* layer - the layer
    uint32_t amount - an alpha amount from 0 - 255

  Returns:
    laResult - success if the operation succeeded

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetAlphaAmount(laLayer* layer, uint32_t amount);

// *****************************************************************************
/* Function:
    laBool laLayer_GetMaskEnable(const laLayer* layer)

  Summary:
    Gets the layer mask enable flag

  Description:
    Gets the layer mask enable flag

  Parameters:
    laLayer* layer - the layer

  Returns:
    laBool - the value of the mask enable flag

  Remarks:

*/
LIB_EXPORT laBool laLayer_GetMaskEnable(const laLayer* layer);

// *****************************************************************************
/* Function:
    laResult laLayer_SetMaskEnable(laLayer* layer, laBool enable)

  Summary:
    Sets the layer mask enable flag to the specified value

  Parameters:
    laLayer* layer - the layer
    laBool enable - the desired value of the flag

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetMaskEnable(laLayer* layer, laBool enable);

// *****************************************************************************
/* Function:
    GFX_Color laLayer_GetMaskColor(const laLayer* layer)

  Summary:
    Returns the mask color value for the current layer

  Description:
    Returns the mask color value for the current layer

  Parameters:
    laLayer* layer - the layer

  Returns:
    GFX_Color - the layer mask color value

  Remarks:

*/
LIB_EXPORT GFX_Color laLayer_GetMaskColor(const laLayer* layer);

// *****************************************************************************
/* Function:
    void laLayer_SetMaskColor(laLayer* layer, GFX_Color color)

  Summary:
    Set the mask color value for the current layer to the specified value

  Description:
    Set the mask color value for the current layer to the specified value

  Parameters:
    laLayer* layer - the layer
    GFX_color color - the desired mask color value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetMaskColor(laLayer* layer, GFX_Color color);

// *****************************************************************************
/* Function:
    uint32_t laLayer_GetBufferCount(const laLayer* layer)

  Summary:
    Return the buffer count for the current layer

  Description:
    Return the buffer count for the current layer

  Parameters:
    laLayer* layer - the layer

  Returns:
    uint32_t - the current number of buffers for the layer

  Remarks:

*/
LIB_EXPORT uint32_t laLayer_GetBufferCount(const laLayer* layer);

// *****************************************************************************
/* Function:
    laResult laLayer_SetBufferCount(laLayer* layer, uint32_t count)

  Summary:
    Set the buffer count for the current layer to the specified value

  Description:
    Set the buffer count for the current layer to the specified value

  Parameters:
    laLayer* layer - the layer
    uint32_t count - the desired number of buffers

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetBufferCount(laLayer* layer, uint32_t count);

// *****************************************************************************
/* Function:
    laBool laLayer_GetVSync(const laLayer* layer)

  Summary:
    Gets the layer's vsync flag setting

  Description:


  Parameters:
    const laLayer* layer - the layer

  Returns:
    laBool - the state of the layer's vsync flag

  Remarks:

*/
LIB_EXPORT laBool laLayer_GetVSync(const laLayer* layer);

// *****************************************************************************
/* Function:
    void laLayer_SetVSync(laLayer* layer, laBool enable)

  Summary:
    Sets the layer's vsync flag.

  Description:
    Sets the layer's vsync flag.

  Parameters:
    const laLayer* layer - the layer

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetVSync(laLayer* layer, laBool enable);

// *****************************************************************************
/* Function:
    GFX_Rect laLayer_GetInputRect(const laLayer* layer)

  Summary:
    Gets the layer's input rectangle.

  Description:
    Gets the layer's input rectangle.

  Parameters:
    const laLayer* layer - the layer

  Returns:
    GFX_Rect - the input rectangle

  Remarks:

*/
LIB_EXPORT GFX_Rect laLayer_GetInputRect(const laLayer* layer);

// *****************************************************************************
/* Function:
    laResult laLayer_SetInputRect(laLayer* layer, laBool enable)

  Summary:
    Sets the layer's input rect dimensions.

  Description:
    Sets the layer's input rect dimensions.  This rectangle controls the input
    area of the layer.  Some use cases may require a layer to accept input even
    if the input is outside of the physical dimensions of the layer.  One example
    is a touch glass that is larger than the size of a display.  Widgets may need
    to be placed in this invisible external area and still be capable of receiving
    input events.

  Parameters:
    const laLayer* layer - the layer
    int32_t x - the x position of the rectangle
    int32_t y - the y position of the rectangle
    int32_t width - the width of the rectangle
    int32_t height - the height of the rectangle

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetInputRect(laLayer* layer,
                                         int32_t x,
                                         int32_t y,
                                         int32_t width,
                                         int32_t height);

// *****************************************************************************
/* Function:
    laBool laLayer_GetInputRectLocked(const laLayer* layer)

  Summary:
    Gets the layer's input rect locked flag

  Description:
    This flag controls whether the layer input rectangle is locked to
    match the size of the layer's actual dimensions.

  Parameters:
    const laLayer* layer - the layer

  Returns:
    laBool - the state of the layer's input rect locked flag

  Remarks:

*/
LIB_EXPORT laBool laLayer_GetInputRectLocked(const laLayer* layer);

// *****************************************************************************
/* Function:
    laResult laLayer_SetInputRectLocked(laLayer* layer, laBool enable)

  Summary:
    Sets the layer's input rect locked flag.

  Description:
    Sets the layer's input rect locked flag.  This flag controls whether the
    input rectangle is locked to match the size of the layer's actual dimensions.
    When enabled, any change to the layer's size will be propagated to the input
    area as well.  The default value is true.

  Parameters:
    const laLayer* layer - the layer

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetInputRectLocked(laLayer* layer, laBool locked);

// *****************************************************************************
/* Function:
    laBool laLayer_GetAllowInputPassThrough(const laLayer* layer)

  Summary:
    Gets the layer's input passthrough setting

  Description:
    The input passthrough setting is used to prohibit or allow input events
    to pass through a layer.  If a layer is opaque or semi-opaque input events
    should probably not be allowed to pass through.  If the layer is completely
    transparent then input events may be allowed to pass through to interact
    with widgets on layers further back in the hierarchy.

    An application that disables this is responsible for ensuring that it
    is modified when the dimensions of the layer change.

  Parameters:
    const laLayer* layer - the layer

  Returns:
    laBool - the state of the layer's passthrough flag

  Remarks:

*/
LIB_EXPORT laBool laLayer_GetAllowInputPassThrough(const laLayer* layer);

// *****************************************************************************
/* Function:
    laResult laLayer_SetAllowInputPassthrough(laLayer* layer, laBool enable)

  Summary:
    Sets the layer's input passthrough flag.

  Description:
    Sets the layer's input passthrough flag.

  Parameters:
    const laLayer* layer - the layer

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_SetAllowInputPassthrough(laLayer* layer, laBool enable);

// *****************************************************************************
/* Function:
    laResult laLayer_AddDamageRect(laLayer* layer, const GFX_Rect* rect)

   Summary:
    Adds a damaged rectangle to the list.  Damage rectangles are used in minimal
    redraw algorithms.

   Precondition:

   Parameters:
     laLayer* layer - the layer
     const GFX_Rect* rect - the rectangle

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laLayer_AddDamageRect(laLayer* layer,
                                          const GFX_Rect* rect,
                                          laBool noCombine);

// *****************************************************************************
/* Function:
    laBool laLayer_IsDrawing(laLayer* layer)

   Summary:
    Queries a layer to find out if it is currently drawing a frame.

   Precondition:

   Parameters:
     laLayer* layer - the layer

  Returns:
    laBool - the result of the operation

  Remarks:

*/
LIB_EXPORT laBool laLayer_IsDrawing(laLayer* layer);

// DOM-IGNORE-BEGIN
// these are for internal use only
void _laLayer_Reset(laLayer* layer);

laBool _laLayer_NeedsDraw(laLayer* layer, GFX_Rect* rect);
void _laLayer_Preframe(laLayer* layer);
void _laLayer_Postframe(laLayer* layer);
void _laLayer_FinishFrame(laLayer* layer);

void _laLayer_InvalidateChildren(laLayer* layer);

void _laLayer_Paint(laLayer* layer);

void _laLayer_RedrawPartial(laLayer* layer);
void _laLayer_SwapLists(laLayer* layer);

void _laLayer_AddDamageRectToList(laRectArray* arr,
                                  const GFX_Rect* rect,
                                  laBool noCombine);

void _laLayer_MergeRectLists(laLayer* layer);
// DOM-IGNORE-END

#endif /* LIBARIA_LAYER_H */