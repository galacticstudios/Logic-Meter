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


#include "gfx/libaria/inc/libaria_list.h"

#include "gfx/libaria/inc/libaria_context.h"

int32_t laList_Create(laList* l)
{
    if(l == NULL)
        return -1;

    l->head = NULL;
    l->tail = NULL;

    l->size = 0;

    return 0;
}

int32_t laList_Assign(laList* l, size_t idx, void* val)
{
    laListNode* node;
    size_t pos;

    if(l == NULL || val == NULL || idx >= l->size)
        return -1;
    
    /* iterate through the nodes for the desired idx */
    node = l->head;

    for(pos = 0; pos < idx; pos++)
        node = node->next;

    /* assign the new val to the node */
    node->val = val;

    return 0;
}
    
int32_t laList_PushFront(laList* l, void* val)
{
    laListNode* node;
    void* ptr;

    if(laContext_GetActive() == NULL || l == NULL || val == NULL)
        return -1;

    /* create a new node */
    ptr = laContext_GetActive()->memIntf.heap.malloc(sizeof(laListNode));

    if(ptr == NULL)
        return -1;

    node = (laListNode*)ptr;

    node->next = NULL;
    node->val = val;

    /* if list is empty just insert */
    if(l->size == 0)
    {
        l->head = node;
        l->tail = node;
    }
    /* insert into list */
    else
    {
        /* set node's next to _head */
        node->next = l->head;

        /* set _head to node */
        l->head = node;                    
    }

    l->size++;

    return 0;
}

void laList_PopFront(laList* l)
{
    laListNode* n;

    if(laContext_GetActive() == NULL || l == NULL)
        return;

    if(l->size == 0)
        return;

    /* clear the list */
    if(l->size == 1)
    {
        laContext_GetActive()->memIntf.heap.free(l->head);

        l->head = NULL;
        l->tail = NULL;

        l->size = 0;
    }
    /* remove the head */
    else
    {
        n = l->head;

        /* set the new head */
        l->head = n->next;
        
        /* free the old head */
        laContext_GetActive()->memIntf.heap.free(n);
        
        l->size--;
    }
}

int32_t laList_PushBack(laList* l, void* val)
{
    laListNode* node;
    void* ptr;

    if(laContext_GetActive() == NULL || l == NULL)
        return -1;

    /* create a new node */
    ptr = laContext_GetActive()->memIntf.heap.malloc(sizeof(laListNode));

    if(ptr == NULL)
        return -1;

    node = (laListNode*)ptr;

    node->next = NULL;
    node->val = val;

    /* if list is empty just insert */
    if(l->size == 0)
    {
        l->head = node;
        l->tail = node;
    }
    /* insert into list */
    else
    {
        /* set tail's next to node */
        l->tail->next = node;                

        /* set tail to node */
        l->tail = node;
    }

    l->size++;

    return 0;
}

void* laList_Get(laList* l, uint32_t idx)
{
    laListNode* node;
    uint32_t count = 0;
    
    if(l == NULL || idx >= l->size)
        return NULL;

    node = l->head;

    while(count < idx)
    {
        node = node->next;
        count++;                
    }

    return node->val;
}

int32_t laList_Find(laList* l, void* val)
{
    laListNode* node;
    int32_t idx = 0;
    
    if(l == NULL || val == NULL)
        return LA_FAILURE;

    node = l->head;

    while(node != NULL)
    {
        if(node->val == val)
            return idx;
        
        node = node->next;
        idx++;                
    }

    return LA_FAILURE;
}

int32_t laList_InsertAt(laList* l,
                        void* val,
                        uint32_t idx)
{
    laListNode* slot = NULL;
    laListNode* node = NULL;
    laListNode* prev = NULL;
    void* ptr;
    uint32_t count = 0;

    if(laContext_GetActive() == NULL || 
       l == NULL ||
       val == NULL ||
       idx >= l->size)
        return -1;

    if(l->size == 0)
    {
        return laList_PushBack(l, val);
    }
    
    if(idx == 0)
    {
        return laList_PushFront(l, val);
    }
    
    // find existing node
    slot = l->head;

    while(count < idx)
    {
        prev = slot;
        slot = slot->next;
        count++;                
    }
    
    if (slot == NULL)
    {
        return laList_PushBack(l, val);
    }
    
    /* create a new node */
    ptr = laContext_GetActive()->memIntf.heap.malloc(sizeof(laListNode));

    if(ptr == NULL)
        return -1;

    node = (laListNode*)ptr;

    /* copy existing node */
    //*node = *slot;

    /* store new value */
    node->val = val;
    
    if(prev != NULL)
        prev->next = node;
    
    node->next = slot;
    l->size++;

    return 0;
}

int32_t laList_Remove(laList* l, void* val)
{
    laListNode* node = NULL;
    laListNode* prev = NULL;

    if(laContext_GetActive() == NULL || l == NULL || l->size == 0)
        return -1;
   
    node = l->head;
    
    /* find the node that matches val */
    while(node != NULL)
    {
        if(node->val == val)
            break;

        prev = node;
        node = node->next;
    }

    /* didn't find the value */
    if(node == NULL)
        return -1;

    /* remove the head node */
    if(node == l->head)
    {
        l->head = node->next;
    }
    /* remove the tail node */
    else if(node == l->tail)
    {
        l->tail = prev;
        l->tail->next = NULL;
    }
    else
    {
        prev->next = node->next;
    }

    laContext_GetActive()->memIntf.heap.free(node);
    l->size--;

    return 0;
}

int32_t laList_RemoveAt(laList* l, uint32_t idx)
{
    laListNode* node = NULL;
    laListNode* prev = NULL;
    uint32_t count = 0;
    
    if(laContext_GetActive() == NULL ||
       l == NULL ||
       l->size == 0 ||
       idx >= l->size)
        return LA_FAILURE;

    node = l->head;

    while(count < idx)
    {
        prev = node;
        node = node->next;
        count++;                
    }

    /* remove the head node */
    if(node == l->head)
    {
        l->head = node->next;
    }
    /* remove the tail node */
    else if(node == l->tail)
    {
        l->tail = prev;
        l->tail->next = NULL;
    }
    else
    {
        prev->next = node->next;
    }

    laContext_GetActive()->memIntf.heap.free(node);
    l->size--;

    return 0;
}

int32_t laList_Copy(laList* dest, laList* src)
{
    laListNode *l, *r;
    void* ptr;

    if(laContext_GetActive() == NULL || dest == NULL || src == NULL)
        return -1;

    if(dest->size != 0)
        laList_Clear(dest);

    dest->size = src->size;

    r = src->head;
    l = NULL;

    while(r != NULL)
    {
        /* create a new node */
        ptr = laContext_GetActive()->memIntf.heap.malloc(sizeof(laListNode));

        if(ptr == NULL)
            return -1;

        l = (laListNode*)ptr;
    
        l->val = r->val;
        l->next = NULL;
        
        if(dest->head == NULL)
        {
            dest->head = l;
            dest->tail = l;
        }
        else
        {
            dest->tail->next = l;
            dest->tail = l;
        }

        r = r->next;
    }

    return 0;
}

void laList_Clear(laList* l)
{
    laListNode *node, *next;
    
    if(laContext_GetActive() == NULL || l == NULL)
        return;

    node = l->head;

    /* iterate through the nodes and delete */
    while(node != NULL)
    {
        next = node->next;
        
        laContext_GetActive()->memIntf.heap.free(node);       
        
        node = next;
    }

    l->size = 0;

    l->head = NULL;
    l->tail = NULL;
}

void laList_Destroy(laList* l)
{
    laListNode *node, *next;
    
    if(laContext_GetActive() == NULL || l == NULL)
        return;

    node = l->head;

    /* iterate through the nodes and delete */
    while(node != NULL)
    {
        next = node->next;
        
        laContext_GetActive()->memIntf.heap.free(node->val);
        laContext_GetActive()->memIntf.heap.free(node);        
        
        node = next;
    }

    l->size = 0;

    l->head = NULL;
    l->tail = NULL;
}

