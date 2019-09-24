/**
 * Mini Memcheck
 * CS 241 - Fall 2019
 * partner mengz5 ruoyuz3
 */
#include "mini_memcheck.h"
#include <stdio.h>
#include <string.h>

meta_data* head = NULL;
size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    if(!request_size) return NULL;
    meta_data* metadata = (meta_data*) malloc(sizeof(meta_data) + request_size);
    if(!metadata) return NULL;
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
    size_t size = num_elements * element_size;
    void* calloc_space = mini_malloc(size, filename, instruction);
    if(calloc_space){
        memset(calloc_space,0,size);
    }
    return calloc_space;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
    if(payload == NULL){
        void* temp = mini_malloc(request_size, filename, instruction);
        return temp;
    }
    if(request_size == 0){
        mini_free(payload);
        return NULL;
    }
    meta_data* meta = ((meta_data*)payload -1);
    meta_data *prev = NULL;
    meta_data *cur = head;

    while (cur) {
        if (cur == meta) {
            if (request_size == cur->request_size) {
                meta->filename = filename;
                meta->instruction = instruction;
                return payload;
            }

            if (prev) {
                prev->next = cur->next;
            } else {
                head = cur->next;
            }
            if (request_size > cur->request_size) {
                total_memory_requested += request_size - cur->request_size;
            } else {
               total_memory_freed  += cur->request_size - request_size;
            }
            void *new_mem = realloc(cur, sizeof(meta_data) + request_size);
            if (!new_mem) return NULL;

            meta_data *metadata = (meta_data *) new_mem;
            void *res  = new_mem + sizeof(meta_data);
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
            return res;
        }
        prev = cur;
        cur = cur->next;
    }
    invalid_addresses++;
    return NULL;
}


void mini_free(void *payload) {
    // your code here
    if(!payload) return;
    meta_data* meta = ((meta_data*)payload -1);
    meta_data* this = head;
    meta_data* prev = NULL;
    while(this){
        if(this == meta){
            if(prev){
                prev->next = this->next;
            }else{
                head = this->next;
            }
            total_memory_freed += this->request_size;
            free(this);
            return;
        }
        prev = this;
        this = this->next;
    }
    invalid_addresses++;

}




