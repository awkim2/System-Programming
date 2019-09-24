/**
 * Mini Memcheck
 * CS 241 - Fall 2019
 */
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Your tests here using malloc and free
    char* a = malloc(15);
    char* b = calloc(20, 1);
    char* c = realloc(a, 30);
    //free(c);
    free(c);
    //free(a);
    return 0;
}
