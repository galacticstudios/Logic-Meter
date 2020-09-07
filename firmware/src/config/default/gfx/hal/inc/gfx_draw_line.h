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
    gfx_draw_line.h

  Summary:
    Line and gradient functions.

  Description:
    Line and gradient drawing, internal use.
*******************************************************************************/

#ifndef GFX_DRAW_LINE_H
#define GFX_DRAW_LINE_H

#include "gfx/hal/inc/gfx_common.h"

#if GFX_DRAW_PIPELINE_ENABLED

typedef struct GFX_DrawState_t GFX_DrawState;

// internal use only
GFX_Result cpuDrawLine(const GFX_Point* p1,
                       const GFX_Point* p2,
                       const GFX_DrawState* state);

// internal use only                
GFX_Result cpuDrawLine_Normal(const GFX_Point* p1,
                              const GFX_Point* p2,
                              const GFX_DrawState* state);

// internal use only                
GFX_Result cpuDrawLine_Thick(const GFX_Point* p1,
                             const GFX_Point* p2,
                             const GFX_DrawState* state);

// internal use only                         
GFX_Result cpuDrawLine_Thick_AA(const GFX_Point* p1,
                                const GFX_Point* p2,
                                const GFX_DrawState* state);

// internal use only                            
GFX_Result cpuDrawLine_AA(const GFX_Point* p1,
                          const GFX_Point* p2,
                          const GFX_DrawState* state);

// internal use only                      
GFX_Result cpuDrawLine_Gradient(const GFX_Point* p1,
                                const GFX_Point* p2,
                                const GFX_DrawState* state);

// internal use only                                
GFX_Result cpuDrawLine_Gradient_AA(const GFX_Point* p1,
                                   const GFX_Point* p2,
                                   const GFX_DrawState* state);                                

// internal use only                               
GFX_Result cpuDrawLine_Horz(const GFX_Point* p1,
                            const GFX_Point* p2,
                            const GFX_DrawState* state);


// internal use only                       
GFX_Result cpuDrawLine_Vert(const GFX_Point* p1,
                            const GFX_Point* p2,
                            const GFX_DrawState* state);   
                           
#endif // GFX_DRAW_PIPELINE_ENABLED                              
#endif /* GFX_DRAW_LINE_H */                              

//DOM-IGNORE-END
