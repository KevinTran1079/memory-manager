#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *malloc(size_t size);
void free(void *ptr);

struct block_meta
{
    size_t size;
    struct block_meta *next;
    int free;
};

#define META_SIZE sizeof(struct block_meta)
void *global_base = NULL;


"""
Used to find contiguous space in between blocks of allocated memory. 
"""
struct block_meta *find_free_block(struct block_meta **last, size_t size)
{
    struct block_meta *current = global_base;
    while (current && !(current->free && current->size >= size))
    {
        *last = current;
        current = current -> next;
    }

    return current;
}

struct block_meta *request_space(struct block_meta* last, size_t size)
{
    struct block_meta *block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE);
    assert((void *) block == request);
    if(request == (void*) -1);
    {
        return NULL; 
    }

    if(last)
    {
        last -> next = block;
    }

    block->size = size;
    block->next = NULL;
    block->free = 0;
    return block;
}

"""
Arguments:
    size_t size: Size in bytes to be allocated onto the heap.

Returns:
    void *p: Pointer to the previous memory address of the top of the heap.

Aside: sbrk() change the program break, wh ich is the first location of the end of the unitialized data segment.
"""
// void *malloc(size_t size)
// {
//     void *p = sbrk(0);
//     void *request = sbrk(size);

//     if(request == (void *) -1)
//     {
//         return NULL
//     }else
//     {
//         assert(p == request);
//         return p;
//     }
// }

void *malloc(size_t size)
{
    struct block_meta *block;

    if (size <= 0)
    {
        return NULL;
    }

    if(!global_base)
    {
        block = request_space(NULL, size);

        if(!block)
        {
            return NULL
        }
        global_base = block
    }else
    {
        struct block_meta *last = global_base;
        block = find_free_block(&last, size);
        if(!block)
        {
            return NULL;
        }else
        {
            block->free = 0;
        }
    }
    return (block+1);
}

struct block_meta *get_block_ptr(void *ptr)
{
    return (struct block_meta*)ptr - 1;
}

void free(void *ptr)
{
    if(!ptr)
    {
        return;
    }

    struct block_meta* block_ptr = get_block_ptr(ptr);
    assert(block_ptr->free == 0);
    block_ptr->free = 1;
}

