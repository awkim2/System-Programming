/**
 * Perilous Pointers
 * CS 241 - Fall 2019
 */
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // your code here
    first_step(81);

    int num = 132;
    int* second = &num;
    second_step(second);

    int num3 = 8942;
    int* ptr1 = &num3;
    int** ptr2 = &ptr1;
    double_step(ptr2);

    int num4 = 15;
    char* ptr3 = NULL;
    ptr3 = (char*) &num4;
    strange_step(ptr3-5);


    void* ptrr = NULL;
    int num5 = 0;
    ptrr = (char*)&num5 - 3;
    empty_step(ptrr);

    char* cptr = malloc(4);
    cptr[3] = 'u';
    void* vptr = cptr;
    two_step(vptr, cptr);
    free(cptr);


   
    char* f1 = malloc(3);
    char* f2 = f1+2;
    char* f3 = f2+2;
    three_step(f1,f2,f3);
    free(f1);



    char* third = malloc(4);
    third[3] = '8';
    char* second1= malloc(3);
    second1[2] = '0'; 
    char* first = malloc(2);
    first[1] = '(';
    step_step_step(first, second1, third);
    free(third);
    free(second1);
    free(first);


    int a = 1;
    char* intptr = (char*)(&a);
    it_may_be_odd(intptr,a);

    char tok[10] = "aaab,CS241";
    tok_step(tok);

    char * end = malloc(4);
    end[0] = 1;
    end[1] = 0;
    end[2] = 0;
    end[3] = 5;
    void * end1 = end;
    void * end2 = end;
    the_end(end1, end2);
    free(end);




    return 0;

}
