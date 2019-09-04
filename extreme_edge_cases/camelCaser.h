/**
 * Extreme Edge Cases
 * CS 241 - Fall 2019
 */
#pragma once

/**
 * Takes an input string, and returns the camelCased output. Details on this
 * function, as well as the formal description of camelCase are available in
 * the assignment's documentation
 */
char **camel_caser(const char *input_str);

/**
 * Destroys the camelCased output returned by the camel_caser function
 */
void destroy(char **result);
