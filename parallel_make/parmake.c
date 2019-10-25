/**
 * Parallel Make Lab
 * CS 241 - Fall 2019
 */
 

#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"

queue *q = NULL; 
graph* g = NULL;

int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    q = queue_create(-1);
    g = parser_parse_makefile(makefile, targets);
    vector *goal = graph_neighbors(g, "");
    return 0;
}
