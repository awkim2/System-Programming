/**
 * Teaching Threads
 * CS 241 - Fall 2019
 * partner mengz5 ruoyuz3
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

/* You might need a struct for each task ... */
typedef struct thread_data
{
    int* list;
    reducer reduce_func;
    size_t length;
    int base_case;
}thread_data;


/* You should create a start routine for your threads. */
void* reduce_routine(void* data){
    thread_data* new_data = (thread_data*)data;
	int result = new_data->base_case;
	for (size_t i = 0; i < new_data->length; ++i) {
        result = new_data->reduce_func(result, new_data->list[i]);
    }
    int * res = malloc(sizeof(int));
    *res = result;
    return (void*) res;
}


int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
    if(list_len<= num_threads) return reduce(list,list_len,reduce_func,base_case);
    pthread_t threads[num_threads];
    int new_list[num_threads];
    int split_num = list_len / num_threads;
    thread_data* t_data[num_threads];
    size_t i = 0;
    for(; i < num_threads-1; i++){
        t_data[i] = malloc(sizeof(thread_data));
        t_data[i] -> list = list + (i*split_num);
        t_data[i] -> reduce_func = reduce_func;
        t_data[i] -> length = split_num;
        t_data[i] -> base_case = base_case;
    }
    t_data[i] = malloc(sizeof(thread_data));
    t_data[i] -> list = list + (i*split_num);
    t_data[i] -> reduce_func = reduce_func;
    t_data[i] -> length = list_len - (i*split_num);
    t_data[i] -> base_case = base_case;
    for(size_t j = 0; j < num_threads;j++){
        pthread_create(&threads[j],0,reduce_routine,(void*)t_data[j]);
    }
    for(size_t k = 0; k < num_threads; k++){
        void* result;
        pthread_join(threads[k],&result);
        new_list[k] = *((int*)result);
        free(result);
        free(t_data[k]);
    }
    int final_ = reduce(new_list,num_threads,reduce_func,base_case);
    return final_;
}
