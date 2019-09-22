/**
 * Mini Memcheck
 * CS 241 - Fall 2019
 * partner mengz5 ruoyuz3
 */
#include "mini_memcheck.h"
#include <stdio.h>
#include <string.h>

meta_data * head = NULL;
size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    if(request_size == 0){
        return NULL;
    }
    meta_data* metadata = (meta_data*) malloc(sizeof(meta_data) + request_size);
    if(!metadata){
        return NULL;
    }
    metadata->request_size = request_size;
    metadata->filename = filename;
    metadata-> instruction = instruction;
    metadata-> next = NULL;
    if(!head){
        head = metadata;
    } else{
        metadata->next = head;
        head = metadata;
    }
    total_memory_requested += request_size;
    return (void*)(metadata+1);
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
    size_t total_space = num_elements * element_size;
    void* calloc_space = mini_malloc(total_space, filename, instruction);
    if(calloc_space){
        memset(calloc_space,0,total_space);
    }
    return calloc_space;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
    
    if(!payload){
        void* result = mini_malloc(request_size, filename, instruction);
        return result;
    }
    if(!request_size){
        mini_free(payload);
        return NULL;
    }
    meta_data* meta_start = ((meta_data*)payload -1);
    meta_data* cur = head;
    while(cur){
        if(cur == meta_start){
            break;
        }
        cur = cur->next;
    }
    if(!cur){
        invalid_addresses ++;
        return NULL;
    }
    size_t old_size = cur -> request_size;
    meta_data* new_meta = realloc(cur, request_size+ sizeof(meta_data));
    cur -> request_size = request_size;
    cur ->filename = filename;
    cur->instruction = instruction;

    ssize_t size_diff = request_size - old_size;
    if(size_diff < 0){
        total_memory_freed -=  size_diff;
        
    } else{
       total_memory_requested += size_diff;
    }
    return (void*)new_meta + 1;
}

void mini_free(void *payload) {
    // your code here
    
    if(!payload){
        return;
    }
    if(!head){
        invalid_addresses++;
        return;
    }
    meta_data * meta_start = ((meta_data*) payload - 1);
    
    meta_data* prev = head;
    meta_data* cur = head;
    if(cur == meta_start){
        total_memory_freed += cur -> request_size;
        if(head == cur){
            head = cur -> next;
        } 
        else{
            prev -> next = cur -> next;
        }
        free(cur);
    } else{
    cur = cur->next;
    while(cur!=meta_start){
        cur = cur->next;
        prev = prev -> next;
    }
    if(!cur){
        invalid_addresses++;
        return;
    } else{
        total_memory_freed += cur -> request_size;
        if(head == cur){
            head = cur -> next;
        } 
        else{
            prev -> next = cur -> next;
        }
        free(cur);
    }
    }
}

