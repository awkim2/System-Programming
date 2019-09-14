/**
 * Vector
 * CS 241 - Fall 2019
 */
#include "sstring.h"
#include <string.h>
struct sstring {
    // Anything you want
    char * sc;
};



int main(int argc, char *argv[]) {
    // TODO create some tests
    // const char* a = "abc";
    // sstring * sstr = cstr_to_sstring(a);
    // vector * v = sstr->sc;
    // for(size_t i = 0;i < vector_size(v); i++){
    //     printf("%s", vector_get(v,i));
    // }
    // char* d = sstring_to_cstr(sstr);
    // printf("%s\n", d);



    sstring *str1 = cstr_to_sstring("abcdefghijk");
    sstring *str2 = cstr_to_sstring("def");
    printf("%s\n", str1->sc);
    printf("%s\n", str2->sc);
    char* re = sstring_to_cstr(str2);
    printf("%s\n", re);
    char* slice = sstring_slice(str1, 2, 5);
    printf("%s\n", slice);
    printf("size of slice =%lu, strlen = %lu\n", sizeof(slice), strlen(slice));
    // int len = sstring_append(str1, str2); // len == 6
    // sstring_to_cstr(str1); // == "abcdef"
    return 0;
}
