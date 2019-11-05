/**
 * Parallel Make Lab
 * CS 241 - Fall 2019
 */
 
#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "set.h"
#include "queue.h"
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>

graph *g = NULL;
queue *q = NULL;
dictionary *d = NULL;

pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
// void* for dic_set
int zero = 0;
int one = 1;
//count for finished thread
size_t thread_count = 0;

//VECTOR_FOR_EACH
//Vector iteration macro. `vecname` is the name of the vector. `varname` is the
//name of a temporary (local) variable to refer to each element in the vector,
//and `callback` is a block of code that gets executed once for each element in
//the vector until `break;` is called

int is_Cyclic(void* temp){
    if(!dictionary_contains(d, temp)) return 0;
    if(*(int *)dictionary_get(d, temp) == 1) return 1;
    dictionary_set(d, temp, &one);
    vector* dependencies = graph_neighbors(g, temp);
    VECTOR_FOR_EACH(dependencies, vt, {
        if (*(int*)dictionary_get(d, vt) == 0) {
            int r = is_Cyclic(vt);
            vector_destroy(dependencies);
            return r;
        }
        vector_destroy(dependencies);
        return 1;
    });
    vector_destroy(dependencies);
    return 0;
}


void push_q(char *target) {
    if (*(int*)dictionary_get(d, target) == 1) return;
    dictionary_set(d, target, &one);
    vector* dependencies = graph_neighbors(g, target);
    //push for each item in dependencies
    VECTOR_FOR_EACH(dependencies, vt, {push_q(vt);});
    if (vector_empty(dependencies)) queue_push(q, target);
    rule_t *rule = (rule_t *)graph_get_vertex_value(g, target);
    rule->state = vector_size(dependencies);
    vector_destroy(dependencies);
}


void *run_commands(void *ptr) {
    while (1) {
        char *target = (char *)queue_pull(q);
        if (!target) break;
        rule_t *rule = (rule_t *) graph_get_vertex_value(g, target);
        struct stat stat_info;
        int flag_stat = 0;
        int result = stat(rule->target, &stat_info);
        if (result == -1) flag_stat = 1;
        //if all  stat
        if (flag_stat == 0) {
            pthread_mutex_lock(&m);
            vector *dependencies = graph_neighbors(g, target);
            pthread_mutex_unlock(&m);
            VECTOR_FOR_EACH(dependencies, vt, {
                rule_t *temp_rule = (rule_t*)graph_get_vertex_value(g, vt);
                struct stat tempstat;
                //compare edit time
                if (stat(temp_rule->target, &tempstat) == -1 || tempstat.st_mtime > stat_info.st_mtime) {
                    flag_stat = 1;
                    break;
                }
            });
            vector_destroy(dependencies);
        }
        int done = 1;
        //if ok run command
        if (flag_stat) {
            VECTOR_FOR_EACH(rule->commands, vt, {
                if (system(vt) != 0) {
                    done = 0;
                    break;
                }
            });
        }
        //m lock
        pthread_mutex_lock(&m);
        vector *par = graph_antineighbors(g, target);
        VECTOR_FOR_EACH(par, vt, {
            if (done) {
                rule_t *r = graph_get_vertex_value(g, vt);
                r->state -= 1;
                if (r->state == 0) queue_push(q, vt);
            }
            if (!strcmp(vt, "")) {
                thread_count++;
                pthread_cond_signal(&cv);
            }
        });
        pthread_mutex_unlock(&m);
        //m unlock
        vector_destroy(par);
    }
    return NULL;
}

//reset the dic everytime
void zero_out() {
    vector* Vert = graph_vertices(g);
    VECTOR_FOR_EACH(Vert, vt, {dictionary_set(d, vt, &zero);});
    vector_destroy(Vert);
}


int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    g = parser_parse_makefile(makefile, targets);
    q = queue_create(-1);
    d = string_to_int_dictionary_create();
    zero_out();
    pthread_t pids[num_threads];
    vector *goal_v = graph_neighbors(g, "");
    size_t i = 0;
    for (;i < vector_size(goal_v); i++) {
        char *goal = vector_get(goal_v, i);
        if (is_Cyclic(goal)) {
            print_cycle_failure(goal);
            rule_t *rule = (rule_t*) graph_get_vertex_value(g, goal);
            rule->state = -1;
            vector_erase(goal_v, i);
        }
    }
    rule_t *root = graph_get_vertex_value(g, "");
    root->state = vector_size(goal_v);
    if (vector_empty(goal_v)) return 0;
    zero_out();
    VECTOR_FOR_EACH(goal_v, vt, {push_q(vt);});
    zero_out();
    for (size_t i = 0; i < num_threads; i++) {
        pthread_create(pids + i, NULL, run_commands, NULL);
    }
    //lock m
    pthread_mutex_lock(&m);
    while (thread_count != vector_size(goal_v)) {
        pthread_cond_wait(&cv, &m);
    }
    pthread_mutex_unlock(&m);
    //unlock m
    for (size_t i = 0; i < num_threads + 1; i++) {
        queue_push(q, NULL);
    }
    //pjoin
    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(pids[i], NULL);
    }
    //done
    vector_destroy(goal_v);
    graph_destroy(g);
    queue_destroy(q);
    dictionary_destroy(d);
    pthread_cond_destroy(&cv);
    pthread_mutex_destroy(&m);
    return 0;
}