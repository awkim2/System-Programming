/**
 * Malloc
 * CS 241 - Fall 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef struct memblock {
    int avi;
    size_t size_;
} memblock;


static void *memStart;
static void *lastAddr;
static int hasInit;

void init()
{
    lastAddr = sbrk(0);
    memStart = lastAddr; 
    hasInit = 1;
}

void slice_large_block(memblock* temp, size_t size){
    memblock* second_part = (void*) temp + size;
    second_part->size_ = temp->size_ - size;
    second_part->avi = 1;
    temp->size_ = size;
    temp->avi = 0;
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
	if (!hasInit) init();
	void *current = memStart;
	void *new_mem = NULL; 
	size += sizeof(memblock); 

	while (current != lastAddr) { 
		memblock *pcurrent = current; 
		if (pcurrent->avi && pcurrent->size_ >= size){
            if(pcurrent->size_ >= (size + 512)){
                slice_large_block(pcurrent, size);
            }
			pcurrent->avi = 0; 
			new_mem = current;
			break; 
		} 
		current += pcurrent->size_; 
	}
    // no enough size block need sbrk
	if (!new_mem) {
        if( *(int*) sbrk(size) == -1) return NULL;
		new_mem = lastAddr; 
		lastAddr += size; 
		memblock *pcb = new_mem; 
		pcb->size_ = size; 
		pcb->avi = 0; 
	}

	new_mem += sizeof(memblock); 
	return new_mem;
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
    memblock *pmcb = (memblock *)(ptr - sizeof(memblock));
    pmcb->avi = 1;

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
