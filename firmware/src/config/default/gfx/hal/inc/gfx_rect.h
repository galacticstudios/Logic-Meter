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
    gfx_rect.h

  Summary:
    Defines general purposes rectangle functions.

  Description:
    Rectangle management functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_RECT_H
#define GFX_RECT_H
//DOM-IGNORE-END

#include "gfx/hal/inc/gfx_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    GFX_Bool GFX_RectContainsPoint(const GFX_Rect* rect, const GFX_Point* point)

  Summary:
    Determines if a point is inside a rectangle.

  Parameters:
    const GFX_Rect* rect - the rectangle to test
    const GFX_Point* point - the point to use for the test
    
  Returns:
    GFX_Bool - GFX_TRUE if the point is inside the rectangle
*/
LIB_EXPORT GFX_Bool GFX_RectContainsPoint(const GFX_Rect* rect, const GFX_Point* point);

// *****************************************************************************
/* Function:
    GFX_Bool GFX_RectContainsRect(const GFX_Rect* l_rect, const GFX_Rect* r_rect)

  Summary:
    Determines if a rectangle is completely inside another rectangle.  Still
    returns true if the edges are touching.

  Parameters:
    const GFX_Rect* l_rect - the subject rectangle
    const GFX_Rect* r_rect - the object rectangle
    
  Returns:
    GFX_Bool - returns GFX_TRUE if r_rect is completly inside l_rect
*/
LIB_EXPORT GFX_Bool GFX_RectContainsRect(const GFX_Rect* l_rect, const GFX_Rect* r_rect);

// *****************************************************************************
/* Function:
    GFX_Bool GFX_RectIntersects(const GFX_Rect* l_rect, const GFX_Rect* r_rect)

  Summary:
    Determines if two rectangles are intersecting

  Parameters:
    const GFX_Rect* l_rect - rectangle argument
    const GFX_Rect* r_rect - rectangle argument
    
  Returns:
    GFX_Bool - returns GFX_TRUE if l_rect and r_rect are intersecting
*/
LIB_EXPORT GFX_Bool GFX_RectIntersects(const GFX_Rect* l_rect, const GFX_Rect* r_rect);

// *****************************************************************************
/* Function:
    GFX_Rect GFX_RectCombine(const GFX_Rect* l_rect, 
                             const GFX_Rect* r_rect)

  Summary:
    Combines the area of two rectangles into a single rectangle.

  Parameters:
    const GFX_Rect* l_rect - the first rectangle
    const GFX_Rect* r_rect - the second rectangle
    
  Returns:
    void
    
  Remarks:
*/
LIB_EXPORT GFX_Rect GFX_RectCombine(const GFX_Rect* l_rect, 
                                    const GFX_Rect* r_rect);

// *****************************************************************************
/* Function:
    void GFX_RectClip(const GFX_Rect* l_rect, 
                      const GFX_Rect* r_rect, 
                      GFX_Rect* result)

  Summary:
    Clips a rectangle to the space of another rectangle.  The result rectangle
    is a rectangle that will fit inside both of the given rectangles.

  Parameters:
    const GFX_Rect* l_rect - the subject rectangle
    const GFX_Rect* r_rect - the object rectangle
    GFX_Rect* result - the result rectangle
    
  Returns:
    void
    
  Remarks:
    result will equals l_rect if the rectangles aren't intersecting
    
*/
LIB_EXPORT void GFX_RectClip(const GFX_Rect* l_rect, 
                             const GFX_Rect* r_rect, 
                             GFX_Rect* result);

// *****************************************************************************
/* Function:
    GFX_Rect GFX_RectClipAdj(const GFX_Rect* l_rect,
                             const GFX_Rect* r_rect, 
                             GFX_Rect* adj)

  Summary:
    Returns the rectangle clipped using r_rect, and also adjusts the third rectangle

  Parameters:
    const GFX_Rect* l_rect - the subject rectangle
    const GFX_Rect* r_rect - the object rectangle
    GFX_Rect* adj - the adjust rectangle
    
  Returns:
    void
    
  Remarks:
    result will equals l_rect if the rectangles aren't intersecting
    
*/
LIB_EXPORT GFX_Rect GFX_RectClipAdj(const GFX_Rect* l_rect,
                                    const GFX_Rect* r_rect, 
                                    GFX_Rect* adj);

// *****************************************************************************
/* Function:
    GFX_Rect GFX_RectFromPoints(const GFX_Point* p1,
                                const GFX_Point* p2)

  Summary:
    Returns a GFX_Rect structure based on 2 points

  Parameters:
    const GFX_Point* p1 - the first point
    const GFX_Point* p2 - the second point
    
  Returns:
    GFX_Rect
    
  Remarks:
    
*/                             
LIB_EXPORT GFX_Rect GFX_RectFromPoints(const GFX_Point* p1,
                                       const GFX_Point* p2);

// *****************************************************************************
/* Function:
    void GFX_RectToPoints(const GFX_Rect* rect,
                          GFX_Point* p1,
                          GFX_Point* p2

  Summary:
    Returns the points for the upper left and lower right vertices 
    of a rectangle

  Parameters:
    const GFX_Rect* rect - the rectangle
    GFX_Point* p1 - the point of upper left vertex
    GFX_Point* p2 - the point of the lower right vertex
    
  Returns:
    GFX_Rect
    
  Remarks:
    
*/
LIB_EXPORT void GFX_RectToPoints(const GFX_Rect* rect,
                                 GFX_Point* p1,
                                 GFX_Point* p2);    

// *****************************************************************************
/* Function:
    uint32_t GFX_RectSplit(const GFX_Rect* sub,
                           const GFX_Rect* obj,
                           GFX_Rect res[4])

  Summary:
    Splits two overlapping rectangles into several (up to 4) 
    non-overlapping rectangles

  Parameters:
    const GFX_Rect* sub - the first rectangle
    const GFX_Rect* obj - the second rectangle
    GFX_Rect res[4] - the output rectangles
    
  Returns:
    uint32_t - the number of non-overlapping rectangles returned
    
  Remarks:
    
*/
LIB_EXPORT uint32_t GFX_RectSplit(const GFX_Rect* sub,
                                  const GFX_Rect* obj,
                                  GFX_Rect res[4]); 

// *****************************************************************************
/* Function:
    int32_t GFX_RectCompare(const GFX_Rect* l,
                            const GFX_Rect* r)

  Summary:
    Returns 1 if the two rectangles have the same position and dimensions

  Parameters:
    const GFX_Rect* l,
    const GFX_Rect* r
    
  Returns:
    int32_t
    
  Remarks:
    
*/
LIB_EXPORT int32_t GFX_RectCompare(const GFX_Rect* l,
                                   const GFX_Rect* r);   

// *****************************************************************************
/* Function:
    GFX_Bool GFX_RectsAreSimilar(const GFX_Rect* l,
                                 const GFX_Rect* r)

  Summary:
    Returns GFX_TRUE if the two rectangles are adjacent and vertically 
    or horizontally aligned

  Parameters:
    const GFX_Rect* l - the first rectangle
    const GFX_Rect* r - the second rectangle
    
  Returns:
    GFX_Bool
    
  Remarks:
    
*/
LIB_EXPORT GFX_Bool GFX_RectsAreSimilar(const GFX_Rect* l,
                                        const GFX_Rect* r);                                                            

//DOM-IGNORE-BEGIN
static const GFX_Rect GFX_Rect_Zero = {0, 0, 0, 0};
//DOM-IGNORE-END
#endif /* GFX_RECT_H */
