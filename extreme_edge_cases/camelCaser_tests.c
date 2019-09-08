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


    // const char* sp = " a ..a.";
    // char** out_sp = camel_caser(sp);
    // if(out_sp[0] != 'a'){
    //     destroy(out_sp);
    //     return 0;
    // }

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

    //test5
    char* test5 = "wbgnkxC5xF xKA@LH yGEE?eaVJLwkZ56fMuk^PTQ fuZGPveVsx3q nh@M7VXYz=^2y6n _MDcGg5S9N7QTb* +^Dv&K2FK2vL3g7&x+ +x2zM_-Zeg nw&5FJ 23X$d?$%y=M$!";
    char** out5 = camelCaser(test5);
    char** except5 = (char**)malloc(25*sizeof(char*));
    except5[0] = "wbgnkxc5xfXka";
    except5[1] = "lhYgee";
    except5[2] = "eavjlwkz56fmuk";
    except5[3] = "ptqFuzgpvevsx3qNh";
    except5[4] = "m7vxyz";
    except5[5] = "";
    except5[6] = "2y6n";
    except5[7] = "mdcgg5s9n7qtb";
    except5[8] = "";
    except5[9] = "";
    except5[10] = "dv";
    except5[11] = "k2fk2vl3g7";
    except5[12] = "x";
    except5[13] = "";
    except5[14] = "x2zm";
    except5[15] = "";
    except5[16] = "zegNw";
    except5[17] = "5fj23X";
    except5[18] = "d";
    except5[19] = "";
    except5[20] = "";
    except5[21] = "y";
    except5[22] = "m";
    except5[23] = "";
    except5[24] = NULL;

    if((sizeof(except5)/sizeof(char*)) != (sizeof(out5)/sizeof(char*))){
        destroy(out5);
        free(except5);
        return 0;
    }

    iter  = 0;
    while(except5[iter]){
        if(strcmp(except5[iter], out5[iter])){
            //printf("%d\n",iter);
            //printf("%s\n",out5[iter]);
            destroy(out5);
            free(except5);
            return 0;
        }
        iter++;
    }
    destroy(out5);
    free(except5);
    //test 6
    // test = "This is ./ so hard.. i do wanna try / i just wanna give up. seriouSly! . .  ... ok. se.e .ya.";
    // out = camelCaser(test);
    // char** except6 = malloc(17*sizeof(char*));
    // except6[0] = "thisIs";
    // except6[1] = "";
    // except6[2] = "soHard";
    // except6[3] = "";
    // except6[4] = "iDoWannaTry";
    // except6[5] = "iJustWannaGiveUp";
    // except6[6] = "seriously";
    // except6[7] = "";
    // except6[8] = "";
    // except6[9] = "";
    // except6[10] = "";
    // except6[11] = "";
    // except6[12] = "ok";
    // except6[13] = "se";
    // except6[14] = "e";
    // except6[15] = "ya";
    // except6[16] = NULL;
    

    // if((sizeof(except6)/sizeof(char*)) != (sizeof(test)/sizeof(char*))){
    //     destroy(out);
    //     free(except6);
    //     return 0;
    // }

    // iter  = 0;
    // while(except6[iter]){
    //     if(strcmp(except6[iter], out[iter]) != 0){
    //         destroy(out);
    //         free(except6);
    //         return 0;
    //     }
    //     iter++;
    // }
    // destroy(out);
    // free(except6);


    return 1;
}
