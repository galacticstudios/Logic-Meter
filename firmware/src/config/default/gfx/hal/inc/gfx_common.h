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
    gfx_common.h

  Summary:
    Common definitions for MPLAB Harmony Graphics Hardware Abstraction Layer

  Description:
    Type definitions for common functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_COMMON_H
#define GFX_COMMON_H
//DOM-IGNORE-END

#include <stdlib.h>
#include <string.h>

/* include standard int types for windows */
#ifdef _WIN32
#include "native_common.h"

#include "gfx_native_config.h"

#else
#include <stdint.h>
#include <stddef.h>

#include "gfx/hal/gfx_config.h"
#endif

#ifdef GFX_ASSERT_ENABLE
#ifdef _WIN32
#include <assert.h>
#endif
#define GFX_ASSERT assert
#else
#define GFX_ASSERT(x) { }
#endif

#ifdef _WIN32
#define GFX_DEPRECATED
#else
#define GFX_DEPRECATED __attribute__ ((deprecated))
#endif

// DOM-IGNORE-BEGIN
#define GFX_MAX_BUFFER_COUNT     2

#define GFX_NULL     0

typedef int32_t      GFX_Result;
#define GFX_UNSUPPORTED -2
#define GFX_FAILURE     -1
#define GFX_SUCCESS      0

typedef uint32_t     GFX_Bool;
#define GFX_TRUE     1
#define GFX_FALSE    0

typedef uint32_t     GFX_Driver;
typedef uint32_t     GFX_Display;
typedef uint32_t     GFX_Processor;
typedef void*        GFX_Handle;
typedef void*        GFX_Buffer;
typedef uint32_t     GFX_Color;
typedef void*        GFX_GlobalPalette;

#define GFX_GLOBAL_PALETTE_SIZE 256

typedef struct GFX_Context_t GFX_Context;
// BA removed this because it caused C++ errors
//typedef enum GFX_ColorMode_t GFX_ColorMode;

typedef void (*GFX_SyncCallback_FnPtr)(void);
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Enumeration:
    GFX_HardwareMode_t

  Summary:
    Hardware draw path settings.

*/
typedef enum GFX_PipelineMode_t
{
    GFX_PIPELINE_SOFTWARE = 0,
    GFX_PIPELINE_GCU      = 1,
    GFX_PIPELINE_GPU      = 2,
    GFX_PIPELINE_GCUGPU   = 3
} GFX_PipelineMode;

#define GFX_PIPELINE_MODE_COUNT (GFX_PIPELINE_GCUGPU + 1)

// *****************************************************************************
/* Enumeration:
    GFX_Orientation_t

  Summary:
    Orthogonal orientation settings.

*/
typedef enum GFX_Orientation_t
{
    GFX_ORIENTATION_0   = 0x0,
    GFX_ORIENTATION_90,
    GFX_ORIENTATION_180,
    GFX_ORIENTATION_270
} GFX_Orientation;

// *****************************************************************************
/* Enumeration:
    GFX_BlendMode_t

  Summary:
    Blend mode masks
*/
typedef enum GFX_BlendMode_t
{
    GFX_BLEND_NONE     = 0x0,
    GFX_BLEND_CHANNEL  = 0x1,
    GFX_BLEND_GLOBAL   = 0x2,
    GFX_BLEND_ALL      = GFX_BLEND_CHANNEL | GFX_BLEND_GLOBAL
} GFX_BlendMode;

// *****************************************************************************
/* Enumeration:
    GFX_BufferState_t

  Summary:
    Frame buffer memory states

  Description:
    address - The buffer is set to a discrete address.  This could be an address
              located in DDR memory, a buffer allocated by the application,
              or some other location.
    
    malloc - The buffer has been dynamically allocated from some form of heap or
             memory manager.  These can be freed as desired.
             
    managed - The buffer is owned by the system and cannot be allocated or freed,
              it just is.  This is common in systems where the memory is owned
              by the graphics driver or the memory resides on the graphics
              controller.

  Remarks:
    None.
*/
typedef enum GFX_BufferState_t
{
    GFX_BS_NONE = 0x0,
    GFX_BS_ADDRESS,
    GFX_BS_MALLOC,
    GFX_BS_MANAGED
} GFX_BufferState;

// *****************************************************************************
/* Enumeration:
    GFX_BufferSelection_t

  Summary:
    Buffer selector used when querying layers for certain buffer states.

*/
typedef enum GFX_BufferSelection_t
{
    GFX_BUFFER_READ,
    GFX_BUFFER_WRITE
} GFX_BufferSelection;

// *****************************************************************************
/* Enumeration:
    GFX_Flag_t

  Summary:
    Hardware abstraction state interface flags.  See gfx.h for a comprehensive
    description.    
*/
typedef enum GFX_Flag_t
{
    GFXF_NONE = 0,
    GFXF_DISPLAY_COUNT,
    GFXF_DISPLAY_INFO,
    GFXF_DRIVER_COUNT,
    GFXF_DRIVER_INFO,
    GFXF_BRIGHTNESS_RANGE,
    GFXF_BRIGHTNESS,
    GFXF_VSYNC_CALLBACK,
    GFXF_HSYNC_CALLBACK,
    GFXF_ORIENTATION,
    GFXF_MIRRORED,
    GFXF_COLOR_MODE,
    GFXF_GLOBAL_PALETTE,
    GFXF_LAYER_COUNT,
    GFXF_LAYER_ACTIVE,
    GFXF_LAYER_ENABLED,
    GFXF_LAYER_VISIBLE,
    GFXF_LAYER_VSYNC,
    GFXF_LAYER_INVALID,
    GFXF_LAYER_SWAP_SYNC,
    GFXF_LAYER_SWAP,
    GFXF_LAYER_POSITION,
    GFXF_LAYER_SIZE,
    GFXF_LAYER_ALPHA_ENABLE,
    GFXF_LAYER_ALPHA_AMOUNT,
    GFXF_LAYER_MASK_ENABLE,
    GFXF_LAYER_MASK_COLOR,
    GFXF_LAYER_BUFFER_COUNT,
    GFXF_LAYER_BUFFER_ADDRESS,
    GFXF_LAYER_BUFFER_COHERENT,
    GFXF_LAYER_BUFFER_ALLOCATE,
    GFXF_LAYER_BUFFER_FREE,
#if GFX_DRAW_PIPELINE_ENABLED
    GFXF_DRAW_PIPELINE_MODE,
    GFXF_DRAW_MODE,
    GFXF_DRAW_COLOR,
    GFXF_DRAW_GRADIENT_COLOR,
    GFXF_DRAW_PALETTE,
    GFXF_DRAW_TARGET,
    GFXF_DRAW_THICKNESS,
    GFXF_DRAW_BLEND_MODE,
    GFXF_DRAW_RESIZE_MODE,
    GFXF_DRAW_ALPHA_ENABLE,
    GFXF_DRAW_ALPHA_VALUE,
    GFXF_DRAW_MASK_ENABLE,
    GFXF_DRAW_MASK_VALUE,
    GFXF_DRAW_CLIP_ENABLE,
    GFXF_DRAW_CLIP_RECT,
#endif
    GFXF_LAST_FLAG
} GFX_Flag;

#define GFX_NUM_FLAGS GFXF_LAST_FLAG

// *****************************************************************************
/* Structure:
    GFX_Point_t

  Summary:
    A two dimensional Cartesian point.
*/
typedef struct GFX_Point_t
{
    int32_t x;
    int32_t y;
} GFX_Point;

// *****************************************************************************
/* Structure:
    GFX_Size_t

  Summary:
    A two dimensional indication of size.  Values are signed but should never be
    negative.
*/
typedef struct GFX_Size_t
{
    int32_t width;
    int32_t height;
} GFX_Size;

// *****************************************************************************
/* Structure:
    GFX_Rect_t

  Summary:
    A rectangle definition.  
*/
typedef struct GFX_Rect_t
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} GFX_Rect;

/* memory abstraction */
// *****************************************************************************
/*  Function pointer

    Function:
        typedef void* (*GFX_Malloc_FnPtr)(size_t);

    Summary:
        Simple wrapper around the standard malloc function pointer.
        Used for redirecting memory allocation to other memory management
        systems.
*/
// *****************************************************************************
typedef void* (*GFX_Malloc_FnPtr)(size_t);
// *****************************************************************************
/*  Function pointer

    Function:
        typedef void* (*GFX_Calloc_FnPtr)(size_t, size_t);

    Summary:
        Simple wrapper around the standard calloc function pointer.
        Used for redirecting memory allocation to other memory management
        systems.
*/
// *****************************************************************************
typedef void* (*GFX_Calloc_FnPtr)(size_t, size_t);
// *****************************************************************************
/*  Function pointer

    Function:
        typedef void* (*GFX_Realloc_FnPtr)(void*, size_t);

    Summary:
        Simple wrapper around the standard realloc function pointer.
        Used for redirecting memory allocation to other memory management
        systems.
*/
// *****************************************************************************
typedef void* (*GFX_Realloc_FnPtr)(void*, size_t);
// *****************************************************************************
/*  Function pointer

    Function:
        typedef void  (*GFX_Free_FnPtr)(void*);

    Summary:
        Simple wrapper around the standard free function pointer.
        Used for redirecting memory free to other memory management
        systems.
*/
// *****************************************************************************
typedef void  (*GFX_Free_FnPtr)(void*);
// *****************************************************************************
/*  Function pointer

    Function:
        typedef void* (*GFX_Memset_FnPtr)(void*, int32_t, size_t);

    Summary:
        Simple wrapper around the standard memset function pointer.
        Used for redirecting memset to other memory management
        systems.
*/
// *****************************************************************************
typedef void* (*GFX_Memset_FnPtr)(void*, int32_t, size_t);
// *****************************************************************************
/*  Function pointer

    Function:
        typedef void* (*GFX_Memcpy_FnPtr)(void*, const void*, size_t);

    Summary:
        Simple wrapper around the standard memcpy function pointer.
        Used for redirecting memcpy to other memory management
        systems.
*/
// *****************************************************************************
typedef void* (*GFX_Memcpy_FnPtr)(void*, const void*, size_t);

// *****************************************************************************
/* Structure:
    GFX_MemoryIntf_t

  Summary:
    Custom memory manager interface.

  Description:
    Applications utilizing the hardware abstraction layer may want to implement
    or utilize memory managers other than the standard library.  This interface
    is the method for notifying the HAL of that manager.
    
    The application must create a GFX_MemoryIntf struct, populate it with the
    fuction pointers that point to the custom memory manager, and pass the
    struct in to GFX_Open when the HAL context is created.
    
    If no GFX_MemoryIntf is provided then the standard library memory management
    APIs will be used by default.

  Remarks:
    None.
*/
typedef struct GFX_MemoryIntf_t
{
    GFX_Malloc_FnPtr malloc;
    GFX_Malloc_FnPtr coherent_alloc;
    GFX_Calloc_FnPtr calloc;
    GFX_Realloc_FnPtr realloc;
    GFX_Free_FnPtr free;
    GFX_Free_FnPtr coherent_free;
    GFX_Memset_FnPtr memset;
    GFX_Memcpy_FnPtr memcpy;
} GFX_MemoryIntf;




#endif /* GFX_COMMON_H */
