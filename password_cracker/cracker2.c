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
  char *name;
  char *hash;
  char *known;
} task;
static task *temp = NULL;

static int finshed = 0;
static int thread_number = 0;
static int found = 0;
static char* final_result = NULL;
static int count_all = 0;

pthread_barrier_t m_barrier;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *thread_fun(void* id){
    struct crypt_data input;
    input.initialized = 0;
    int tid = (long) id;
    char *password = calloc(10, sizeof(char));
    while(1){
        pthread_barrier_wait(&m_barrier);
        if(finshed) break;
        strcpy(password, temp->known);
        int count_t = 0;
        long start_index = 0;
        long count = 0;
        //divide task for threads and move then to correct start point
        int remain = strlen(password) - getPrefixLength(password);
        getSubrange(remain, thread_number, tid, &start_index, &count);
        setStringPosition(password + getPrefixLength(password), start_index);
        v2_print_thread_start(tid, temp->name, start_index, password);
        long i = 0;
        for (; i < count; i++){
            char *hash_v = crypt_r(password, "xx", &input);
            count_t++;
            //success thread print result, add count and then exit loop
            if(!strcmp(hash_v, temp->hash)){
                pthread_mutex_lock(&m);
                strcpy(final_result, password);
                found = 1;
                v2_print_thread_result(tid, count_t, 0);
                count_all = count_all + count_t;
                pthread_mutex_unlock(&m);
                break;
            }
            //other thread success, print result, add count and then exit loop
            if(found){
                pthread_mutex_lock(&m);
                v2_print_thread_result(tid, count_t, 1);
                count_all = count_all + count_t;
                pthread_mutex_unlock(&m);
                break;
            }
            // continue searching 
            incrementString(password);
        }

        pthread_barrier_wait(&m_barrier);
        // No one found the key
        if(!found){
            pthread_mutex_lock(&m);
            v2_print_thread_result(tid, count_t, 2);
            count_all = count_all + count_t;
            pthread_mutex_unlock(&m);
        }
        pthread_barrier_wait(&m_barrier);
    }
    free(password);
    return NULL;
}


 
int start(size_t thread_count) {
    pthread_barrier_init(&m_barrier, NULL, thread_count+1);
    thread_number = thread_count;
    pthread_t ptd[thread_count];
    //create thread
    for (size_t i = 0; i < thread_count; i++){
        pthread_create(ptd+i, NULL, thread_fun, (void*)(i+1));
    }
    //create task
    temp = calloc(sizeof(task),1);
    temp->name = calloc(10, sizeof(char));
    temp->hash = calloc(15, sizeof(char));
    temp->known = calloc(10,sizeof(char));

    final_result = calloc(10,sizeof(char));

    //get one task per line
    char *buffer = NULL;
    size_t len = 0;
    ssize_t nread = 0;
    while (1){   
        nread = getline(&buffer, &len, stdin);
        //file reach the end 
        if(nread == -1){
            finshed = 1;
            free(temp->hash);
            free(temp->known);
            free(temp->name);
            free(temp);
            free(final_result);
            free(buffer);
            pthread_barrier_wait(&m_barrier);
            break;
        }
        
        //create task
        if (buffer[nread-1] == '\n') buffer[nread-1] = '\0';
        char *name = strtok(buffer, " ");
        char *hash = strtok(NULL, " ");
        char *known = strtok(NULL, " ");
        strcpy(temp->name, name);
        strcpy(temp->hash, hash);
        strcpy(temp->known, known);

        v2_print_start_user(temp->name);
        //start time
        double start = getTime();
        double start_cpu = getCPUTime();
        //barrier wait
        pthread_barrier_wait(&m_barrier);
        pthread_barrier_wait(&m_barrier);   
        pthread_barrier_wait(&m_barrier);
        //end time
        double total = getTime() - start;
        double total_cpu = getCPUTime() - start_cpu;
        //print summary 
        if(found){
            v2_print_summary(temp->name, final_result, count_all,total, total_cpu, 0);  
        }else{
            v2_print_summary(temp->name, NULL, count_all, total, total_cpu, 1);
        }
        found = 0;
        count_all = 0;
    }
    //wait for all thread finish
    for (size_t j = 0; j < thread_count; j++) {
      pthread_join(ptd[j], NULL);
    }
    pthread_mutex_destroy(&m);
    pthread_barrier_destroy(&m_barrier);
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
