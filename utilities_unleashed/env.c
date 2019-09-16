/**
 * Utilities Unleashed
 * CS 241 - Fall 2019
 */


#include "format.h"
#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// char** split(char* ele){
//     char** res = calloc(2, sizeof(char*));
//     char* first = ele;
//     char* sec = ele;

// }


int main(int argc, char *argv[]) {
    if(argc < 3) print_env_usage();




    // int pid = fork();
    // if(pid < 0) print_fork_failed();
    // if(pid == 0){
    //     //child
    //     int exce1 = execvp();
    //     if(exce1 == -1) print_exec_failed();
    //     int pid2 = fork();
    //     if(pid2 < 0) print_fork_failed();
    //     if(pid == 0){
    //         int exce2 = execvp();
    //         if(exce2 == -1) print_exec_failed();
    //         exit(1);
    //     }else{
    //         int status2 = 0;
    //         waitpid(pid2, $status2, 0);
    //     }
    //     exit(1);
    // }else{
    //     int status = 0;
    //     waitpid(pid, &status, 0);

    // }
    return 0;
}
