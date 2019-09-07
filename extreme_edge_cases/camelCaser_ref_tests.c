/**
 * Extreme Edge Cases
 * CS 241 - Fall 2019
 */
#include <stdio.h>

#include "camelCaser_ref_utils.h"

int main() {
    // Enter the string you want to test with the reference here
    char *input = "7nG9k2vO8gmNL4yKcy1. kk81mqmKdWkpWgyL. GtxVB/C8Y370bA./6eZVh9I. o1jJ6VlkPlBJq.C,ke 6DZJVJe5U ..GnNT'Kjt4o1, 4jfeTzqlE5INinBj44eB0f!!! !ATZ8BvUM0xDWzgqeyFhEC RZhjo5kFD5VR0q7b, LfBMLhHOqW3n09.!ioviz9 LLMx. M05CaY3M/F2.  .uU47h8 kqTxEYZn d1MVh";
  
    // This function prints the reference implementation output on the terminal
    print_camelCaser(input);

    // Put your expected output for the given input above
    //char **output = NULL;
    char *output[] = {"hello", "welcomeToCS241", "iLoveSystemProgramming", NULL};

    // Compares the expected output you supplied with the reference output
    printf("check_output test: %d\n", check_output(input, output));

    // Feel free to add more test cases
    return 0;
}
