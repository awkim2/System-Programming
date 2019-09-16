/**
 * Utilities Unleashed
 * CS 241 - Fall 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h> 
#include <unistd.h>
#include "format.h"
int main(int argc, char *argv[]) {
    if(argc<2) print_time_usage();
    struct timespec begin;
    struct timespec end;
    int pid = fork();
    if(pid  == -1) print_fork_failed();
    clock_gettime(CLOCK_MONOTONIC, &begin);

    //child
    if(pid == 0){
        int stat = execvp(argv[1], &argv[1]);
        if(stat == -1) print_exec_failed();
        exit(1);
    }   
    if(pid != 0){
        int status = 0;
        waitpid(pid, &status,0);
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_ = end.tv_sec - begin.tv_sec + ((end.tv_nsec - begin.tv_nsec)/1000000000.0);
        if(WIFEXITED(status)) display_results(argv, time_);
    }
    return 0;
}
