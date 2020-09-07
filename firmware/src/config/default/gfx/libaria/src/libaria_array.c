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


#include "gfx/libaria/inc/libaria_array.h"

#include "gfx/libaria/inc/libaria_context.h"

#define STD_RESIZE_AMT   5

static void _shuffleRight(laArray* arr, uint32_t idx)
{
    uint32_t i;

    if(arr->size == 0)
        return;

    if(arr->size < arr->capacity)
        arr->size++;

    for(i = arr->size - 1; i >= idx; i++)
        arr->values[i+1] = arr->values[i];

    arr->values[i] = NULL;
}

static void _shuffleLeft(laArray* arr, uint32_t idx)
{
    uint32_t i;

    if(arr->size == 0)
        return;

    for(i = idx; i < arr->size; i++)
        arr->values[i] = arr->values[i+1];

    arr->values[i-1] = NULL;

    arr->size--;
}

laResult laArray_Create(laArray* arr)
{
    if(arr == NULL)
        return LA_FAILURE;

    arr->values = NULL;
    arr->size = 0;
    arr->capacity = 0;

    return LA_SUCCESS;
}

laResult laArray_Resize(laArray* arr, uint32_t sz)
{
    if(laContext_GetActive() == NULL || arr == NULL || arr->capacity == sz)
        return LA_FAILURE;

    arr->values = laContext_GetActive()->memIntf.heap.realloc(arr->values, 
                                                              sizeof(void*) * sz);

    if(arr->values == NULL)
    {
        arr->size = 0;
        arr->capacity = 0;

        return LA_FAILURE;
    }

    arr->capacity = sz; 

    if(arr->size >= arr->capacity)
        arr->size = arr->capacity;

    return LA_SUCCESS;
}

laResult laArray_PushFront(laArray* arr, void* val)
{
    if(laContext_GetActive() == NULL ||
       arr == NULL ||
       arr->size == 0 ||
       val == NULL)
        return LA_FAILURE;

    if(arr->size == arr->capacity)
    {
        if(laArray_Resize(arr, arr->capacity + STD_RESIZE_AMT) == LA_FAILURE)
            return LA_FAILURE;
    }

    _shuffleRight(arr, 0);

    arr->values[0] = val;

    return LA_SUCCESS;
}

laResult laArray_PopFront(laArray* arr)
{
    if(arr == NULL || arr->size == 0)
        return LA_FAILURE;

    _shuffleLeft(arr, 0);

    return LA_SUCCESS;
}

laResult laArray_PushBack(laArray* arr, void* val)
{
    if(laContext_GetActive() == NULL ||
       arr == NULL ||
       val == NULL)
        return LA_FAILURE;

    if(arr->size == arr->capacity)
    {
        if(laArray_Resize(arr, arr->capacity + STD_RESIZE_AMT) == LA_FAILURE)
            return LA_FAILURE;
    }

    arr->values[arr->size] = val;
    arr->size++;

    return LA_SUCCESS;
}

laResult laArray_PopBack(laArray* arr)
{
    if(arr == NULL || arr->size == 0)
        return LA_FAILURE;

    arr->size--;

    return LA_SUCCESS;
}

laResult laArray_InsertAt(laArray* arr,
                                uint32_t idx,
                                void* val)
{
    if(laContext_GetActive() == NULL ||
       arr == NULL ||
       idx > arr->size ||
       val == NULL)
        return LA_FAILURE;

    if(idx == arr->size)
        return laArray_PushBack(arr, val);

    if(arr->size == arr->capacity)
    {
        if(laArray_Resize(arr, arr->capacity + STD_RESIZE_AMT) == LA_FAILURE)
            return LA_FAILURE;
    }

    _shuffleRight(arr, idx);

    arr->values[idx] = val;

    return LA_SUCCESS;
}

laResult laArray_RemoveAt(laArray* arr, uint32_t idx)
{
    if(laContext_GetActive() == NULL ||
       arr == NULL ||
       arr->size == 0 ||
       idx >= arr->size)
        return LA_FAILURE;

    _shuffleLeft(arr, idx);

    return LA_SUCCESS;
}

laResult laArray_Remove(laArray* arr, void* val)
{
    uint32_t idx;

    if(arr == NULL || arr->size == 0 || val == NULL)
        return LA_FAILURE;

    idx = laArray_Find(arr, val);

    if(idx == -1)
        return LA_FAILURE;

    _shuffleLeft(arr, idx);

    return LA_SUCCESS;
}

void* laArray_Get(laArray* arr, uint32_t idx)
{
    if (idx > arr->size)
        return NULL;

    return arr->values[idx];
}

int32_t laArray_Find(laArray* arr, void* val)
{
    uint32_t i;

    if(arr == NULL || arr->size == 0 || val == NULL)
        return LA_FAILURE;

    for(i = 0; i < arr->size; i++)
    {
        if(arr->values[i] == val)
            return i;
    }

    return -1;
}

laResult laArray_Copy(laArray* src, laArray* dest)
{
    if(laContext_GetActive() == NULL || src == NULL || dest == NULL)
        return LA_FAILURE;

    if(dest->capacity < src->size)
    {
        if(laArray_Resize(dest, src->size) == LA_FAILURE)
            return LA_FAILURE;
    }

    dest->size = src->size;

    if(dest->size == 0)
        return LA_SUCCESS;

    laContext_GetActive()->memIntf.heap.memcpy(dest->values,
                                               src->values,
                                               src->size * sizeof(laWidget*));
    
    return LA_SUCCESS;
}

laResult laArray_Clear(laArray* arr)
{
    if(arr == NULL)
        return LA_FAILURE;

    arr->size = 0;

    return LA_SUCCESS;
}

laResult laArray_Destroy(laArray* arr)
{
    uint32_t i;

    if(laContext_GetActive() == NULL || arr == NULL)
        return LA_FAILURE;

    for(i = 0; i < arr->size; i++)
        laContext_GetActive()->memIntf.heap.free(arr->values[i]);

    if(arr->values != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(arr->values);
        arr->values = NULL;
    }
    
    arr->capacity = 0;
    arr->size = 0;

    return LA_SUCCESS;
}

laResult laArray_Free(laArray* arr)
{
    uint32_t i;

    if(laContext_GetActive() == NULL || arr == NULL)
        return LA_FAILURE;

    for(i = 0; i < arr->size; i++)
    {
        laContext_GetActive()->memIntf.heap.free(arr->values[i]);
        arr->values[i] = NULL;
    }

    arr->size = 0;

    return LA_SUCCESS;
}

