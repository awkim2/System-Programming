/**
 * Utilities Unleashed
 * CS 241 - Fall 2019
 * partner: ruoyuz3 mengz5
 */


#include "format.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

char **strsplit(const char* input, const char* d) {
    char *s = strdup(input);
    size_t tokens_alloc = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_alloc, sizeof(char*));
    char *token = s;
    char *rest = s;
    while ((token = strsep(&rest, d))) {
        if (tokens_used == tokens_alloc) {
            tokens_alloc *= 2;
            tokens = realloc(tokens, tokens_alloc * sizeof(char*));
        }
        tokens[tokens_used++] = strdup(token);
    }
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
    free(s);
    return tokens;
}


int main(int argc, char *argv[]) {
    if(argc < 3) print_env_usage();
    int flag = 0;
    pid_t pid = fork();
    if(pid == -1){
        print_fork_failed();
    }
    else if(pid == 0){
        //child 
        int i = 1;
        for(; i< argc-2; i++){
            if(strcmp(argv[i],"--") == 0) break;
            char** splited = strsplit(argv[i], "=");
            if(splited[1][0] == '%'){
                char*c = getenv(splited[1]+1);
                flag = setenv(splited[0],c,1);
                if(flag == -1) print_environment_change_failed();
            } else{
                flag = setenv(splited[0],splited[1],1);
                if(flag == -1) print_environment_change_failed();
            }
            free(splited);
        }
        execvp(argv[i+1],&argv[i+1]);
        print_exec_failed();
    }else{
        int status;
        waitpid(pid,&status,0);
        return 0;
    }
    return 0;
}
