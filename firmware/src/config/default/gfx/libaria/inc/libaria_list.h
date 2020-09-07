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
    libaria_list.h

  Summary:
    A linked list implementation for the Aria user interface library

  Description:
    This is a linked list implementation that is used internally by the 
    Aria user interface library.  All of the memory operations are handled
    by the memory interface that is provided by the active libaria
    context.  Applications that wish to use this implementation must ensure
    that the appropriate libaria context is active when calling these functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_LIST_H
#define LIBARIA_LIST_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

typedef struct laContext_t laContext;

// *****************************************************************************
/* Structure:
    laListNode_t

  Summary:
    Linked list node definition

  Remarks:
    None.
*/
typedef struct laListNode_t
{
    struct laListNode_t* next;
    void*  val;
} laListNode;


// *****************************************************************************
/* Structure:
    laList_t

  Summary:
    Linked list definition

  Remarks:
    None.
*/
typedef struct laList_t
{
    laListNode* head;
    laListNode* tail;
    size_t size;
} laList;

// *****************************************************************************
/* Function:
    int32_t laList_Create(laList* list)

   Summary:
    Initializes a new linked list

   Parameters:
    laList* list - pointer to the list to initilaize

  Returns:
    int32_t - 0 if success, -1 if failure

  Remarks:        
*/
LIB_EXPORT int32_t laList_Create(laList* list);

// *****************************************************************************
/* Function:
    int32_t laList_Assign(laList* list, size_t idx, void* val)

   Summary:
    Assignes a new pointer to an index in the list

   Parameters:
    laList* list - pointer to the list to modify
    size_t idx - the index to modify
    void* val - the new value of the node
    
  Returns:
    int32_t - 0 if success, -1 if failure
*/
LIB_EXPORT int32_t laList_Assign(laList* list, size_t idx, void* val);

// *****************************************************************************
/* Function:
    int32_t laList_PushFront(laList* list, void* val)

   Summary:
    Pushes a new node onto the front of the list

   Parameters:
    laList* list - pointer to the list to modify
    void* val - the new value of the node
    
  Returns:
    int32_t - 0 if success, -1 if failure
*/
LIB_EXPORT int32_t laList_PushFront(laList* list, void*);

// *****************************************************************************
/* Function:
    void laList_PopFront(laList* list)

   Summary:
    Removes the first value from the list

   Parameters:
    laList* list - pointer to the list to modify
*/
LIB_EXPORT void laList_PopFront(laList* list);

// *****************************************************************************
/* Function:
    int32_t laList_PushBack(laList* list, void* val)

   Summary:
    Pushes a new node onto the back of the list

   Parameters:
    laList* list - pointer to the list to modify
    void* val - the new value of the node
    
  Returns:
    int32_t - 0 if success, -1 if failure
*/
LIB_EXPORT int32_t laList_PushBack(laList* list, void* val);

// *****************************************************************************
/* Function:
    void laList_PopBack(laList* list)

   Summary:
    Removes the last value from the list

   Parameters:
    laList* list - pointer to the list to modify
*/
LIB_EXPORT int32_t laList_PopBack(laList* list);

// *****************************************************************************
/* Function:
    void* laList_Get(laList* list, uint32_t idx)

   Summary:
    Retrieves a value from the list

   Parameters:
    laList* list - pointer to the list to reference
    uint32_t idx - the index of the value to retrieve
   
   Returns:
     void* - the retrieved value
*/
LIB_EXPORT void* laList_Get(laList* list, uint32_t idx);

// *****************************************************************************
/* Function:
    int32_t laList_Find(laList* list, void* val)

   Summary:
    Retrieves the index of a value from the list

   Parameters:
    laList* list - pointer to the list to reference
    void* val - the value to search for
   
   Returns:
    int32_t - the index of the value searched for
*/
LIB_EXPORT int32_t laList_Find(laList* list, void* val);

// *****************************************************************************
/* Function:
    int32_t laList_InsertAt(laList* list,
                            void* val,
                            uint32_t idx)

   Summary:
    Inserts an item into a list at a given index.  All existing from index
    are shifted right one place.

   Parameters:
    laList* list - pointer to the list to modify
    void* val - the value to insert
    uint32_t idx - the position to insert the value
   
   Returns:
    int32_t - 0 if success, -1 if failure
*/
LIB_EXPORT int32_t laList_InsertAt(laList* list,
                                   void* val,
                                   uint32_t idx);

// *****************************************************************************
/* Function:
    int32_t laList_Remove(laList* list, void*)

   Summary:
    Removes an item from the list

   Parameters:
    laList* list - pointer to the list to modify
    void* val - the value to remove
   
   Returns:
    int32_t - 0 if success, -1 if failure
*/
LIB_EXPORT int32_t laList_Remove(laList* list, void*);

// *****************************************************************************
/* Function:
    int32_t laList_Remove(laList* list, uint32_t idx)

   Summary:
    Removes an item from the list at an index

   Parameters:
    laList* list - pointer to the list to modify
    uint32_t idx - the index of the value to remove
   
   Returns:
    int32_t - 0 if success, -1 if failure
*/
LIB_EXPORT int32_t laList_RemoveAt(laList* list, uint32_t idx);

// *****************************************************************************
/* Function:
    int32_t laList_Copy(laList* l, laList* r)

   Summary:
    Creates a duplicate of an existing list

   Parameters:
    laList* l - the source list
    laList* r - the result list
   
   Returns:
    int32_t - 0 if success, -1 if failure
*/
LIB_EXPORT int32_t laList_Copy(laList* l, laList* r);

// *****************************************************************************
/* Function:
    void laList_Clear(laList* list)

   Summary:
    Removes all nodes from a given list

   Parameters:
    laList* list - the list to modify
       
   Returns:
    void
*/
LIB_EXPORT void laList_Clear(laList* list);

// *****************************************************************************
/* Function:
    void laList_Destroy(laList* list)

   Summary:
    Removes all nodes from a given list and frees the data of each node

   Parameters:
    laList* list - the list to modify
       
   Returns:
    void
*/
LIB_EXPORT void laList_Destroy(laList* list);

#endif /* LIBARIA_LIST_H */
