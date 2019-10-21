/**
 * Password Cracker
 * CS 241 - Fall 2019
 */
#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "./includes/queue.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <stdlib.h>
#include <math.h>

typedef struct task {
  char* name;
  char* hash;
  char* known;
} task;
queue* q = NULL;
static int solve = 0;
static int fail= 0;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;


void* cryptFuct(void* id){
    struct crypt_data cdata;
    cdata.initialized = 0;
    int tid = (long) id;
    task *temp = NULL;
    //loop through all names in queue
    while((temp = queue_pull(q))){
        queue_push(q,NULL);
        int count = 0;
        int found = 0;
        v1_print_thread_start(tid, temp->name);
        double start = getThreadCPUTime();
        char *key = calloc(9,1);
        strcpy(key, temp->known);
        int remainlen = 8 - getPrefixLength(key);
        setStringPosition(key + getPrefixLength(key), 0);
        //loop through all possible password for one name
        while (1) {
            char* password = crypt_r(key, "xx", &cdata);
            count++;
            //found match hash 
            if (!strcmp(password, temp->hash)) {
                double time = getThreadCPUTime() - start;
                v1_print_thread_result(tid, temp->name, key, count, time, 0);
                found = 1;
                pthread_mutex_lock(&m1);
                solve++;
                pthread_mutex_unlock(&m1);
                break;
            }
            incrementString(key);
            //try all possible hash
            if(count == pow(26,remainlen)) break;
        }
        // failed finding solution, try next one
        if (!found){
            double timeusing = getThreadCPUTime() - start;
            v1_print_thread_result(tid, temp->name, NULL, count, timeusing, 1);
            pthread_mutex_lock(&m2);
            fail++;
            pthread_mutex_unlock(&m2);
            free(key);
            free(temp->name);
            free(temp->hash);
            free(temp->known);
            free(temp);
            continue;
        }
        // success finding solution, try next one
            free(key);
            free(temp->name);
            free(temp->hash);
            free(temp->known);
            free(temp);
        }
            return NULL;

}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    q = queue_create(-1);
    char* buffer = NULL;
    size_t len = 0;
    ssize_t nread;
    //push all lines in queue
    while(1){
        nread = getline(&buffer, &len, stdin);
        if(nread <= 0) break;
        if(nread>0 && buffer[nread -1] == '\n') buffer[nread-1] = '\0';
        task* new_task = malloc(sizeof(task));
        char* name = strtok(buffer, " ");
        char* hash = strtok(NULL, " ");
        char* known = strtok(NULL, " ");
        new_task->name = calloc(strlen(name)+1, 1);
        new_task->hash = calloc(strlen(hash)+1, 1);
        new_task->known = calloc(strlen(known)+1, 1);
        strcpy(new_task->name, name);
        strcpy(new_task->hash, hash);
        strcpy(new_task->known, known);
        queue_push(q, new_task);
    }
    free(buffer);
    buffer = NULL;
    pthread_t thread_num[thread_count];
    size_t i = 0;
    size_t j = 0;
    for(; i < thread_count; i++){
        pthread_create(thread_num + i, NULL, cryptFuct, (void*)i+1);
        //finish create thread, and join all threads
        if(i == thread_count - 1){
            for(; j < thread_count; j++){
                pthread_join(thread_num[j], NULL);
            }
        }
    }
    queue_destroy(q);
    pthread_mutex_destroy(&m1);
    pthread_mutex_destroy(&m2);  
    v1_print_summary(solve, fail); 
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
