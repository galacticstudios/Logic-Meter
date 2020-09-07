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
    gfx_interface.h

  Summary:
    Defines the get and set interfaces for the main abstract interface API

  Description:
    Interface table generation.
*******************************************************************************/

#ifndef GFX_INTERFACE_H
#define GFX_INTERFACE_H

#include "gfx/hal/inc/gfx_common.h"

#include <stdarg.h>

#define GFX_GET 0
#define GFX_SET 1

#define GFX_INTERFACE(fn_name) \
GFX_Result get_##fn_name(va_list); \
GFX_Result set_##fn_name(va_list)

/* interfaces */
GFX_INTERFACE(unimplemented);
GFX_INTERFACE(displayCount);
GFX_INTERFACE(displayInfo);
GFX_INTERFACE(driverCount);
GFX_INTERFACE(driverInfo);
GFX_INTERFACE(brightnessRange);
GFX_INTERFACE(brightness);
GFX_INTERFACE(vsyncCallback);
GFX_INTERFACE(hsyncCallback);
GFX_INTERFACE(orientation);
GFX_INTERFACE(mirroring);
GFX_INTERFACE(colorMode);
GFX_INTERFACE(globalPalette);
GFX_INTERFACE(layerCount);
GFX_INTERFACE(layerActive);
GFX_INTERFACE(layerEnabled);
GFX_INTERFACE(layerVisible);
GFX_INTERFACE(layerVsync);
GFX_INTERFACE(layerInvalid);
GFX_INTERFACE(layerSwapSync);
GFX_INTERFACE(layerSwap);
GFX_INTERFACE(layerPosition);
GFX_INTERFACE(layerSize);
GFX_INTERFACE(layerAlphaEnable);
GFX_INTERFACE(layerAlphaAmount);
GFX_INTERFACE(layerMaskEnable);
GFX_INTERFACE(layerMaskColor);
GFX_INTERFACE(layerBufferCount);
GFX_INTERFACE(layerBufferAddress);
GFX_INTERFACE(layerBufferCoherent);
GFX_INTERFACE(layerBufferAllocated);
GFX_INTERFACE(layerBufferFree);

#if GFX_DRAW_PIPELINE_ENABLED

GFX_INTERFACE(drawPipelineMode);
GFX_INTERFACE(drawMode);
GFX_INTERFACE(drawColor);
GFX_INTERFACE(drawGradientColor);
GFX_INTERFACE(drawPalette);
GFX_INTERFACE(drawTarget);
GFX_INTERFACE(drawBlendMode);
GFX_INTERFACE(drawResizeMode);
GFX_INTERFACE(drawAlphaEnable);
GFX_INTERFACE(drawAlphaValue);
GFX_INTERFACE(drawMaskEnable);
GFX_INTERFACE(drawMaskValue);
GFX_INTERFACE(drawAntialias);
GFX_INTERFACE(drawThickness);
GFX_INTERFACE(drawClipEnable);
GFX_INTERFACE(drawClipRect);

#endif // GFX_DRAW_PIPELINE_ENABLED

typedef GFX_Result (*GFX_Interface_FnPtr)(va_list);

extern GFX_Interface_FnPtr GFX_InterfaceTable[GFX_NUM_FLAGS][2];

#endif /* GFX_INTERFACE_H */

//DOM-IGNORE-END