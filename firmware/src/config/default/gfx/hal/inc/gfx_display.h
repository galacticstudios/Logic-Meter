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
    gfx_display.h

  Summary:
    Defines MPLAB Harmony Graphics Hardware Abstraction Layer display
    information struct

  Description:
    Display information.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_DISPLAY_H
#define GFX_DISPLAY_H
//DOM-IGNORE-END

#include "gfx/hal/inc/gfx_common.h"
#include "gfx/hal/inc/gfx_color.h"

// *****************************************************************************
/* Structure:
    GFX_DisplayInfo_t

  Summary:
    Describes a graphical display device.

  Description:
    name - a short human-readable name
    color_formats - mask of color formats this display supports
    rect - the size of the display   

  Remarks:
    None.
*/
typedef struct GFX_DisplayInfo_t
{
    const char name[16];
    
    GFX_ColorMask color_formats;

    GFX_Rect rect;
    
    struct attributes_t
    {        
        int8_t data_width;
        
        struct horizontal_t
        {
            int32_t pulse_width;
            int32_t back_porch;
            int32_t front_porch;
        } horz;
        
        struct vertical_t
        {
            int32_t pulse_width;
            int32_t back_porch;
            int32_t front_porch;
        } vert;
        
        int32_t inv_left_shift;
          
    } attributes;
} GFX_DisplayInfo;

//DOM-IGNORE-BEGIN
// internal use only
LIB_EXPORT GFX_DisplayInfo* GFX_DisplayInfoPtrGet(GFX_Display idx);
//DOM-IGNORE-END

#endif /* GFX_DISPLAY_H */