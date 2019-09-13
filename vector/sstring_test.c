/**
 * Vector
 * CS 241 - Fall 2019
 */
#include "sstring.h"
struct sstring {
    // Anything you want
    vector * svec;
};



int main(int argc, char *argv[]) {
    // TODO create some tests
    const char* a = "abc";
    sstring * sstr = cstr_to_sstring(a);
    vector * v = sstr->svec;
    for(size_t i = 0;i < vector_size(v); i++){
        printf("%s", vector_get(v,i));
    }
    char* d = sstring_to_cstr(sstr);
    printf("%s\n", d);



    sstring *str1 = cstr_to_sstring("abc");
    sstring *str2 = cstr_to_sstring("def");
    int len = sstring_append(str1, str2); // len == 6
    sstring_to_cstr(str1); // == "abcdef"
    printf("len is %d and result is %s", len, sstring_to_cstr(str1));
    return 0;
}
