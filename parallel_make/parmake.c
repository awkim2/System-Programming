/**
 * Parallel Make Lab
 * CS 241 - Fall 2019
 */
 
#include "./includes/vector.h"
#include "./includes/graph.h"
#include "./includes/set.h"
#include "./includes/queue.h"
#include "./includes/compare.h"
#include "format.h"
#include "parmake.h"
#include "parser.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
queue *q = NULL; 
graph* g = NULL;
set* visited_set = NULL;
int fail = 0;

int isCyclic(char* target) {
    if (!visited_set) visited_set = shallow_set_create();
    if (set_contains(visited_set, target)) {
        set_destroy(visited_set);
        visited_set = NULL;
        return 1;
    }else{
      set_add(visited_set, target);
      vector *neighbor_vec = graph_neighbors(g, target);
      for (size_t i = 0; i < vector_size(neighbor_vec); i++) {
        if (isCyclic(vector_get(neighbor_vec, i))) {
          vector_destroy(neighbor_vec);
          return 1;
        }
      }
      vector_destroy(neighbor_vec);
    }
    set_destroy(visited_set);
    visited_set = NULL;
    return 0;
}
int is_fail(char* target){
    int flag = 0;
    if(access(target, F_OK) == -1) flag = 1;
    vector* neighbor_vec = graph_neighbors(g,target);
    for(size_t i = 0 ; i < vector_size(neighbor_vec);i++){
        char* neighbor = vector_get(neighbor_vec, i);
        if(access(neighbor, F_OK) == -1){
            flag = 1;
        }else{
            struct stat oldtime;
            stat(target, &oldtime);
            struct stat newtime;
            stat(neighbor, &newtime);
            if (difftime(oldtime.st_mtime, newtime.st_mtime) < 0) flag = 0;
        }
        is_fail(neighbor);   
    }
    vector_destroy(neighbor_vec);
    rule_t *rule = (rule_t*) graph_get_vertex_value(g, target);
    if(flag && !rule->state && !fail){
        queue_push(q, rule);
        rule_t *rule = queue_pull(q);
        for (size_t i = 0; i < vector_size(rule->commands); i++) {
            if (system(vector_get(rule->commands, i)) != 0) {
                fail = 1;
                break;
            }
        }   
        rule->state = 1;
    }
    return 0;
}

int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    q = queue_create(-1);
    g = parser_parse_makefile(makefile, targets);
    vector *goal_vec = graph_neighbors(g, "");
    for(size_t i = 0; i < vector_size(goal_vec); i++){
        char* goal =  vector_get(goal_vec, i);
        if(isCyclic(goal)) print_cycle_failure(goal);
        else {is_fail(goal);}
    }
    queue_push(q,NULL);
    vector_destroy(goal_vec);
    graph_destroy(g);
    queue_destroy(q);
    return 0;
}
