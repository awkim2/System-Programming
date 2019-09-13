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
    vector * svec;
};

sstring *cstr_to_sstring(const char *input) {
    // your code goes here
    sstring *sstr = calloc(1,sizeof(sstring));
    sstr->svec = char_vector_create();
    size_t i = 0;
    for(; i <= strlen(input)+1; i++){
        vector_push_back(sstr->svec, (char*)(input));
        input++;
    }
    return sstr;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
    char* cstr = calloc(vector_size(input->svec), 1);
    for(size_t i = 0; i < vector_size(input->svec); i++){
        cstr[i] = *(char*)vector_get(input->svec, i);
    }
    return cstr;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
    for (size_t i = 0; i < vector_size(addition->svec); i++)
    {
        vector_push_back(this->svec, vector_get(addition->svec, i));
    }
    return vector_size(this->svec);
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
    
    return NULL;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // your code goes here
    return -1;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
    return NULL;
}

void sstring_destroy(sstring *this) {
    // your code goes here
}
