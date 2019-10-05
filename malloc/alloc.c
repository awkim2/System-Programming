/**
 * Malloc
 * CS 241 - Fall 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BLOCK_SIZE sizeof(memblock)

typedef struct memblock {
    int free;
    size_t size;
    struct memblock *prev;
    struct memblock *next;
}memblock;

static void *heap_begin = NULL;
static size_t total_memory_used = 0;
static size_t total_memory_sbrk = 0; 

void get_heap_bottm(){
    if(!heap_begin) heap_begin = sbrk(0);
}

void *get_last_block(){
    memblock *temp;
    temp = heap_begin;
    while(temp->next){
        temp = temp->next;
    }
    return temp;
}

void *find_fit_block(size_t size){
    if(total_memory_sbrk - total_memory_used >= size){
    memblock *temp;
    temp = heap_begin;
    while(temp){
        if(temp->free && temp->size >= size) break;
        temp = temp->next;
    }
    return temp;
    }
    return NULL;
}

void *sbrk_heap(void *prev, size_t size){
    void *new_mem;
    memblock *new_meta, *prev_meta;
    new_mem = sbrk(size + BLOCK_SIZE);
    if(!new_mem) return NULL;
    new_meta = new_mem;
    total_memory_sbrk += size + BLOCK_SIZE;
    total_memory_used += size + BLOCK_SIZE;
    new_meta->free = 0;
    new_meta->size = size;
    new_meta->prev = prev;
    new_meta->next = NULL;
    if(prev){
        prev_meta = prev;
        prev_meta->next = new_meta;
    }
    return new_mem + BLOCK_SIZE;
}

void split_block(memblock *blk_meta, size_t size){
    memblock *sub_block;
    if(size + BLOCK_SIZE < blk_meta->size){
        sub_block = blk_meta + size + BLOCK_SIZE;
        sub_block->prev = blk_meta;
        sub_block->next = blk_meta->next;
        blk_meta->next->prev = sub_block;
        sub_block->size = blk_meta->size - size - BLOCK_SIZE;
        sub_block->free = 1;

        blk_meta->size = size;
        blk_meta->free = 0;
        blk_meta->next = sub_block;

        total_memory_used -= sub_block->size;
    }
    else{
        blk_meta->free = 0;
    }
}

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    void* new_mem = malloc(num*size);
    if(new_mem){
         memset(new_mem + sizeof(memblock), 0, num*size);
    }
    return new_mem;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
    // implement malloc!
    void *split_blk, *cur_brk, *ptr;
    memblock *last_blk;

    if(!size)
        return NULL;

    // if(size & 0x7){
    //     size = ((size >> 3) + 1) << 3;
    // }

    get_heap_bottm();
    cur_brk = sbrk(0);
    if(heap_begin == cur_brk){
        ptr = sbrk_heap(NULL, size);
        if(ptr)
            return ptr;
        else
            return NULL;
    }
    else{
        if(total_memory_sbrk - total_memory_used >= size){
            split_blk = find_fit_block(size);
            if(split_blk){
                split_block(split_blk, size);
                ptr = split_blk + BLOCK_SIZE;
                return ptr;
            }else{
                last_blk = get_last_block();
                ptr = sbrk_heap(last_blk, size);
                if(ptr){
                    return ptr;
                }
                else{
                    return NULL;
                }
            }
        }else{
            last_blk = get_last_block();
            ptr = sbrk_heap(last_blk, size);
            if(ptr){
                return ptr;
            }
            else{
                return NULL;
            }
        }
    }
}
/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    // implement free!
    memblock *curr, *prev_block, *next_block;

    if(!ptr) return;

    curr = ptr - BLOCK_SIZE;
    curr->free = 1;
    total_memory_used -= curr->size;
    next_block = curr->next;
    if(next_block){
        if(next_block->free){
            curr->next = next_block->next;
            curr->size += next_block->size + BLOCK_SIZE;
        }
    }

    prev_block = curr->prev;
    if(prev_block){
        if(prev_block->free){
            prev_block->next = curr->next;
            prev_block->size += curr->size + BLOCK_SIZE;
        }
    }

}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    // implement realloc!
    if(!ptr) return malloc(size);
    if(size == 0){
        free(ptr);
        return NULL;
    }
    void* new_mem = malloc(size);
    if(!new_mem) return NULL;

    return new_mem;
}
