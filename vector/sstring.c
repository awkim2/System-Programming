/**
 * Vector
 * CS 241 - Fall 2019
 */
#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    // Anything you want
    char * sc;
};

sstring *cstr_to_sstring(const char *input) {
    // your code goes here
    sstring *sstr = calloc(1,sizeof(sstring));
    sstr->sc = strdup(input);
    return sstr;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
    return strdup(input->sc);
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
    this->sc = realloc(this->sc, strlen(this->sc)+strlen(addition->sc)+1);
    strcat(this->sc, addition->sc); 
    return strlen(this->sc);
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
    // int numofd = 0;
    // char* temp = this->sc;
    // while(temp){
    //     if(*temp == delimiter)  numofd++;
    //     temp++;
    // }
    // prinff("delimiter = %d", numofd);
    // temp = this->sc;
    // for(int i = 0; i < numofd; i++){
    //     if(temp[i] == delimiter){
    //         temp[i] = '\0';
    //     }
    // }
    return NULL;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // your code goes here
    return -1;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
    char * begin = this->sc + start;
    char * slice = malloc(end-start+1);
    strncpy(slice, begin, end - start);
    slice[end-start] = '\0';
    return slice;
}

void sstring_destroy(sstring *this) {
    free(this->sc);
    free(this);
    // your code goes here
}
