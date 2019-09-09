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

    // test 1
    const char* test_0 = NULL;
    char** out_0 = camelCaser(test_0);
    if(out_0!= NULL){
        destroy(out_0);
        return 0;
    }

    //test single punct 2
    const char* pun = ".";
    char** out_pun = camelCaser(pun);
    if(out_pun[0][0] != '\0' || out_pun[1] != NULL){
        destroy(out_pun);
        return 0;
    }
    destroy(out_pun);

    //test 3
    const char* sp = " a ..a.";
    char** out_sp = camelCaser(sp);
    if(out_sp[0][0] != 'a'){
        destroy(out_sp);
        return 0;
    }
    destroy(out_sp);

    //test4
    const char* test = "  . ..A.";
    char** out = camelCaser(test);
    char** except1 = malloc(5*sizeof(char*));
    except1[0] = "";
    except1[1] = "";
    except1[2] = "";
    except1[3] = "a";
    except1[4] = NULL;

    if((sizeof(except1)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
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


    //test5
    test = "/32";
    out = camelCaser(test);
    char** except2 = malloc(2*sizeof(char*));
    except2[0] = "";
    except2[1] = NULL;

    if((sizeof(except2)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
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
    

    //test6
    test = "@a.$.";
    out = camelCaser(test);
    char** except3 = malloc(5*sizeof(char*));
    except3[0] = "";
    except3[1] = "a";
    except3[2] = "";
    except3[3] = "";
    except3[4] = NULL;

    if((sizeof(except3)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
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

    //test7
    test = ".a...a.";
    out = camelCaser(test);
    char** except4 = malloc(6*sizeof(char*));
    except4[0] = "";
    except4[1] = "a";
    except4[2] = "";
    except4[3] = "";
    except4[4] = "a";
    except4[5] = NULL;

    if((sizeof(except4)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
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

    //test8
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
            //printf("return at %s diff is %s with iter %d",except5[iter], out5[iter],iter);
            destroy(out5);
            free(except5);
            return 0;
        }
        iter++;
    }
    destroy(out5);
    free(except5);


    //test 9
    test = "5bgmp$ QbR Hf8hZ@M5FAk XMWk6nwN ytq5?a6 6_B-D-A @qsH3 P2e?U?_^=QyE s?9S 2H5P3p!jr aas$#ZZf! Q+*VvfJ^^& c%!MRe VSgJZCwNZqCQ$ nF e5QQ75hRBLs ^qS4X";
    out = camelCaser(test);
    char** except6 = malloc(27*sizeof(char*));
    except6[0] = "5bgmp";
    except6[1] = "qbrHf8hz";
    except6[2] = "m5fakXmwk6nwnYtq5";
    except6[3] = "a66";
    except6[4] = "b";
    except6[5] = "d";
    except6[6] = "a";
    except6[7] = "qsh3P2e";
    except6[8] = "u";
    except6[9] = "";
    except6[10] = "";
    except6[11] = "";
    except6[12] = "qyeS";
    except6[13] = "9s2H5p3p";
    except6[14] = "jrAas";
    except6[15] = "";
    except6[16] = "zzf";
    except6[17] = "q";
    except6[18] = "";
    except6[19] = "vvfj";
    except6[20] = "";
    except6[21] = "";
    except6[22] = "c";
    except6[23] = "";
    except6[24] = "mreVsgjzcwnzqcq";
    except6[25] = "nfE5qq75hrbls";
    except6[26] = NULL;
    
    if((sizeof(except6)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
        destroy(out);
        free(except6);
        return 0;
    }

    iter  = 0;
    while(except6[iter]){
        if(strcmp(except6[iter], out[iter]) != 0){
            //printf("return at %s with iter %d",except6[iter], iter);
            destroy(out);
            free(except6);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except6);

    //test 10
    test = "This is ./ so hard.. i do wanna try / i just wanna give up. seriouSly! . .  ... ok. se.e .ya.";
    out = camelCaser(test);
    char** except7 = malloc(17*sizeof(char*));
    except7[0] = "thisIs";
    except7[1] = "";
    except7[2] = "soHard";
    except7[3] = "";
    except7[4] = "iDoWannaTry";
    except7[5] = "iJustWannaGiveUp";
    except7[6] = "seriously";
    except7[7] = "";
    except7[8] = "";
    except7[9] = "";
    except7[10] = "";
    except7[11] = "";
    except7[12] = "ok";
    except7[13] = "se";
    except7[14] = "e";
    except7[15] = "ya";
    except7[16] = NULL;
    

    if((sizeof(except7)/sizeof(char*)) != (sizeof(test)/sizeof(char*))){
        destroy(out);
        free(except7);
        return 0;
    }

    iter  = 0;
    while(except7[iter]){
        if(strcmp(except7[iter], out[iter]) != 0){
            //printf("return at %s with iter %d",except7[iter], iter);
            destroy(out);
            free(except7);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except7);



    //test 11
    test =  " Gpk5 *A*^D5FL $9EAH .rcc8nx &qusXnHF ^yXq2g mFqUy -KQGHK5W2k.n Ux6 B!=q7 BY?mw qER46LhsYY Ezn !yQ we4tWH6x tcFUmVuxY% cDpHQ cKs%ayG Lu3d VnsqEyM7UtGY";
    out = camelCaser(test);
    char** except9 = malloc(16*sizeof(char*));
    except9[0] = "gpk5";
    except9[1] = "a";
    except9[2] = "";
    except9[3] = "d5fl";
    except9[4] = "9eah";
    except9[5] = "rcc8nx";
    except9[6] = "qusxnhf";
    except9[7] = "yxq2gMfquy";
    except9[8] = "kqghk5w2k";
    except9[9] = "nUx6B";
    except9[10] = "";
    except9[11] = "q7By";
    except9[12] = "mwQer46lhsyyEzn";
    except9[13] = "yqWe4twh6xTcfumvuxy";
    except9[14] = "cdphqCks";
    except9[15] = NULL;
    
    if((sizeof(except9)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
        destroy(out);
        free(except9);
        return 0;
    }

    iter  = 0;
    while(except9[iter]){
        if(strcmp(except9[iter], out[iter]) != 0){
            //printf("return at %s with iter %d",except9[iter], iter);
            destroy(out);
            free(except9);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except9);

    //test 10
    // test = "9hMT ?W?ACTr cFS443xuk_r^u 9pr^yYP ZEqR#^4 _ED*F$T_U cJWHF^fg HY+=c aT_E EpHT*AbPN Ag$Rez7+%zHKrpNHRpB+E%*r _Ju?c7d+ =EwM6 *w+5Lt#U3hnV55fEmw";
    // out = camelCaser(test);
    // char** except10 = malloc(30*sizeof(char*));
    // except10[0] = "9hmt";
    // except10[1] = "w";
    // except10[2] = "actrCfs443xuk";
    // except10[3] = "r";
    // except10[4] = "u9Pr";
    // except10[5] = "yypZeqr";
    // except10[6] = "";
    // except10[7] = "4";
    // except10[8] = "ed";
    // except10[9] = "f";
    // except10[10] = "t";
    // except10[11] = "uCjwhf";
    // except10[12] = "fgHy";
    // except10[13] = "";
    // except10[14] = "cAt";
    // except10[15] = "eEpht";
    // except10[16] = "abpnAg";
    // except10[17] = "rez7";
    // except10[18] = "";
    // except10[19] = "zhkrpnhrpb";
    // except10[20] = "e";
    // except10[21] = "";
    // except10[22] = "r";
    // except10[23] = "ju";
    // except10[24] = "c7d";
    // except10[25] = "";
    // except10[26] = "ewm6";
    // except10[27] = "w";
    // except10[28] = "5lt";
    // except10[29] = NULL;
    // if((sizeof(except10)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
    //     destroy(out);
    //     free(except10);
    //     return 0;
    // }

    // iter  = 0;
    // while(except10[iter]){
    //     if(strcmp(except10[iter], out[iter]) != 0){
    //         //printf("return at %s with iter %d",except10[iter], iter);
    //         destroy(out);
    //         free(except10);
    //         return 0;
    //     }
    //     iter++;
    // }
    // destroy(out);
    // free(except10);

    //test12
    const char* test_empty = "";
    char** out_empty = camelCaser(test_empty);
    if(out_empty[0] != NULL){
        destroy(out_empty);
        return 0;
    }
    destroy(out_empty);

    //test13
    test = "aa cc\nbb. dd.";
    out = camelCaser(test);
    char** except13 = malloc(3*sizeof(char*));
    except13[0] = "aaCcBb";
    except13[1] = "dd";
    except13[2] = NULL;

    if((sizeof(except13)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
        destroy(out);
        free(except13);
        return 0;
    }

    iter  = 0;
    while(except13[iter]){
        if(strcmp(except13[iter], out[iter]) != 0){
            //printf("return at %s with iter %d",except10[iter], iter);
            destroy(out);
            free(except13);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except13);

    //test11
    test =  "\1\2\3\4\\5";
    out  = camelCaser(test);
    char** except11 = malloc(2*sizeof(char*));
    except11[0] = "\1\2\3\4";
    except11[1] = NULL;

    if((sizeof(except11)/sizeof(char*)) != (sizeof(out)/sizeof(char*))){
        destroy(out);
        free(except11);
        return 0;
    }

    iter  = 0;
    while(except11[iter]){
        if(strcmp(except11[iter], out[iter]) != 0){
            //printf("return at %s with iter %d",except10[iter], iter);
            destroy(out);
            free(except11);
            return 0;
        }
        iter++;
    }
    destroy(out);
    free(except11);

    test = "aa";
    out = camelCaser(test);
    if(out[0] != NULL){
        destroy(out);
        return 0;
    }
    destroy(out);

    const char* test_final = " ";
    char** out_final = camelCaser(test_final);
    if(out_final[0] != NULL){
        destroy(out_final);
        return 0;
    }
    destroy(out_final);




    
    return 1;
}

