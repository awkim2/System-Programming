/**
 * Shell
 * CS 241 - Fall 2019
 */
#include "format.h"
#include "shell.h"
#include "vector.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
extern char *optarg;
extern int optind, opterr, optopt;

static int fail = 0;

typedef struct process {
    char *command;
    pid_t pid;
} process;

struct vector {
    /* The function callback for the user to define the way they want to copy
     * elements */
    copy_constructor_type copy_constructor;

    /* The function callback for the user to define the way they want to destroy
     * elements */
    destructor_type destructor;

    /* The function callback for the user to define the way they a default
     * element to be constructed */
    default_constructor_type default_constructor;

    /* Void pointer to the beginning of an array of void pointers to arbitrary
     * data. */
    void **array;

    /**
     * The number of elements in the vector.
     * This is the number of actual objects held in the vector,
     * which is not necessarily equal to its capacity.
     */
    size_t size;

    /**
     * The size of the storage space currently allocated for the vector,
     * expressed in terms of elements.
     */
    size_t capacity;
};


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

void buildin_cd(char* dir){
    int temp = chdir(dir);
    if(temp != 0){
        print_no_directory(dir);
        fail = 1;
        return;
    }

}

void buildin_history(vector* history){
    size_t i = 0;
    for(; i < vector_size(history); i++){
        print_history_line(i,vector_get(history,i));
    }
}

char* buildin_index(vector* history, char* buffer){
    char** sub = NULL;
    size_t temp = 0;
    sub = strsplit(buffer, "#");
    temp = atoi(sub[1]);
    if(temp >= vector_size(history)){
        print_invalid_index();
        return NULL;
    }
    print_command(history->array[temp]);
    return history->array[temp];
}


char* buildin_prefix(vector* history, char* buffer){
    int i = vector_size(history)-1;
    for(; i >= 0; i--){
        if(strstr(vector_get(history,i),buffer)){
            print_command(history->array[i]);
            return history->array[i];
        }
    }
    print_no_history_match();
    return NULL;
}

void extern_comm(char** spl){
    int pid = fork();
    if(pid == -1){
        print_fork_failed();
        fail = 1;
    }else if(pid == 0){
        int childpid = getpid();
        print_command_executed(childpid);
        int status = execvp(spl[0], &spl[0]);
        if(status == -1){
            print_exec_failed(spl[0]);
            fail = 1;
        }
        exit(1);
    }else{
        int status2;
        waitpid(pid,&status2,0);
        if(!WIFEXITED(status2)){
            print_wait_failed();
            fail = 1;
        }
    }
}

void command_set(char** spl, vector* history, char* buffer){

        if(!strcmp(spl[0], "cd")){
            buildin_cd(spl[1]);
        }

        if(!strcmp(spl[0], "!history")){
            buildin_history(history);
        }

}
void logic_and(char* buffer){
        char** logicspl = strsplit(buffer, "&&");
        char* comm1 = logicspl[0];
        char* comm2 = logicspl[1];
        char** comm1_spl = strsplit(comm1, " ");
        char** comm2_spl = strsplit(comm2, " ");
        if(!strcmp(comm1_spl[0], "cd")){
            buildin_cd(comm1_spl[1]);
        }else{
            extern_comm(comm1_spl);
        }

        if(fail != 1){
            if(!strcmp(comm2_spl[0], "cd")){
                buildin_cd(comm2_spl[1]);
            }else{
                extern_comm(comm2_spl+1);
            }
       }
}

int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.

    //bad input
    if(!(argc == 1 || argc == 3 || argc == 5)){
        print_usage();
        exit(1);
    }

    char cwd[PATH_MAX+1];
    int pid = getpid();
    vector* history = string_vector_create();
    char* buffer = NULL;
    size_t buffer_size;
    int len = 0;
    char** spl = NULL;
    while(1){
        if (getcwd(cwd, sizeof(cwd))){
            print_prompt(cwd, pid);
        }

        len = getline(&buffer, &buffer_size, stdin);
        buffer[len - 1] = 0;

        if(len == -1) break;

        if(strcmp(buffer, "exit") == 0) {
            break;
        }
        spl = strsplit(buffer, " ");
        
        //&&
        if(strstr(buffer, "&&")){
            logic_and(buffer);
            vector_push_back(history,buffer);
            continue;
        }

        if(!strcmp(spl[0], "cd")){
            buildin_cd(spl[1]);
            vector_push_back(history, buffer);
            continue;
        }

        if(!strcmp(spl[0], "!history")){
            buildin_history(history);
            continue;
        }
        
        if(buffer[0] == '#'){
            char* comm = NULL;
            comm = buildin_index(history, buffer);
            if(comm){
                char** splcomm = NULL;
                splcomm = strsplit(comm, " ");
                command_set(splcomm, history, comm);
                extern_comm(splcomm);
                vector_push_back(history, comm);
                continue;
            }
            continue;
        }

        if(buffer[0] == '!' && strcmp(buffer, "!history")){
            char* comm2 = NULL;
            comm2 = buildin_prefix(history, buffer+1);
            if(comm2){
                char** splcomm2 = NULL;
                splcomm2 = strsplit(comm2, " ");
                command_set(splcomm2, history, comm2);
                extern_comm(splcomm2);
                vector_push_back(history, comm2);
                continue;
            }
            continue;
        }

           extern_comm(spl);
           vector_push_back(history, buffer);
    }


    return 0;
}
