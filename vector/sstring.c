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
    //your code goes here
    size_t numofd = 0;
    char* temp = this->sc;
    vector* vv = string_vector_create();
    while(*temp){
        if(*temp == delimiter)  numofd++;
        temp++;
    }
    printf("delimiter = %lu\n", numofd);
    temp = this->sc;
    char* temp2 = this->sc;
    size_t count = 0;
    while(count<numofd){
        if(*temp == delimiter){
            *temp = '\0';
            vector_push_back(vv, temp2);
            temp++;
            temp2 = temp;
            count++;
            continue;
        }
        temp++;
    }
    vector_push_back(vv, temp);
    // printf("push count %lu\n", count);
    // printf("%s\n", vector_get(vv,0));
    // printf("%s\n", vector_get(vv,1));
    // printf("%s\n", vector_get(vv,2));
    // printf("%s\n", vector_get(vv,3));
    return vv;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // your code goes here
    char* start = this->sc + offset;
    char* sub = strstr(start, target);
    if(!sub) return -1;
    size_t newlen = strlen(this->sc) - strlen(target) + strlen(substitution);
    char* temp = malloc(newlen+1);
    strncpy(temp,this->sc, (strlen(this->sc)- strlen(sub)));
    strcat(temp, substitution);
    strcat(temp, &sub[strlen(target)]);
    this->sc = realloc(this->sc, strlen(temp)+1);
    strcpy(this->sc, temp);
    free(temp);
    printf("%s\n", this->sc);
    return 0;
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
