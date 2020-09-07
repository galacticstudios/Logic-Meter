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
    gfx_color.h

  Summary:
    Contains functions for color information and manipulation operations

  Description:
    Color conversion and color channel management
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef GFX_COLOR_H
#define GFX_COLOR_H
//DOM-IGNORE-END

#include "gfx/hal/inc/gfx_common.h"

// DOM-IGNORE-BEGIN
#define GFX_COLOR_MAX_SIZE 4

#define RGB_8_BITS             255
#define RGB_6_BITS             63
#define RGB_5_BITS             31
#define RGB_3_BITS             7
#define RGB_2_BITS             2

#define RGB_332_RED_MASK       0xE0
#define RGB_332_GREEN_MASK     0x1C
#define RGB_332_BLUE_MASK      0x3

#define RGB_565_RED_MASK       0xF800
#define RGB_565_GREEN_MASK     0x7E0
#define RGB_565_BLUE_MASK      0x1F

#define RGBA_5551_RED_MASK     0xF800
#define RGBA_5551_GREEN_MASK   0x7C0
#define RGBA_5551_BLUE_MASK    0x3E
#define RGBA_5551_ALPHA_MASK   0x1

#define RGB_888_RED_MASK       0xFF0000
#define RGB_888_GREEN_MASK     0xFF00
#define RGB_888_BLUE_MASK      0xFF

#define RGBA_8888_RED_MASK     0xFF000000
#define RGBA_8888_GREEN_MASK   0xFF0000
#define RGBA_8888_BLUE_MASK    0xFF00
#define RGBA_8888_ALPHA_MASK   0xFF

#define AGBA_8888_RED_MASK     0xFF0000
#define AGBA_8888_GREEN_MASK   0xFF00
#define AGBA_8888_BLUE_MASK    0xFF
#define AGBA_8888_ALPHA_MASK   0xFF000000

//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    GFX_ColorMask_t

  Summary:
    Maskable list of color valies.

*/
typedef enum GFX_ColorMask_t
{
    GFX_COLOR_MASK_GS_8      = 0x1,
    GFX_COLOR_MASK_RGB_332   = 0x4,
    GFX_COLOR_MASK_RGB_565   = 0x8,
    GFX_COLOR_MASK_RGBA_5551 = 0x10,
    GFX_COLOR_MASK_RGB_888   = 0x20,
    GFX_COLOR_MASK_RGBA_8888 = 0x40,
    GFX_COLOR_MASK_ARGB_8888 = 0x80,
    GFX_COLOR_MASK_YUV      = 0x100,
    GFX_COLOR_MASK_ALL = GFX_COLOR_MASK_GS_8      |
                         GFX_COLOR_MASK_RGB_332   |
                         GFX_COLOR_MASK_RGB_565   |
                         GFX_COLOR_MASK_RGBA_5551 |
                         GFX_COLOR_MASK_RGB_888   | 
                         GFX_COLOR_MASK_RGBA_8888 |
                         GFX_COLOR_MASK_ARGB_8888 |
                         GFX_COLOR_MASK_YUV
} GFX_ColorMask;

// *****************************************************************************
/* Enumeration:
    GFX_ColorMode_t

  Summary:
    List of available color modes.
*/
typedef enum GFX_ColorMode_t
{
    GFX_COLOR_MODE_GS_8       = 0x0,
    GFX_COLOR_MODE_RGB_332,
    GFX_COLOR_MODE_RGB_565,
    GFX_COLOR_MODE_RGBA_5551,
    GFX_COLOR_MODE_RGB_888,
    GFX_COLOR_MODE_RGBA_8888,
    GFX_COLOR_MODE_ARGB_8888,
    GFX_COLOR_MODE_YUV,
    GFX_COLOR_MODE_INDEX_1,
    GFX_COLOR_MODE_INDEX_4,
    GFX_COLOR_MODE_INDEX_8,
    GFX_COLOR_MODE_LAST = GFX_COLOR_MODE_INDEX_8
} GFX_ColorMode;

#define GFX_COLOR_MODE_LAST_COLOR (GFX_COLOR_MODE_YUV)
#define GFX_COLOR_MODE_COUNT      (GFX_COLOR_MODE_LAST + 1)

#define GFX_COLOR_MODE_IS_PIXEL(mode) ((mode >= GFX_COLOR_MODE_GS_8) && (mode <= GFX_COLOR_MODE_YUV))
#define GFX_COLOR_MODE_IS_INDEX(mode) ((mode >= GFX_COLOR_MODE_INDEX_1) && (mode <= GFX_COLOR_MODE_INDEX_8))

#define GFX_COLOR_MODE_IS_ALPHA(mode) ((mode == GFX_COLOR_MODE_RGBA_5551) || (mode == GFX_COLOR_MODE_RGBA_8888) || (mode == GFX_COLOR_MODE_ARGB_8888))

// *****************************************************************************
/* Enumeration:
    GFX_BitsPerPixel_t

  Summary:
    List of available bits-per-pixel sizes.    
*/
typedef enum GFX_BitsPerPixel_t
{
    GFX_BPP1,
    GFX_BPP4,
    GFX_BPP8,
    GFX_BPP16,
    GFX_BPP24,
    GFX_BPP32
} GFX_BitsPerPixel;

// *****************************************************************************
/* Structure:
    GFX_ColorModeInfo_t

  Summary:
    Struct that provides information about a color mode.

  Description:
    size - size in bytes
    bpp - bpp value
    bppOrdinal - bpp enum value
    masks - the masks used for extracting individual color channel information

  Remarks:
    None.
*/
typedef struct GFX_ColorModeInfo_t
{
    uint32_t size;
    uint32_t bpp;
    GFX_BitsPerPixel bppOrdinal;

    struct masks
    {
        uint32_t red;
        uint32_t green;
        uint32_t blue;
        uint32_t alpha;
    } mask;
    
    struct shifts
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    } shift;
    
} GFX_ColorModeInfo;

extern GFX_ColorModeInfo GFX_ColorInfo[GFX_COLOR_MODE_COUNT];

// *****************************************************************************
/* Structure:
    GFX_ColorName_t

  Summary:
    Color name reference table
*/
typedef enum GFX_ColorName_t
{
    GFX_COLOR_BLACK,
    GFX_COLOR_WHITE,
    GFX_COLOR_RED,
    GFX_COLOR_LIME,
    GFX_COLOR_BLUE,
    GFX_COLOR_YELLOW,
    GFX_COLOR_CYAN,
    GFX_COLOR_MAGENTA,
    GFX_COLOR_SILVER,
    GFX_COLOR_DARKGRAY,
    GFX_COLOR_GRAY,
    GFX_COLOR_LIGHTGRAY,
    GFX_COLOR_MAROON,
    GFX_COLOR_OLIVE,
    GFX_COLOR_GREEN,
    GFX_COLOR_PURPLE,
    GFX_COLOR_TEAL,
    GFX_COLOR_NAVY,
    GFX_COLOR_LAST
} GFX_ColorName;

#if GFX_DRAW_PIPELINE_ENABLED

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

LIB_EXPORT GFX_ColorModeInfo GFX_ColorModeInfoGet(GFX_ColorMode mode);

// *****************************************************************************
/* Function:
    GFX_Color GFX_ColorValue(GFX_ColorMode mode, GFX_ColorName name)

  Summary:
    Used for getting a color value by name.

  Parameters:
    GFX_ColorMode - the color mode for the return type
    GFX_ColorName - the name of the color to retrieve
    
  Returns:
    GFX_Color - the color value of the given name in the specified format
    
  Remarks:
    
*/
LIB_EXPORT GFX_Color GFX_ColorValue(GFX_ColorMode mode, GFX_ColorName name);

// *****************************************************************************
/* Function:
    uint32_t GFX_ColorChannelRed(GFX_Color clr, GFX_ColorMode mode)

  Summary:
    Used for getting the red color channel of a given color value.

  Description:
    

  Parameters:
    GFX_Color - the source color value
    GFX_ColorMode - the source color mode
    
  Returns:
    uint32_t - the red color channel
    
  Remarks:
    
*/
LIB_EXPORT uint32_t GFX_ColorChannelRed(GFX_Color clr, GFX_ColorMode mode);

// *****************************************************************************
/* Function:
    uint32_t GFX_ColorChannelGreen(GFX_Color clr, GFX_ColorMode mode)

  Summary:
    Used for getting the green color channel of a given color value.

  Description:
    

  Parameters:
    GFX_Color - the source color value
    GFX_ColorMode - the source color mode
    
  Returns:
    uint32_t - the green color channel
    
  Remarks:
    
*/
LIB_EXPORT uint32_t GFX_ColorChannelGreen(GFX_Color clr, GFX_ColorMode mode);

// *****************************************************************************
/* Function:
    uint32_t GFX_ColorChannelBlue(GFX_Color clr, GFX_ColorMode mode)

  Summary:
    Used for getting the blue color channel of a given color value.

  Description:
    

  Parameters:
    GFX_Color - the source color value
    GFX_ColorMode - the source color mode
    
  Returns:
    uint32_t - the blue color channel
    
  Remarks:
    
*/
LIB_EXPORT uint32_t GFX_ColorChannelBlue(GFX_Color clr, GFX_ColorMode mode);

// *****************************************************************************
/* Function:
    uint32_t GFX_ColorChannelAlpha(GFX_Color clr, GFX_ColorMode mode)

  Summary:
    Used for getting the alpha color channel of a given color value.

  Description:
    

  Parameters:
    GFX_Color - the source color value
    GFX_ColorMode - the source color mode
    
  Returns:
    uint32_t - the alpha color channel
    
  Remarks:
    
*/
LIB_EXPORT uint32_t GFX_ColorChannelAlpha(GFX_Color clr, GFX_ColorMode mode); 

// *****************************************************************************
/* Function:
    GFX_Color GFX_ColorConvert(GFX_ColorMode mode_in,
                               GFX_ColorMode mode_out,
                               GFX_Color color)

  Summary:
    Converts a color value from one mode to another


  Parameters:
    GFX_ColorMode - the input color mode
    GFX_ColorMode - the output color mode
    GFX_Color - the source color
    
  Returns:
    GFX_Color - the result color
    
  Remarks:
    
*/
LIB_EXPORT GFX_Color GFX_ColorConvert(GFX_ColorMode mode_in,
                                      GFX_ColorMode mode_out,
                                      GFX_Color color);
                            
//LIB_EXPORT GFX_Color GFX_ColorBlend(GFX_Color fore, GFX_Color back, GFX_ColorMode mode);

// *****************************************************************************
/* Function:
    GFX_Color GFX_ColorBlend_RGBA_8888(GFX_Color fore, GFX_Color back)

  Summary:
    Blends two RGBA8888 colors together using their alpha channel values.

  Description:
    

  Parameters:
    GFX_Color - the foreground color
    GFX_Color - the background color
    
  Returns:
    GFX_Color - the blended result color
    
  Remarks:
    
*/    
LIB_EXPORT GFX_Color GFX_ColorBlend_RGBA_8888(GFX_Color fore, GFX_Color back);

// *****************************************************************************
/* Function:
    GFX_Color GFX_ColorLerp(GFX_Color l,
                            GFX_Color r,
                            uint32_t percent,
                            GFX_ColorMode mode)

  Summary:
    Linear interpolation between two colors

  Parameters:
    GFX_Color - first color input
    GFX_Color - second color input
    uint32_t - percentage of interpolation [0-100]
    GFX_ColorMode - input color mode
    
  Returns:
    GFX_Color - the result color
    
  Remarks:
    
*/
LIB_EXPORT GFX_Color GFX_ColorLerp(GFX_Color l,
                                   GFX_Color r,
                                   uint32_t percent,
                                   GFX_ColorMode mode);
                                   
// *****************************************************************************
/* Function:
    GFX_Color GFX_ColorBilerp(GFX_Color c00,
                              GFX_Color c01,
                              GFX_Color c10,
                              GFX_Color c11,
                              uint32_t xper,
                              uint32_t yper,
                              GFX_ColorMode mode)

  Summary:
    Calculates bilinear interpolation between four colors

  Parameters:
    GFX_Color c00 - top left color input
    GFX_Color c01 - top right color input
    GFX_Color c10 - bottom left color input
    GFX_Color c11 - bottom right color input
    uint32_t xper - percentage of interpolation in x [0-100]
    uint32_t yper - percentage of interpolation in y [0-100]
    GFX_ColorMode - input color mode
    
  Returns:
    GFX_Color - the result color
    
  Remarks:
    
*/
LIB_EXPORT GFX_Color GFX_ColorBilerp(GFX_Color c00,
                                     GFX_Color c01,
                                     GFX_Color c10,
                                     GFX_Color c11,
                                     uint32_t xper,
                                     uint32_t yper,
                                     GFX_ColorMode mode);                                   

#endif // GFX_DRAW_PIPELINE_ENABLED
#endif /* GFX_COLOR_H */
