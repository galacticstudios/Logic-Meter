
// To enable this code, the linker needs these switches on the command line:
// --wrap malloc --wrap free

#include <stdint.h>
#include <stdlib.h>

void *__real_malloc(size_t);
void __real_free(void *);
void *__real_realloc(void *, size_t);

size_t largest, total;

void *__wrap_malloc(size_t sz)
{
    if (sz > largest)
        largest = sz;
    total += sz;
    return __real_malloc(sz);
}

void __wrap_free(void *p)
{
    __real_free(p);
}

void *__wrap_realloc(void *p, size_t sz)
{
    return __real_realloc(p, sz);
}

#ifdef REPLACE_HEAP

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#define HEAP_SIZE 100000

size_t heap[HEAP_SIZE / sizeof(size_t)] = {HEAP_SIZE};
typedef struct FreeHeader_s
{
    size_t sz; // size of the block includes the header and tail
    int inUse; // non-zero if block is in use
    struct FreeHeader_s *prev, *next;
} FreeHeader;
typedef struct
{
    size_t sz; // size of the block includes the header and tail
} Tail;

typedef struct
{
    size_t sz;
    int inUse;
} AllocatedHeader;

FreeHeader *freeList = (FreeHeader *) heap;

static void *replaceHeapMalloc(size_t sz)
{
    // Enlarge sz so we have room for the block header and tail
    sz += sizeof(AllocatedHeader) + sizeof(Tail);
    // Make sure sz is a multiple of 4 bytes
    sz = (sz + 3) & 0xfffffffc;
    
    // Find the first free block that's big enough
    FreeHeader *freeBlock = freeList;
    while (freeBlock && freeBlock->sz < sz)
        freeBlock = freeBlock->next;
    
    if (freeBlock == NULL)
    {
        __builtin_software_breakpoint();
        return (void *) 0;
    }
    
    // If we split the block, how big would the remainder be?
    size_t remainderSz = freeBlock->sz - sz;
    // If the free block is big enough to split
    if (remainderSz > sizeof(FreeHeader) + sizeof(Tail))
    {
        // Shrink the freeBlock
        freeBlock->sz = remainderSz;
        // Make a new tail for the freeBlock
        Tail *freeTail = (Tail *) ((uint8_t *) freeBlock + freeBlock->sz - sizeof(Tail));
        freeTail->sz = freeBlock->sz;
        // Initialize the allocated block's header and tail
        AllocatedHeader *allocated = (AllocatedHeader *) (freeTail + 1);
        allocated->sz = sz;
        allocated->inUse = 1;
        Tail *allocatedTail = (Tail *) ((uint8_t *) allocated + sz - sizeof(Tail));
        allocatedTail->sz = sz;
        
        // Return a pointer to the usable space
        return allocated + 1;
    }
    // Else (return the whole block)
    else
    {
        // Remove the block from the free list
        freeBlock->inUse = 1;
        // If there's a previous free block
        if (freeBlock->prev)
        {
            freeBlock->prev->next = freeBlock->next;
        }
        // Else (no previous free block) we must be the head of the list.
        // But not anymore
        else
            freeList = freeBlock->next;
        if (freeBlock->next)
            freeBlock->next->prev = freeBlock->prev;
        
        // Return a pointer to the usable space
        return (AllocatedHeader *) freeBlock + 1;
    }
}

static FreeHeader *AddToFreeList(AllocatedHeader *allocated)
{
    FreeHeader *freed = (FreeHeader *) allocated;
    freed->inUse = 0;
    freed->prev = NULL;
    freed->next = freeList;
    if (freeList)
        freeList->prev = freed;
    freeList = freed;
    return freed;
}

static void replaceHeapFree(void *p)
{
    if (p)
    {
        AllocatedHeader *allocated = (AllocatedHeader *) ((uint8_t *) p - sizeof(AllocatedHeader));
        Tail *allocatedTail = (Tail *) ((uint8_t *) allocated + allocated->sz - sizeof(Tail));
        if (!allocated->inUse)
            __builtin_software_breakpoint();
        
        FreeHeader *freed;
        
        // If we're not at the front of the heap
        if ((size_t *) allocated != heap)
        {
            // Get the tail of the previous block
            Tail *prevTail = (Tail *) allocated - 1;
            FreeHeader *prevBlock = (FreeHeader *) ((uint8_t *) allocated - prevTail->sz);
            // If the previous block is not in use
            if (!prevBlock->inUse)
            {
                // Coalesce
                prevBlock->sz += allocated->sz;
                allocatedTail->sz = prevBlock->sz;
                freed = (FreeHeader *) allocated;
            }
            // Else (previous block is in use)
            else
            {
                freed = AddToFreeList(allocated);
            }
        }
        // Else (we're the first memory location in the heap)
        else
            freed = AddToFreeList(allocated);
        
        // See if we can coalesce with the block after us
        FreeHeader *nextBlock = (FreeHeader *) ((uint8_t *) freed + freed->sz);
        if (!nextBlock->inUse)
        {
            // Combine the blocks
            Tail *nextTail = (Tail *) ((uint8_t *) nextBlock + nextBlock->sz - sizeof(Tail));
            freed->sz += nextBlock->sz;
            nextTail->sz = freed->sz;
            // Adjust the pointers
            if (nextBlock->prev)
                nextBlock->prev->next = nextBlock->next;
            if (nextBlock->next)
                nextBlock->next->prev = nextBlock->prev;
        }
    }
}

#endif // REPLACE_HEAP