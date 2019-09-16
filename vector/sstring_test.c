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


    // sstring_split(cstr_to_sstring("abcdeefg"), 'e');

     // == [ "This", "is", "a", "sentence." ]
    // sstring *replace_me = cstr_to_sstring("This is a {} day, {}!");
    // sstring_substitute(replace_me, 18, "{}", "friend");

    // sstring_substitute(replace_me, 0, "{}", "good");
    // sstring_to_cstr(replace_me); // == "This is a good day, friend!"
    return 0;
}
