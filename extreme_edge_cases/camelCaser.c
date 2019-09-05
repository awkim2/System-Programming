/**
 * Extreme Edge Cases
 * CS 241 - Fall 2019
 */
#include "camelCaser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
char **camel_caser(const char *input_str) {
    // TODO: Implement me!

    // NULL case
    if (!input_str || *input_str == '\0') return NULL;


    //get_sentence_count
    const char* countptr = input_str;
    int count = 0;
    while(*countptr){
        if(ispunct(*countptr)) count++;
        countptr++;
    }

    //initialize char** result
    char** result = calloc((count + 1), sizeof(char*));
    // int t = 0;
    // while(t < count){
    //     result[t] = NULL;
    //     t++;
    // }
    result[count] = NULL;

    //initialize ptr array
    countptr = input_str;
    int i = 0;
    while(i < count){
        int length = 0;
        while(!ispunct(*countptr)){
            if(!isspace(*countptr)) length++;
            countptr++;
        }
        result[i] = malloc(length+1);
        result[i][length] = '\0';
        i++;
        countptr++;
    }

    char input_c;
    int j = 0;
    int k = 0;
    char result_c;
    int sentence = 0;
 
    int cap = 0;
    int start_C = 1;

    while ((input_c = input_str[j++])) {
        if (!result[sentence])
            break;
        if (ispunct(input_c)) {
            cap = 0;
            start_C = 1;
            sentence++;
            k = 0;
        } else if (isspace(input_c)) {
            cap = 1;
        } else {
            if (isalpha(input_c)) {
                if(cap && !start_C){
                    result_c = toupper(input_c);
                }else{
                    result_c = tolower(input_c);
                }
            } else {
                result_c = input_c;
            }
            cap = 0;
            start_C = 0;
            result[sentence][k] = result_c;
            k++;
        }
    }
    //printf("after == %s \n", *(result+2));
    
     //camCaser
    // size_t input_len = strlen(input_str)+1;
    // char copy_str[input_len];
    // strcpy(copy_str, input_str);

    // int i = 0, j = 0;
    // while(copy_str[i] != '\0'){
    //     copy_str[i] = tolower(copy_str[i]);
    //     i++;
    // }
    // while(copy_str[j] != '\0'){
    //     while(!ispunct(copy_str[j])){
    //         if(isspace(copy_str[j])) copy_str[j+1] = toupper(copy_str[j+1]);
    //         j++;  
    //     }
    //     while(ispunct(copy_str[j]) || isspace(copy_str[j])){
    //         j++;
    //     }
    // }



    return result;
    
}

void destroy(char **result) {
    // TODO: Implement me!
    int i = 0;
    while(result[i] != NULL){
        free(result[i]);
        result[i] = NULL;
    }
    free(result);
    result = NULL;
    return;
}
