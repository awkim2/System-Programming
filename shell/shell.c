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
extern char *optarg;
extern int optind, opterr, optopt;


typedef struct process {
    char *command;
    pid_t pid;
} process;





int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.

    // vector* history = string_vector_create();
    // char* history_file = NULL;

    //bad input
    if(!(argc == 1 || argc == 3 || argc == 5)){
        print_usage();
        exit(1);
    }


    char cwd[PATH_MAX+1];
    int pid = getpid();
    //char input[1024];

        //Prompting input 
        if (getcwd(cwd, sizeof(cwd))){
            print_prompt(cwd, pid);
        }



    return 0;
}
