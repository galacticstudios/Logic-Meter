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
 Module for Microchip Graphics Library - Aria User Interface Library

  Company:
    Microchip Technology Inc.

  File Name:
    libaria_rectarray.h

  Summary:
    An array implementation for storing rectangles for the
    Aria user interface library

  Description:
    This is an array implementation that is used internally by the 
    Aria user interface library.  All of the memory operations are handled
    by the memory interface that is provided by the active libaria
    context.  Applications that wish to use this implementation must ensure
    that the appropriate libaria context is active when calling these functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_RECTARRAY_H
#define LIBARIA_RECTARRAY_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

typedef struct laContext_t laContext;

// *****************************************************************************
/* Structure:
    laRectArray_t

  Summary:
    Rectangle array definition

  Remarks:
    None.
*/
typedef struct laRectArray_t
{
    GFX_Rect* rects;
    uint32_t size;
    uint32_t capacity;
} laRectArray;

// *****************************************************************************
/* Function:
    laResult laRectArray_Create(laRectArray* arr)

   Summary:
    Initializes a new rectangle array.

   Parameters:
    laRectArray* arr - pointer to the array to initilaize

  Returns:
    laResult - the result of the operation

  Remarks:        
*/
LIB_EXPORT laResult laRectArray_Create(laRectArray* arr);


// *****************************************************************************
/* Function:
    laResult laRectArray_Resize(laRectArray* arr)

   Summary:
    Resizes the capacity of the array.  If the array shrinks, any nodes beyond
    the new capacity will be discarded.

   Parameters:
    laRectArray* arr - pointer to the array to resize
    uint32_t sz - the desired capacity

  Returns:
    laResult - the result of the operation

  Remarks:
*/
LIB_EXPORT laResult laRectArray_Resize(laRectArray* arr, uint32_t sz);

// *****************************************************************************
/* Function:
    laResult laRectArray_PushFront(laRectArray* arr, const GFX_Rect* rect))

   Summary:
    Pushes a new rectangle onto the front of the array.  Shuffles all other
    nodes backward one index.

   Parameters:
    laRectArray* arr - pointer to the array to modify
    const GFX_Rect* rect - the rectangle value
    
  Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_PushFront(laRectArray* arr,
                                          const GFX_Rect* rect);

// *****************************************************************************
/* Function:
    void laRectArray_PopFront(laRectArray* arr)

   Summary:
    Removes the first value from the array.  Shuffles all other nodes forward
    one index.

   Parameters:
    laRectArray* arr - pointer to the array to modify

   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_PopFront(laRectArray* arr);

// *****************************************************************************
/* Function:
    laResult laRectArray_PushBack(laRectArray* arr, const GFX_Rect* rect)

   Summary:
    Pushes a new rectangle onto the back of the array

   Parameters:
    laRectArray* arr - pointer to the array to modify
    const GFX_Rect* rect - the rectangle value
    
  Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_PushBack(laRectArray* arr, const GFX_Rect* rect);

// *****************************************************************************
/* Function:
    void laRectArray_PopBack(laRectArray* arr)

   Summary:
    Removes the last rectangle from the array

   Parameters:
    laRectArray* arr - pointer to the array to modify
*/
LIB_EXPORT laResult laRectArray_PopBack(laRectArray* arr);

// *****************************************************************************
/* Function:
    laResult laRectArray_InsertAt(laRectArray* arr,
                                  uint32_t idx,
                                  const GFX_Rect* rect);

   Summary:
    Inserts a rectangle into an array at a given index.  All existing nodes
    from index are shifted right one place.

   Parameters:
    laRectArray* arr - pointer to the array to modify
    uint32_t idx - the position to insert the value
    const GFX_Rect* rect - the rectangle value
   
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_InsertAt(laRectArray* arr,
                                         uint32_t idx,
                                         const GFX_Rect* rect);

// *****************************************************************************
/* Function:
    laResult laRectArray_RemoveAt(laRectArray* arr, uint32_t idx)

   Summary:
    Removes a rectangle from the array at an index

   Parameters:
    laRectArray* arr - pointer to the array to modify
    uint32_t idx - the index of the value to remove
   
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_RemoveAt(laRectArray* arr, uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laRectArray_Copy(laRectArray* l, laRectArray* r)

   Summary:
    Creates a duplicate of an existing array

   Parameters:
    laRectArray* src - the source array
    laRectArray* dest - the result array
   
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_Copy(laRectArray* src, laRectArray* dest);

// *****************************************************************************
/* Function:
    laResult laRectArray_Clear(laRectArray* arr)

   Summary:
    Removes all values from a given array.  Array capacity remains the same.

   Parameters:
    laRectArray* arr - the array to modify
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_Clear(laRectArray* arr);

// *****************************************************************************
/* Function:
    laResult laRectArray_Destroy(laRectArray* arr)

   Summary:
    Removes all nodes from a given array and frees the memory owned by the array.
    Resets array capacity to zero.

   Parameters:
    laRectArray* arr - the array to modify
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_Destroy(laRectArray* arr);


// *****************************************************************************
/* Function:
    laResult laRectArray_RemoveDuplicates(laRectArray* arr)

   Summary:
    Removes any duplicate rectangles from an array.

   Parameters:
    laRectArray* arr - the array to analyze
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_RemoveDuplicates(laRectArray* arr);

// *****************************************************************************
/* Function:
    laResult laRectArray_SortBySize(laRectArray* arr)

   Summary:
    Sorts a given array largest to smallest.

   Parameters:
    laRectArray* arr - the array to analyze
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_SortBySize(laRectArray* arr);

// *****************************************************************************
/* Function:
    laResult laRectArray_MergeSimilar(laRectArray* arr)

   Summary:
    Analyzes an array and merges any rectangles similar in size.

   Parameters:
    laRectArray* arr - the array to analyze
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_MergeSimilar(laRectArray* arr);

// *****************************************************************************
/* Function:
    laResult laRectArray_RemoveOverlapping(laRectArray* arr)

   Summary:
    Sorts the array by size and then removes any rectangles that are completely
    overlapped by another larger rectangle.

   Parameters:
    laRectArray* arr - the array to modify
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laRectArray_RemoveOverlapping(laRectArray* arr);

#endif /* LIBARIA_RECTARRAY_H */
