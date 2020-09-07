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
    gfx_draw.h

  Summary:
    Main header file for MPLAB Harmony Graphics Hardware Abstraction primitive
    draw functions

  Description:
    Shape drawing functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_DRAW_H
#define GFX_DRAW_H
//DOM-IGNORE-END

#include "gfx/hal/inc/gfx_common.h"

#if GFX_DRAW_PIPELINE_ENABLED

#include "gfx/hal/inc/gfx_draw_arc.h"
#include "gfx/hal/inc/gfx_draw_blit.h"
#include "gfx/hal/inc/gfx_draw_circle.h"
#include "gfx/hal/inc/gfx_draw_ellipse.h"
#include "gfx/hal/inc/gfx_draw_line.h"
#include "gfx/hal/inc/gfx_draw_pixel.h"
#include "gfx/hal/inc/gfx_draw_rect.h"
#include "gfx/hal/inc/gfx_draw_stretchblit.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

typedef struct GFX_DrawPipeline_t GFX_DrawPipeline;

// *****************************************************************************
/* Enumeration:
    GFX_DrawMode_t

  Summary:
    Gradient draw modes.

  Description:
    line - draws the outline of a shape
    fill - draws a filled shape
    gradient left/right - draws a gradient from left to right, uses the first
                          two gradient colors
    gradient top/bottom - draws a gradient from top to bottom, uses the first
                          two gradient colors
    

  Remarks:
    None.
*/
typedef enum GFX_DrawMode_t
{
    GFX_DRAW_LINE = 0x0,
    GFX_DRAW_FILL,
    GFX_DRAW_GRADIENT_LEFT_RIGHT,
    GFX_DRAW_GRADIENT_TOP_BOTTOM
} GFX_DrawMode;

#define GFX_DRAW_MODE_COUNT (GFX_DRAW_GRADIENT_TOP_BOTTOM + 1)

// *****************************************************************************
/* Enumeration:
    GFX_AntialiasMode_t

  Summary:
    Enables anti-aliased drawing hint

  Description:
    

  Remarks:
    None.
*/
typedef enum GFX_AntialiasMode_t
{
    GFX_ANTIALIAS_OFF = 0x0,
    GFX_ANTIALIAS_ON  = 0x1
} GFX_AntialiasMode;

#define GFX_ANTIALIAS_MODE_COUNT (GFX_ANTIALIAS_ON+1)

// *****************************************************************************
/* Enumeration:
    GFX_ResizeMode_t

  Summary:
    The algorithm used for resizing a frame

  Description:
    

  Remarks:
    None.
*/
typedef enum GFX_ResizeMode_t
{
    GFX_RESIZE_NEARESTNEIGHBOR = 0x0,
    GFX_RESIZE_BILINEAR
} GFX_ResizeMode;

// DOM-IGNORE-BEGIN
#define GFX_RESIZE_MODE_COUNT (GFX_RESIZE_BILINEAR+1)
//DOM-IGNORE-END

// *****************************************************************************
/* Structure:
    GFX_DrawState_t

  Summary:
    A list of drawing hints for shape drawing algorithms

  Description:
    mode - the shape drawing mode
    
    color - the draw color
    
    gradient - the list of gradient colors
    
    palette - the palette lookup table for blits
    
    alphaEnable - indicates if alpha blending is enabled
    alphaValue - the desired alpha blending amount
    
    maskEnable - indicates if pixel masking is enabled    
    maskValue - the mask/transparency color value
    
    clipEnable - indicate of pixel clipping is enabled
    clipRect - the pixel clipping rectangle

  Remarks:
    None.
*/
typedef struct GFX_DrawState_t
{
    GFX_DrawMode mode;
    
    GFX_Color color;
    GFX_ColorMode colorMode;
    
    struct
    {
        GFX_Color c0;
        GFX_Color c1;
        GFX_Color c2;
        GFX_Color c3;
    } gradient;
    
    GFX_PixelBuffer palette;
    
    const GFX_PixelBuffer* target;
    GFX_Rect targetClipRect;
    
    GFX_BlendMode blendMode;
    GFX_Bool alphaEnable;
    uint32_t globalAlphaValue;
    
    GFX_Bool maskEnable;
    uint32_t maskValue;
    
    GFX_Bool antialias;
    uint32_t thickness;

    GFX_Bool clipEnable;
    GFX_Rect clipRect;    
    
    GFX_ResizeMode resizeMode;
    
    GFX_PipelineMode pipelineMode;
    GFX_DrawPipeline* pipeline;
} GFX_DrawState;

// *****************************************************************************
/* Function:
    GFX_Result GFX_DrawPixel(int32_t x, int32_t y);

  Summary:
    Sets the pixel at X and Y using the current draw state.

  Parameters:
    x - the x coordinate of the pixel
    y - the y coordinate of the pixel
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the pixel was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DrawPixel(int32_t x, int32_t y);


// *****************************************************************************
/* Function:
    GFX_Result GFX_DrawPixelByDrawState(int32_t x,
                                        int32_t y,
                                        GFX_DrawState state);

  Summary:
    Sets the pixel at X and Y using a specified draw state.

  Parameters:
    x - the x coordinate of the pixel
    y - the y coordinate of the pixel
    state - the draw state

  Returns:
    GFX_Result - Returns GFX_TRUE if the pixel was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DrawPixelByDrawState(int32_t x,
                                               int32_t y,
                                               GFX_DrawState state);

// *****************************************************************************
/* Function:
    GFX_Result GFX_Result GFX_DrawLine(int32_t x1,
                                       int32_t y1,
                                       int32_t x2,
                                       int32_t y2);

  Summary:
    Draws a line from (x1,y1) to (x2,y2) using the current draw state.

  Parameters:
    x1 - the x component of the first coordinate of the line
    y1 - the y component of the first coordinate of the line
    x2 - the x component of the second coordinate of the line
    y2 - the y component of the second coordinate of the line
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the line was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DrawLine(int32_t x1,
                                   int32_t y1,
                                   int32_t x2,
                                   int32_t y2);

// *****************************************************************************
/* Function:
    GFX_Result GFX_Result GFX_DrawLine(int32_t x,
                                       int32_t x,
                                       int32_t width,
                                       int32_t height);

  Summary:
    Draws a rectangle  using the specified dimensions and the current
    draw state.
    
  Description:
    Draws a rectangle using the coordinates:
        x,y                     x + width - 1, y
        
        x, y + height - 1       x + width - 1, y + height - 1

  Parameters:
    x - the x position of the top left point of the rectangle
    y - the y position of the top left point of the rectangle
    width - the width of the rectangle in pixels
    height - the height of the rectangle in pixels
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the rectangle was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/                                      
LIB_EXPORT GFX_Result GFX_DrawRect(int32_t x,
                                   int32_t y,
                                   int32_t width,
                                   int32_t height);

// *****************************************************************************
/* Function:
    GFX_Result GFX_Result GFX_DrawCircle(int32_t x,
                                         int32_t y,
                                         int32_t radius);

  Summary:
    Draws a circle from using the specified dimensions and the current
    draw state.
    
  Parameters:
    x - the x component of the origin position
    y - the y component of the origin position
    radius - the radius of the circle in pixels
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the circle was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DEPRECATED GFX_DrawCircle(int32_t x,
								                    int32_t y,
								                    int32_t radius);    

// *****************************************************************************
/* Function:
    GFX_Result GFX_DrawArc(int32_t x,
                           int32_t y,
                           int32_t radius,
                           int32_t startAngle,
                           int32_t centerAngle);

  Summary:
    Draws a filled arc from using the specified dimensions and the current
    draw state.
    
  Parameters:
    x - the x component of the origin position
    y - the y component of the origin position
    radius - the radius of the circle in pixels
    startAngle - the starting angle, in degrees, of the arc
    centerAngle - the center angle, in degrees, of the arc. A positive value draws CW
                  while a negative value draws CCW.
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the circle was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DrawArc(int32_t x,
                                  int32_t y,
                                  int32_t radius,
                                  int32_t startAngle,
                                  int32_t centerAngle);

// *****************************************************************************
/* Function:
    GFX_Result GFX_DrawEllipse(int32_t x,
                       int32_t y,
                       int32_t a,
                       int32_t b,
                       int32_t theta,
                       int32_t startAngle,
                       int32_t centerAngle,
                       int32_t width,
                       int32_t height);

  Summary:
    Draws a filled ellipse from using the specified dimensions and the current
    draw state.
    
  Parameters:
    x - the x component of the origin position
    y - the y component of the origin position
    a - the length of the short side of the ellipse
    b - the length of the long side of the ellipse
    theta - the tilt angle, in degrees, of the ellipse
    startAngle - the starting angle, in degrees, of the arc
    centerAngle - the center angle, in degrees, of the arc. A positive value draws CW
                  while a negative value draws CCW.
    width - width, in pixels, of the rectangular draw area for the ellipse
    height - height, in pixels, of the rectangular draw area for the ellipse
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the circle was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DrawEllipse(int32_t x,
                       int32_t y,
                       int32_t a,
                       int32_t b,
                       int32_t theta,
                       int32_t startAngle,
                       int32_t centerAngle,
                       int32_t width,
                       int32_t height);

// *****************************************************************************
/* Function:
    GFX_Result GFX_Result GFX_DrawBlit(GFX_PixelBuffer* buffer,
                                       int32_t src_x,
                                       int32_t src_y,
                                       int32_t src_width,
                                       int32_t src_height,
                                       int32_t dest_x,
                                       int32_t dest_y);

  Summary:
    Blits a buffer of pixels into the frame buffer.
    
  Description:
    A pixel buffer is an array of pixel data that can be applied in bulk, or
    'blit'ed, to the frame buffer.  Pixel buffers may be of a different color
    mode and will be converted to match the destination frame buffer before
    application.
    
  Parameters:
    buffer - the pointer to the source pixel buffer
    
    src_x - the x component of the rectangle of the buffer to blit, usually 0
    
    src_y - the y component of the rectangle of the buffer to blit, usually 0
    
    src_width - width of the rectangle of the buffer to blit, usually the
                entire with of the source buffer
                
    src_height - height of the rectangle of the buffer to blit, usually the
                 entire height of the source buffer
    
    dest_x - the x position to blit the source rectangle in the
             destination buffer
    
    dest_x - the y position to blit the source rectangle in the
             destination buffer
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the blit was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DrawBlit(GFX_PixelBuffer* buffer,
                                   int32_t src_x,
                                   int32_t src_y,
                                   int32_t src_width,
                                   int32_t src_height,
                                   int32_t dest_x,
                                   int32_t dest_y);

// *****************************************************************************
/* Function:
    GFX_Result GFX_DrawStretchBlit(GFX_PixelBuffer* buffer,
                                   int32_t src_x,
                                   int32_t src_y,
                                   int32_t src_width,
                                   int32_t src_height,
                                   int32_t dest_x,
                                   int32_t dest_y,
                                   int32_t dest_width,
                                   int32_t dest_height);

  Summary:
    Blits a buffer of pixels into the frame buffer.
    
  Description:
    A pixel buffer is an array of pixel data that can be applied in bulk, or
    'blit'ed, to the frame buffer.  Pixel buffers may be of a different color
    mode and will be converted to match the destination frame buffer before
    application.  This version can resize the source data before blitting.
    The option GFXF_RESIZE_METHOD selects the resize technique.
    
  Parameters:
    buffer - the pointer to the source pixel buffer
    
    src_x - the x component of the rectangle of the buffer to blit, usually 0
    
    src_y - the y component of the rectangle of the buffer to blit, usually 0
    
    src_width - width of the rectangle of the buffer to blit, usually the
                entire with of the source buffer
                
    src_height - height of the rectangle of the buffer to blit, usually the
                 entire height of the source buffer
    
    dest_x - the x position to blit the source rectangle in the
             destination buffer
    
    dest_x - the y position to blit the source rectangle in the
             destination buffer
             
    dest_width - the desired resize width
    
    dest_height - the desired resize height
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the blit was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DrawStretchBlit(GFX_PixelBuffer* buffer,
                                          int32_t src_x,
                                          int32_t src_y,
                                          int32_t src_width,
                                          int32_t src_height,
                                          int32_t dest_x,
                                          int32_t dest_y,
                                          int32_t dest_width,
                                          int32_t dest_height);

// *****************************************************************************
/* Function:
    GFX_Result GFX_DrawDirectBlit(GFX_PixelBuffer* buffer,
                                  int32_t src_x,
                                  int32_t src_y,
                                  int32_t src_width,
                                  int32_t src_height,
                                  int32_t dest_x,
                                  int32_t dest_y);

  Summary:
    Blits a buffer of pixels into the frame buffer without performing per-pixel
    operations on the data.
    
  Description:
    A pixel buffer is an array of pixel data that can be applied in bulk, or
    'blit'ed, to the frame buffer.  This method will not perform per-pixel
    operations on the incoming data.  The incoming pixel data color format
    must match the format of the current target buffer.  Area clipping
    operations are still performed if enabled.
    
  Parameters:
    buffer - the pointer to the source pixel buffer
    
    src_x - the x component of the rectangle of the buffer to blit, usually 0
    
    src_y - the y component of the rectangle of the buffer to blit, usually 0
    
    src_width - width of the rectangle of the buffer to blit, usually the
                entire with of the source buffer
                
    src_height - height of the rectangle of the buffer to blit, usually the
                 entire height of the source buffer
    
    dest_x - the x position to blit the source rectangle in the
             destination buffer
    
    dest_x - the y position to blit the source rectangle in the
             destination buffer
            
    
  Returns:
    GFX_Result - Returns GFX_TRUE if the blit was drawn
                 successfully. Otherwise returns GFX_FALSE.
*/
LIB_EXPORT GFX_Result GFX_DrawDirectBlit(GFX_PixelBuffer* buffer,
                                         int32_t src_x,
                                         int32_t src_y,
                                         int32_t src_width,
                                         int32_t src_height,
                                         int32_t dest_x,
                                         int32_t dest_y);

#endif // GFX_DRAW_PIPELINE_ENABLED

#endif /* GFX_DRAW_H */
