/**
 * Deadlock Demolition
 * CS 241 - Fall 2019
 */
#include "graph.h"
#include "libdrm.h"
#include "set.h"
#include <pthread.h>

struct drm_t { pthread_mutex_t m;};
static graph* g;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
set *new_set;


drm_t *drm_init() {
    /* Your code here */
    drm_t *result = malloc(sizeof(drm_t));
    pthread_mutex_init(&(result->m), NULL);
    pthread_mutex_lock(&m);
    if(!g) g = shallow_graph_create();
    graph_add_vertex(g, result);
    pthread_mutex_unlock(&m);
    return result;
}

int drm_post(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&m);
    int result = 0;
    if(graph_contains_vertex(g, thread_id) && graph_contains_vertex(g,drm)){
        if(graph_adjacent(g, drm, thread_id)){
            result = 1;
            graph_remove_edge(g, drm, thread_id);
            pthread_mutex_unlock(&drm->m);
        }
    }
    pthread_mutex_unlock(&m);
    return result;
}

int isCyclic(void* temp){
    if(!new_set) new_set = shallow_set_create();
    if(set_contains(new_set,temp)){
        new_set = NULL;
        return 1;
    }
    set_add(new_set, temp);
    vector* n = graph_neighbors(g, temp);
    size_t i = 0;
    for(;i<vector_size(n);i++){
        if(isCyclic(vector_get(n, i))) return 1;
    }
    new_set = NULL;
    return 0;
}

int drm_wait(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&m);
    int result = 0;
    graph_add_vertex(g, thread_id);
    if(!graph_adjacent(g, drm, thread_id)){
        graph_add_edge(g,thread_id,drm);
        result = 1;
        if(!isCyclic(thread_id)){
            result = 2;
            pthread_mutex_unlock(&m);
            pthread_mutex_lock(&drm->m);
            pthread_mutex_lock(&m);
            graph_remove_edge(g, thread_id, drm);
            graph_add_edge(g, drm, thread_id);
        }
    }
    if(result == 0){
        pthread_mutex_unlock(&m);
        return 0;
    }
    if(result == 1){
        graph_remove_edge(g,thread_id,drm);
        pthread_mutex_unlock(&m);
        return 0;
    }
    pthread_mutex_unlock(&m);
    return 1;
}

void drm_destroy(drm_t *drm) {
    /* Your code here */
    graph_remove_vertex(g, drm);
    pthread_mutex_destroy(&drm->m);
    pthread_mutex_destroy(&m);
    free(drm);
    return;
}
