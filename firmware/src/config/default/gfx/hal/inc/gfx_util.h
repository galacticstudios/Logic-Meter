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
    gfx_util.h

  Summary:
    Utility functions for the Hardware Abstraction Layer

  Description:
    Layer and point utility functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef GFX_UTIL_H
#define GFX_UTIL_H
//DOM-IGNORE-END

#include "gfx/hal/inc/gfx_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void GFX_UtilOrientPoint(const GFX_Point* point,
                             const GFX_Rect* rect,
                             GFX_Orientation ori)

  Summary:
    Reorients a point to a given orthogonal orientation.

  Parameters:
    const GFX_Point* point - the point to reorient
    const GFX_Rect* rect - the bounding rectangle space
    GFX_Orientation - the orientation setting
    
  Returns:
    GFX_Point
*/
GFX_Point GFX_UtilOrientPoint(const GFX_Point* point,
                              const GFX_Rect* rect,
                              GFX_Orientation ori);
                              
// *****************************************************************************
/* Function:
    void GFX_UtilMirrorPoint(const GFX_Point* point,
                             const GFX_Rect* rect,
                             GFX_Orientation ori)

  Summary:
    Reorients a point to a given mirrored orientation.

  Parameters:
    const GFX_Point* point - the point to reorient
    const GFX_Rect* rect - the bounding rectangle space
    GFX_Orientation - the orientation setting
    
  Returns:
    GFX_Point
*/
GFX_Point GFX_UtilMirrorPoint(const GFX_Point* point,
                              const GFX_Rect* rect,
                              GFX_Orientation ori);

// *****************************************************************************
/* Function:
    GFX_Point GFX_UtilPointFromOrientedSpace(const GFX_Rect* displayRect,
                                             const GFX_Rect* innerRect,
                                             const GFX_Point* pnt,
                                             GFX_Orientation ori,
                                             GFX_Bool mirrored)

  Summary:
    Transforms a point from an oriented rectangle space to an outer space given 
    a display orientation and a mirroring setting.

  Parameters:
    const GFX_Rect* outerRect - the outer rectangle
    const GFX_Rect* subRect - the inner rectangle
    const GFX_Point* point - the point
    GFX_Orientation - the orientation setting
    GFX_Bool - the mirroring setting
    
  Returns:
    GFX_Point
*/                     
GFX_Point GFX_UtilPointFromOrientedSpace(const GFX_Rect* outerRect,
                                         const GFX_Rect* innerRect,
                                         const GFX_Point* pnt,
                                         GFX_Orientation ori,
                                         GFX_Bool mirrored);

// *****************************************************************************
/* Function:
    GFX_Point GFX_UtilPointToOrientedSpace(const GFX_Rect* displayRect,
                                           const GFX_Rect* layerRect,
                                           const GFX_Point* point,
                                           GFX_Orientation ori,
                                           GFX_Bool mirrored)

  Summary:
    Transforms a point to an oriented rectangle space to an outer space given 
    a display orientation and a mirroring setting.

  Parameters:
    const GFX_Rect* outerRect - the outer rectangle
    const GFX_Rect* subRect - the inner rectangle
    const GFX_Point* point - the point
    GFX_Orientation - the orientation setting
    GFX_Bool - the mirroring setting
    
  Returns:
    GFX_Point
*/                                      
GFX_Point GFX_UtilPointToOrientedSpace(const GFX_Rect* outerRect,
                                       const GFX_Rect* innerRect,
                                       const GFX_Point* pnt,
                                       GFX_Orientation ori,
                                       GFX_Bool mirrored);

// *****************************************************************************
/* Function:
    GFX_Size GFX_UtilSizeFromOrientedSpace(const GFX_Size* size, GFX_Orientation ori)

  Summary:
    Transforms a size tuple from oriented space to screen space

  Parameters:
    const GFX_Size* size - the size dimension
    GFX_Orientation - the orientation setting
        
  Returns:
    GFX_Size
*/                                    
GFX_Size GFX_UtilSizeFromOrientedSpace(const GFX_Size* size, GFX_Orientation ori);

// *****************************************************************************
/* Function:
    void GFX_UtilSizeToOrientedSpace(const GFX_Size* size, GFX_Orientation ori)

  Summary:
    Transforms a size tuple from screen space to oriented space

  Parameters:
    const GFX_Size* size - the size dimension
    GFX_Orientation - the orientation setting
        
  Returns:
    GFX_Size
*/
GFX_Size GFX_UtilSizeToOrientedSpace(const GFX_Size* size, GFX_Orientation ori);

// *****************************************************************************
/* Function:
    void GFX_UtilSortPointsX(GFX_Point* p1, GFX_Point* p2)

  Summary:
    Sorts two points in the X axis

  Parameters:
    GFX_Point* p1 - the first point to sort
    GFX_Point* p2 - the second point to sort
        
  Returns:
    void
*/
void GFX_UtilSortPointsX(GFX_Point* p1, GFX_Point* p2);

// *****************************************************************************
/* Function:
    void GFX_UtilSortPointsY(GFX_Point* p1, GFX_Point* p2)

  Summary:
    Sorts two points in the Y axis

  Parameters:
    GFX_Point* p1 - the first point to sort
    GFX_Point* p2 - the second point to sort
        
  Returns:
    void
*/
void GFX_UtilSortPointsY(GFX_Point* p1, GFX_Point* p2);

// *****************************************************************************
/* Function:
    int32_t GFX_UtilGetXGivenYOnLine(GFX_Point p1, GFX_Point p2, int32_t y)

  Summary:
    Gets X given Y and two points on a line

  Parameters:
    GFX_Point p1 - the first point in line
    GFX_Point p2 - the second point in line
    int32_t y - the y-coordinate of the point of interest
        
  Returns:
    int32_t - the x-coordinate of the point of interest
*/
LIB_EXPORT int32_t GFX_UtilGetXGivenYOnLine(GFX_Point p1, GFX_Point p2, int32_t y);


// *****************************************************************************
/* Function:
    int32_t GFX_UtilGetYGivenXOnLine(GFX_Point p1, GFX_Point p2, int32_t x)

  Summary:
    Gets Y given X and two points on a line

  Parameters:
    GFX_Point p1 - the first point in line
    GFX_Point p2 - the second point in line
    int32_t x - the x-coordinate of the point of interest
        
  Returns:
    int32_t - the y-coordinate of the point of interest
*/

LIB_EXPORT int32_t GFX_UtilGetYGivenXOnLine(GFX_Point p1, GFX_Point p2, int32_t x);

#endif /* GFX_UTIL_H */