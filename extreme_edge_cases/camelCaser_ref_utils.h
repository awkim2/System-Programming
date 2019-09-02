/**
 * Extreme Edge Cases
 * CS 241 - Fall 2019
 */
#pragma once

/**
 * This function takes in an input string, converts it to camelCase, and
 * prints it out onto stdout. Note that special characters may not display
 * correctly on the terminal.
 */
void print_camelCaser(char *input);

/**
 * This function checks a given input with an expected output to see if the
 * supplied output is the correct camelCase for the given input. Returns 1
 * if they match, and 0 otherwise.
 *
 * This function is useful when you expect non printable characters in your
 * output.
 */
int check_output(char *input, char **output);
