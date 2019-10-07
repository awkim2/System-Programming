/**
 * Malloc
 * CS 241 - Fall 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
typedef struct meta_block{
    void* ptr;
    size_t size;
    int free;
    struct meta_block *next;
    struct meta_block *prev;
}meta_block;

static size_t mem_free = 0;
static meta_block *head = NULL;

#define BLOCK_SIZE  sizeof(meta_block) 
void merge(meta_block* temp){
    temp->size += temp->prev->size + BLOCK_SIZE;
    temp->prev = temp->prev->prev;
    if (temp->prev) {
      temp->prev->next = temp;
    }else {
      head = temp;
    }
}

void split_space(meta_block* temp, size_t size){
    meta_block* new_space = temp->ptr + size;
    new_space->ptr = new_space + 1;
    new_space->size = temp->size - size - BLOCK_SIZE;
    mem_free += new_space->size;
    new_space->free = 1;
    temp->size = size;
    new_space->next = temp;
    new_space->prev = temp->prev;
    if(temp->prev){
        temp->prev->next = new_space;
    }else{
        head = new_space;
    }
    temp->prev = new_space;
    if(new_space->prev && new_space->prev->free) merge(new_space);
}


void *calloc(size_t num, size_t size) {
    void* temp = malloc(num*size);
    if (!temp) return NULL;
    memset(temp, 0, num*size);
    return temp;
}

void *malloc(size_t size) {
    meta_block* curr = head;
    meta_block* res = NULL;

    // find the fit block
    if(mem_free >= size){
        while(curr){
            if(curr->free && curr->size >= size){
                mem_free -= size;
                res = curr;
                if((res->size - size >= size) && (res->size - size >= BLOCK_SIZE)) split_space(res,size);
                break;
            }
            curr = curr->next;
        }
    }

    if(res){
        // block found
        res->free = 0;
        return res->ptr;
    }
        // not found
    res = sbrk(0);
    if(sbrk(BLOCK_SIZE + size) == (void*) -1) return NULL;
    res->ptr = res + 1;
    res->next = head;
    if(head){
        res->prev = head->prev;
        head->prev = res;
    }else{
        //first malloc
        res->prev = NULL;
    }
    head = res;
    res->size = size;
    res->free = 0;
    return res->ptr;
}

void free(void *ptr) {
    if (!ptr) return;
    meta_block* temp = ptr - BLOCK_SIZE;
    if(temp->free) return;
    temp->free = 1;
    mem_free += (temp->size + BLOCK_SIZE);

    if(temp->prev && temp->prev->free == 1) merge(temp);
    if(temp->next && temp->next->free == 1) merge(temp->next);
}

void *realloc(void *ptr, size_t size) {
    // implement realloc!
    if(!ptr) return malloc(size);
    meta_block* temp = ptr - BLOCK_SIZE;
    assert(temp->ptr == ptr);
 
    if(size == temp->size) return ptr;

    if(size > temp->size){
        if(temp->prev && temp->prev->free && temp->size + temp->prev->size +sizeof(malloc) >= size){
            merge(temp);
            split_space(temp,size);
            return temp->ptr;
        }else{
            void* new_temp = malloc(size);
            memcpy(new_temp, temp->ptr, temp->size);
            free(ptr);
            return new_temp;
        }
    }else{
        if(temp->size - size >= BLOCK_SIZE){
            split_space(temp,size);
            return temp->ptr;
        }else{
            void* new_temp = malloc(size);
            memcpy(new_temp, ptr, size);
            free(ptr);
            return new_temp;
        }
    }
}
