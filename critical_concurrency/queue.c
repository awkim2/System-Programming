/**
 * Critical Concurrency
 * CS 241 - Fall 2019
 * partner guanhua2 mengz5
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
    struct queue *result = malloc(sizeof(struct queue));
    result->head = NULL;
    result->tail = NULL;
    result->size = 0;
    result->max_size = max_size;
    pthread_cond_init(&result->cv, NULL);
    pthread_mutex_init(&result->m, NULL);
    return result;
}

void queue_destroy(queue *this) {
    queue_node *ptr = this->tail;
    queue_node *temp = NULL;
    while (ptr) {
      temp = ptr;
      ptr = ptr->next;
      free(temp);
    }
    pthread_mutex_destroy(&this->m);
    pthread_cond_destroy(&this->cv);
    free(this);
}

void queue_push(queue *this, void *data) {
    pthread_mutex_lock(&this->m);
    while (this->size == this->max_size && this->max_size > 0 ) {
      pthread_cond_wait(&this->cv, &this->m);
    }
    queue_node *newNode = malloc(sizeof(queue_node));
    newNode->data = data;
    newNode->next = NULL;
    if (this->head){
      this->head->next = newNode;
    }
    this->head = newNode;
    if (this->tail == NULL) {
      this->tail = newNode;
    }
    this->size++;
    if (this->size > 0) {
      pthread_cond_broadcast(&this->cv);
    }
    pthread_mutex_unlock(&this->m);
}

void *queue_pull(queue *this) {
    pthread_mutex_lock(&this->m);

    while (this->size == 0) {
      pthread_cond_wait(&this->cv, &this->m);
    }
    void *outcome = this->tail->data;
    queue_node *s = this->tail;
    this->tail = this->tail->next;
    free(s);
    this->size--;
    if(this->size == 0){
        this->head = NULL;
    }
    if (this->size > 0  && this->size < this->max_size) {
      pthread_cond_broadcast(&this->cv);
    }
    pthread_mutex_unlock(&this->m);
    return outcome;
}
