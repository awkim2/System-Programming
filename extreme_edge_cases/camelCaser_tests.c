/**
 * Extreme Edge Cases
 * CS 241 - Fall 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @param  destroy      A pointer to the function that destroys camelCaser
 * 			output.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Return 1 if the passed in function works properly; 0 if it doesn't.

    // test NULL
    const char* test_0 = NULL;
    char** out_0 = camelCaser(test_0);
    if(out_0!= NULL){
        destroy(out_0);
        return 0;
    }
    //test single punct
    const char* pun = ".";
    char** out_pun = camelCaser(pun);
    if(out_pun[0][0] != '\0' || out_pun[1] != NULL){
        destroy(out_pun);
        return 0;
    }

    //test1
    const char* test = "  . ..A.";
    char** out = camelCaser(test);
    char** except1 = malloc(5*sizeof(char*));
    except1[0] = "";
    except1[1] = "";
    except1[2] = "";
    except1[3] = "a";
    except1[4] = NULL;

    if((sizeof(except1)/sizeof(char*)) != (sizeof(test)/sizeof(char*))){
        destroy(out);
        free(except1);
        return 0;
    }

    int iter  = 0;
    while(except1[iter]){
        if(strcmp(except1[iter], out[iter]) != 0){
            destroy(out);
            free(except1);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except1);


    //test2
    test = "/32";
    out = camelCaser(test);
    char** except2 = malloc(2*sizeof(char*));
    except2[0] = "";
    except2[1] = NULL;

    if((sizeof(except2)/sizeof(char*)) != (sizeof(test)/sizeof(char*))){
        destroy(out);
        free(except2);
        return 0;
    }

    iter  = 0;
    while(except2[iter]){
        if(strcmp(except2[iter], out[iter]) != 0){
            destroy(out);
            free(except2);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except2);
    

    //test3
    test = "@a.$.";
    out = camelCaser(test);
    char** except3 = malloc(5*sizeof(char*));
    except3[0] = "";
    except3[1] = "a";
    except3[2] = "";
    except3[3] = "";
    except3[4] = NULL;

    if((sizeof(except3)/sizeof(char*)) != (sizeof(test)/sizeof(char*))){
        destroy(out);
        free(except3);
        return 0;
    }

    iter  = 0;
    while(except3[iter]){
        if(strcmp(except3[iter], out[iter]) != 0){
            destroy(out);
            free(except3);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except3);

    //test4 
    test = ".a...a.";
    out = camelCaser(test);
    char** except4 = malloc(6*sizeof(char*));
    except4[0] = "";
    except4[1] = "a";
    except4[2] = "";
    except4[3] = "";
    except4[4] = "a";
    except4[5] = NULL;

    if((sizeof(except4)/sizeof(char*)) != (sizeof(test)/sizeof(char*))){
        destroy(out);
        free(except4);
        return 0;
    }

    iter  = 0;
    while(except4[iter]){
        if(strcmp(except4[iter], out[iter]) != 0){
            destroy(out);
            free(except4);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except4);

    // //test5
    // test = "7nG9k2vO8gmNL4yKcy1. kk81mqmKdWkpWgyL. GtxVB/C8Y370bA./6eZVh9I. o1jJ6VlkPlBJq.C,ke 6DZJVJe5U ..GnNT'Kjt4o1, 4jfeTzqlE5INinBj44eB0f!!! !ATZ8BvUM0xDWzgqeyFhEC RZhjo5kFD5VR0q7b, LfBMLhHOqW3n09.!ioviz9 LLMx. M05CaY3M/F2.  .uU47h8 kqTxEYZn d1MVh";
    // out = camelCaser(test);
    // char** except5 = malloc(24*sizeof(char*));
    // except5[0] = "7ng9k2vo8gmnl4ykcy1";
    // except5[1] = "kk81mqmkdwkpwgyl";
    // except5[2] = "gtxvb";
    // except5[3] = "c8y370ba";
    // except5[4] = "";
    // except5[5] = "6ezvh9i";
    // except5[6] = "o1jj6vlkplbjq";
    // except5[7] = "c";
    // except5[8] = "ke6Dzjvje5u";
    // except5[9] = "";
    // except5[10] = "gnnt";
    // except5[11] = "kjt4o1";
    // except5[12] = "4jfetzqle5ininbj44eb0f";
    // except5[13] = "";
    // except5[14] = "";
    // except5[15] = "";
    // except5[16] = "atz8bvum0xdwzgqeyfhecRzhjo5kfd5vr0q7b";
    // except5[17] = "lfbmlhhoqw3n09";
    // except5[18] = "";
    // except5[19] = "ioviz9Llmx";
    // except5[20] = "m05cay3m";
    // except5[21] = "f2";
    // except5[22] = "";
    // except5[23] = NULL;

    // if((sizeof(except5)/sizeof(char*)) != (sizeof(test)/sizeof(char*))){
    //     destroy(out);
    //     free(except5);
    //     return 0;
    // }

    // iter  = 0;
    // while(except5[iter]){
    //     if(strcmp(except5[iter], out[iter]) != 0){
    //         destroy(out);
    //         free(except5);
    //         return 0;
    //     }
    //     iter++;
    // }
    // destroy(out);
    // free(except5);

    //test 6
    test = "This is ./ so hard.. i do wanna try / i just wanna give up. seriouSly! . .  ... ok. se.e .ya.";
    out = camelCaser(test);
    char** except6 = malloc(17*sizeof(char*));
    except6[0] = "thisIs";
    except6[1] = "";
    except6[2] = "soHard";
    except6[3] = "";
    except6[4] = "iDoWannaTry";
    except6[5] = "iJustWannaGiveUp";
    except6[6] = "seriously";
    except6[7] = "";
    except6[8] = "";
    except6[9] = "";
    except6[10] = "";
    except6[11] = "";
    except6[12] = "ok";
    except6[13] = "se";
    except6[14] = "e";
    except6[15] = "ya";
    except6[16] = NULL;
    

    if((sizeof(except6)/sizeof(char*)) != (sizeof(test)/sizeof(char*))){
        destroy(out);
        free(except6);
        return 0;
    }

    iter  = 0;
    while(except6[iter]){
        if(strcmp(except6[iter], out[iter]) != 0){
            destroy(out);
            free(except6);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except6);


    return 1;
}
