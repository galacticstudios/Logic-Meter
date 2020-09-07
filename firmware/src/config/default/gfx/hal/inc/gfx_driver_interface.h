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
    gfx_draw_interface.h

  Summary:
    Defines MPLAB Harmony Graphics Hardware Abstraction Layer driver interface
    struct

  Description:
    Display driver information, internal use.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_DRVIER_IMPL_H
#define GFX_DRVIER_IMPL_H
//DOM-IGNORE-END

#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_color.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_util.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    GFX_DriverInfo_t

  Summary:
    A driver description structure.

  Description:
    name - a short human-readable name.
    color formats - a mask of supported color formats
    layer_count - number of layers supported by the driver

  Remarks:
    None.
*/
typedef struct GFX_DriverInfo_t
{
    char name[16];
    
    GFX_ColorMask color_formats;
    
    uint32_t layer_count;
} GFX_DriverInfo;

// DOM-IGNORE-BEGIN

/* gets capability information from the driver */
typedef GFX_Result (*GFX_DriverInfoGet_FnPtr)(GFX_DriverInfo*);

/* creates a context of the driver */
typedef GFX_Result (*GFX_DriverContextCreate_FnPtr)(GFX_Context* context);

typedef struct GFX_DriverIntf_t
{
    GFX_DriverInfoGet_FnPtr       infoGet;
    GFX_DriverContextCreate_FnPtr contextInitialize;
} GFX_DriverIntf;

extern GFX_DriverIntf GFX_DriverInterfaces[];

// this is typically created and called automatically by a code generator
// internal use only
GFX_Result      GFX_InitializeDriverList(void);
GFX_Result      GFX_DriverIntfInitialize(void);
GFX_Result      GFX_DriverIntfGet(GFX_Driver idx, GFX_DriverIntf* iface);
GFX_DriverIntf* GFX_DriverIntfPtrGet(GFX_Driver idx);

//DOM-IGNORE-END

#endif /* GFX_DRVIER_IMPL_H */