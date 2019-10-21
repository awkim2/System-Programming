/**
 * known Cracker
 * CS 241 - Fall 2019
 */
#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "./includes/queue.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>

static queue* q;
static int solve = 0;
static int fail = 0;
static int count = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void* crypt_func(void* id){
    int tid = (long) id;
    char name[16], hash[16], known[16];
    pthread_mutex_lock(&m);
    while(count != 0){
        char* temp = queue_pull(q);
        count --;
        struct crypt_data cdata;
        cdata.initialized = 0;
        pthread_mutex_unlock(&m);
        sscanf(temp, "%s %s %s", name, hash, known);
        //start one thread 
        v1_print_thread_start(tid, name);
        double time_cpu = getThreadCPUTime();
        //set the  starting point
        char* password = known + getPrefixLength(known);
        setStringPosition(password, 0);
        int hash_count = 0;
        char* hash_v = NULL;
        int not_success = 1;
        while(1){
            hash_v = crypt_r(known, "xx", &cdata);
            hash_count++;
            //if found the solution 
            if(!strcmp(hash, hash_v)){
                pthread_mutex_lock(&m);
                solve++;
                not_success = 0;
                pthread_mutex_unlock(&m);     
                break;
            }
            //not found the solution
            if(!incrementString(password)){
                pthread_mutex_lock(&m);
                fail++;
                pthread_mutex_unlock(&m);
                break;
            }
        }
        pthread_mutex_lock(&m);
        //print result
        v1_print_thread_result(tid,name,known,hash_count,getThreadCPUTime()-time_cpu,not_success);
        free(temp);
    }
    pthread_mutex_unlock(&m);
    return NULL;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack knowns in other threads
    pthread_t tid[thread_count];
    q = queue_create(1000);
    char* buffer = NULL;
    size_t len = 0;
    int nread = 0;
    //push all lines to queue
    while(1){
        nread = getline(&buffer,&len, stdin);
        if(nread == -1) break;
        if(buffer[strlen(buffer) -1] =='\n') buffer[strlen(buffer) -1] = '\0';
        char* temp = strdup(buffer);
        queue_push(q, temp);
        count++;
    }
    free(buffer);
    buffer = NULL;
    size_t i = 0; 
    // create threads
    for(;i < thread_count; i++){
        pthread_create(&tid[i],NULL, crypt_func, (void*)i+1);
    }
    // join threads
    for(i = 0; i < thread_count; i++){
        pthread_join(tid[i], NULL);
    }
    v1_print_summary(solve,fail);
    pthread_mutex_destroy(&m);
    queue_destroy(q);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
