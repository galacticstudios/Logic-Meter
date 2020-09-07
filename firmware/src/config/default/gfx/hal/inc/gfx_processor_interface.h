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
    gfx_processor_interface.h

  Summary:
    Defines MPLAB Harmony Graphics Hardware Abstraction Layer graphics
    processor interface struct

  Description:
    Graphics processor information, internal use.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_PROCESSOR_IMPL_H
#define GFX_PROCESSOR_IMPL_H
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
    GFX_ProcessorInfo_t

  Summary:
    A processor description structure.

  Description:
    name - a short human-readable name.
    color formats - a mask of supported color formats

  Remarks:
    None.
*/
typedef struct GFX_ProcessorInfo_t
{
    char name[16];

    GFX_ColorMask color_formats;
} GFX_ProcessorInfo;

// DOM-IGNORE-BEGIN

/* gets capability information from the processor */
typedef GFX_Result (*GFX_ProcessorInfoGet_FnPtr)(GFX_ProcessorInfo*);

/* creates a context of the driver */
typedef GFX_Result (*GFX_ProcessorContextCreate_FnPtr)(GFX_Context* context);

typedef struct GFX_ProcessorIntf_t
{
    GFX_ProcessorInfoGet_FnPtr       infoGet;
    GFX_ProcessorContextCreate_FnPtr contextInitialize;
} GFX_ProcessorIntf;

extern GFX_ProcessorIntf GFX_ProcessorInterfaces[];

// this is typically created and called automatically by a code generator
// internal use only
GFX_Result         GFX_InitializeProcessorList(void);
GFX_Result         GFX_ProcessorIntfInitialize(void);
GFX_Result         GFX_ProcessorIntfGet(GFX_Processor idx, GFX_ProcessorIntf* iface);
GFX_ProcessorIntf* GFX_ProcessorIntfPtrGet(GFX_Processor idx);

// DOM-IGNORE-END

#endif /* GFX_PROCESSOR_IMPL_H */