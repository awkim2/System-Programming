/**
 * Extreme Edge Cases
 * CS 241 - Fall 2019
 */
#include <stdio.h>
#include <stdlib.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

int main() {
    // Feel free to add more test cases of your own!
    //char* input = "This is ./ so hard.. i do wanna try / i just wanna give up. seriouSly! . .  ... ok. se.e .ya.";
    // char* input = "This is ./ so hard.. i do wanna try / i just wanna give up. seriouSly! . .  ... ok. se.e .ya.";
    // char** out = camel_caser(input);
    // int i = 0;
    // while (out[i]){
    //     printf("output is %s \n", out[i]);
    //     i++;
    // }

    if (test_camelCaser(&camel_caser, &destroy)) {
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }
}
