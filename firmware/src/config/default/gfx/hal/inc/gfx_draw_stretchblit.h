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
    gfx_draw_stretchblit.h

  Summary:
    Stretched blit drawing functions.

  Description:
    Blit drawing functions, internal use.
*******************************************************************************/

#ifndef GFX_DRAW_STRETCHBLIT_H
#define GFX_DRAW_STRETCHBLIT_H

#include "gfx/hal/inc/gfx_common.h"

#if GFX_DRAW_PIPELINE_ENABLED

#include "gfx/hal/inc/gfx_pixel_buffer.h"

typedef struct GFX_DrawState_t GFX_DrawState;

// DOM-IGNORE-BEGIN
// internal use only
GFX_Result cpuDrawStretchBlit_NearestNeighbor(const GFX_PixelBuffer* buffer,
                                              const GFX_Rect* source_rect,
                                              const GFX_Rect* dest_rect,
                                              const GFX_DrawState* state);

GFX_Result cpuDrawStretchBlit_Bilinear(const GFX_PixelBuffer* buffer,
                                       const GFX_Rect* source_rect,
                                       const GFX_Rect* dest_rect,
                                       const GFX_DrawState* state);
// DOM-IGNORE-END

#endif // GFX_DRAW_PIPELINE_ENABLED
#endif /* GFX_DRAW_STRETCHBLIT_H */

//DOM-IGNORE-END