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
    gfx_context.h

  Summary:
    Defines MPLAB Harmony Graphics Hardware Abstraction Layer Context

  Description:
    HAL context management functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_CONTEXT_H
#define GFX_CONTEXT_H
//DOM-IGNORE-END

#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_display.h"
#include "gfx/hal/inc/gfx_hal.h"
#include "gfx/hal/inc/gfx_rect.h"



// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************
typedef struct GFX_Layer_t GFX_Layer;
// *****************************************************************************
/* Stucture:
    GFX_Context_t

  Summary:
    An instance of the hardware abstraction layer.

  Description:
    The context is an instance of the hardware abstraction layer.  It is
    essentially the marriage between a graphics driver, a display description,
    and possibly a graphics processor.  It contains the data that describes
    the layout of the display, the buffers that store the display data,
    the current draw state, and the function map that controls everything.
    
  Members:
    display_idx - the display associated with this context
    display_info - a pointer to the information for the display for this
                   context
                   
    layer.count - the number of existing layers
    layer.active_idx - the index of the active layer
    layer.active - the pointer to the active layer
    layer.layers - the array of layers for this context
    
    brightness - the brightness setting for this context
    orientation - the orientation mode for this context
    mirrored - the mirror mode for this context
    
    colorMode - the color mode for this context, all buffers of all layers
                use this mode
    
    draw - the current draw state of this context
    
    vsyncCB - the callback to invoke when the driver enters vsync mode
    hsyncCB - the callback to invoke when the driver enters hsync mode
    
    hal - the function table for this context
    
    memory - the memory management interface for this context
    
    driver_data - a pointer that can be used for driver-specific data purposes

  Remarks:
    None.
*/
typedef struct GFX_Context_t
{
    GFX_Display display_idx;
    GFX_DisplayInfo* display_info;

    struct
    {
        uint32_t count;
        uint32_t active_idx;
        GFX_Layer* active;
        GFX_Layer* layers;
    } layer;

    uint32_t brightness;
    
    GFX_Orientation orientation;
    GFX_Bool mirrored;
    
    GFX_Bool layerSwapSync;
    
    GFX_ColorMode colorMode;
    GFX_GlobalPalette globalPalette;
    
#if GFX_DRAW_PIPELINE_ENABLED
    GFX_DrawState draw;
#endif // GFX_DRAW_PIPELINE_ENABLED
    
    GFX_SyncCallback_FnPtr vsyncCB;
    GFX_SyncCallback_FnPtr hsyncCB;

    GFX_HAL hal;
    
    GFX_MemoryIntf memory;
    
    void* driver_data;
} GFX_Context;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    GFX_Context* GFX_ActiveContext(void)

  Summary:
    Gets the current set active HAL context.

  Returns:
    GFX_Context* - the active context or NULL
*/
LIB_EXPORT GFX_Context* GFX_ActiveContext(void);

// *****************************************************************************
/* Function:
    void         GFX_ContextActiveSet(GFX_Context* const context)

  Summary:
    Sets the active context

  Parameters:
    GFX_Context* - the new active context or NULL
*/
void GFX_ContextActiveSet(GFX_Context* const context);

// DOM-IGNORE-BEGIN
// internal use only
void _GFX_DefaultSyncCallback(void);
// DOM-IGNORE-END

#endif /* GFX_CONTEXT_H */
