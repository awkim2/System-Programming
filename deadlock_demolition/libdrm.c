#include "graph.h"
#include "libdrm.h"
#include "set.h"
#include <pthread.h>

struct drm_t {
    pthread_mutex_t m;
};

graph *gra = NULL;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
set *isVisited = NULL;

//bfs
bool isCycle(void* ver) {
    if(!isVisited) isVisited = shallow_set_create();
    if(set_contains(isVisited, ver)){
        isVisited = NULL;
        return true;
    }else{
      set_add(isVisited, ver);
      vector *neighbors = graph_neighbors(gra, ver);
      size_t i = 0;
      for( i < vector_size(neighbors); i++) {
        if(isCycle(vector_get(neighbors, i))) return true;
      }
    }
    isVisited = NULL;
    return false;
}


drm_t *drm_init() {
    /* Your code here */
    drm_t *drm = malloc(sizeof(drm_t));
    pthread_mutex_init(&drm->m, NULL);
    pthread_mutex_lock(&m);
    if (!gra) gra = shallow_graph_create();
    graph_add_vertex(gra, drm);
    pthread_mutex_unlock(&m);
    return drm;
}

int drm_post(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&m);
    if (!graph_contains_vertex(gra, thread_id)) {
      pthread_mutex_unlock(&m);
      return 0;
    }
    if (!graph_contains_vertex(gra, drm)) {
      pthread_mutex_unlock(&m);
      return 0;
    }
    if (graph_adjacent(gra, drm, thread_id)) {
      graph_remove_edge(gra, drm, thread_id);
      pthread_mutex_unlock(&drm->m);
    }
    pthread_mutex_unlock(&m);
    return 1;
}

int drm_wait(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&m);
    graph_add_vertex(gra, thread_id);
    if(graph_adjacent(gra, drm, thread_id)){
      pthread_mutex_unlock(&m);
      return 0;
    }
    graph_add_edge(gra, thread_id, drm);
    if(isCycle(thread_id)){
      graph_remove_edge(gra, thread_id, drm);
      pthread_mutex_unlock(&m);
      return 0;
    }else{
      pthread_mutex_unlock(&m);
      pthread_mutex_lock(&drm->m);
      pthread_mutex_lock(&m);
      graph_remove_edge(gra, thread_id, drm);
      graph_add_edge(gra, drm, thread_id);
      pthread_mutex_unlock(&m);
      return 1;
    }
}

void drm_destroy(drm_t *drm) {
    /* Your code here */
    graph_remove_vertex(gra, drm);
    pthread_mutex_destroy(&drm->m);
    pthread_mutex_destroy(&m);
    free(drm);
    return;
}