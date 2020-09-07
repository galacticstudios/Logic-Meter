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
    libaria_array.h

  Summary:
    An array implementation for storing pointers for the
    Aria user interface library

  Description:
    This is an array implementation that is used internally by the 
    Aria user interface library.  All of the memory operations are handled
    by the memory interface that is provided by the active libaria
    context.  Applications that wish to use this implementation must ensure
    that the appropriate libaria context is active when calling these functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_ARRAY_H
#define LIBARIA_ARRAY_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

typedef struct laContext_t laContext;

// *****************************************************************************
/* Structure:
    laArray_t

  Summary:
    Array data structure definition

  Remarks:
    None.
*/
typedef struct laArray_t
{
    void** values;
    uint32_t size;
    uint32_t capacity;
} laArray;

// *****************************************************************************
/* Function:
    laResult laArray_Create(laArray* arr)

   Summary:
    Initializes a new array.

   Parameters:
    laArray* arr - pointer to the array to initilaize

  Returns:
    laResult - the result of the operation

  Remarks:        
*/
LIB_EXPORT laResult laArray_Create(laArray* arr);


// *****************************************************************************
/* Function:
    laResult laArray_Resize(laArray* arr)

   Summary:
    Resizes the capacity of the array.  If the array shrinks, any nodes beyond
    the new capacity will be discarded.

   Parameters:
    laArray* arr - pointer to the array to resize
    uint32_t sz - the desired capacity

  Returns:
    laResult - the result of the operation

  Remarks:
*/
LIB_EXPORT laResult laArray_Resize(laArray* arr, uint32_t sz);

// *****************************************************************************
/* Function:
    laResult laArray_PushFront(laArray* arr, void* val))

   Summary:
    Pushes a new value onto the front of the array.  Shuffles all other
    nodes backward one index.

   Parameters:
    laArray* arr - pointer to the array to modify
    void* val - the value to store
    
  Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_PushFront(laArray* arr,
                                      void* val);

// *****************************************************************************
/* Function:
    void laArray_PopFront(laArray* arr)

   Summary:
    Removes the first value from the array.  Shuffles all other nodes forward
    one index.

   Parameters:
    laArray* arr - pointer to the array to modify

   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_PopFront(laArray* arr);

// *****************************************************************************
/* Function:
    laResult laArray_PushBack(laArray* arr, void* val)

   Summary:
    Pushes a new value onto the back of the array

   Parameters:
    laArray* arr - pointer to the array to modify
    void* val - the value to store
    
  Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_PushBack(laArray* arr, void* val);

// *****************************************************************************
/* Function:
    void laArray_PopBack(laArray* arr)

   Summary:
    Removes the last value from the array

   Parameters:
    laArray* arr - pointer to the array to modify
*/
LIB_EXPORT laResult laArray_PopBack(laArray* arr);

// *****************************************************************************
/* Function:
    laResult laArray_InsertAt(laArray* arr,
                              uint32_t idx,
                              void* val);

   Summary:
    Inserts a value into an array at a given index.  All existing nodes
    from index are shifted right one place.

   Parameters:
    laArray* arr - pointer to the array to modify
    uint32_t idx - the position to insert the value
    void* val - the value to store
   
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_InsertAt(laArray* arr,
                                     uint32_t idx,
                                     void* val);

// *****************************************************************************
/* Function:
    laResult laArray_RemoveAt(laArray* arr, uint32_t idx)

   Summary:
    Removes a value from the array at an index

   Parameters:
    laArray* arr - pointer to the array to modify
    uint32_t idx - the index of the value to remove
   
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_RemoveAt(laArray* arr, uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laArray_Remove(laArray* arr, void* val)

   Summary:
    Removes the first instance of a value from the array and shuffles
    all values left to fill the gap

   Parameters:
    laArray* arr - pointer to the array to modify
    void* val - the value to remove
   
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_Remove(laArray* arr, void* val);

// *****************************************************************************
/* Function:
    void* laArray_Get(laArray* arr, uint32_t idx);

   Summary:
    Returns the pointer to the entry at index idx

   Parameters:
    laList* list - pointer to the list to reference
    uint32_t idx - the index of the entry to get
   
   Returns:
    void* - pointer to the entry at idx
*/
void* laArray_Get(laArray* arr, uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laArray_Find(laArray* arr, void* val)

   Summary:
    Finds the first index of a value in the array

   Parameters:
    laArray* arr - pointer to the array to search
    void* val - the value to remove
   
   Returns:
    int32_t - the first index of the value or -1 if not found
*/
LIB_EXPORT int32_t laArray_Find(laArray* arr, void* val);


// *****************************************************************************
/* Function:
    laResult laArray_Copy(laArray* l, laArray* r)

   Summary:
    Creates a duplicate of an existing array

   Parameters:
    laArray* src - the source array
    laArray* dest - the result array
   
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_Copy(laArray* src, laArray* dest);

// *****************************************************************************
/* Function:
    void laArray_Clear(laArray* arr)

   Summary:
    Removes all values from a given array.  Array capacity remains the same.

   Parameters:
    laArray* arr - the array to modify
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_Clear(laArray* arr);

// *****************************************************************************
/* Function:
    void laArray_Destroy(laArray* arr)

   Summary:
    Removes all nodes from a given array and frees the memory owned by the array.
    Resets array capacity to zero.

   Parameters:
    laArray* arr - the array to modify
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_Destroy(laArray* arr);

// *****************************************************************************
/* Function:
    void laArray_Free(laArray* arr)

   Summary:
    Removes all nodes from a given array and frees the memory owned by the array.
    Does not free array capacity.

   Parameters:
    laArray* arr - the array to modify
       
   Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laArray_Free(laArray* arr);

#endif /* LIBARIA_ARRAY_H */
