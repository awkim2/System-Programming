/**
 * Extreme Edge Cases
 * CS 241 - Fall 2019
 */
#include <stdio.h>

#include "camelCaser_ref_utils.h"

int main() {
    // Enter the string you want to test with the reference here
    char *input = "wbgnkxC5xF xKA@LH yGEE?eaVJLwkZ56fMuk^PTQ fuZGPveVsx3q nh@M7VXYz=^2y6n _MDcGg5S9N7QTb* +^Dv&K2FK2vL3g7&x+ +x2zM_-Zeg nw&5FJ 23X$d?$%y=M$!";
  
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
