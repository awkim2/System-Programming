/**
 * mapreduce Lab
 * CS 241 - Spring 2019
 * partner with guanhua2
 */

#include "utils.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if (argc != 6) {
      print_usage();
      return 1;
    }
    char *input = argv[1];
    char *output = argv[2];
    char *mapp = argv[3];
    char *reduce = argv[4];
    int total_count;
    if (sscanf(argv[5], "%d", &total_count) != 1 || total_count < 1) {
      print_usage();
      return 1;
    }
    // Create an input pipe for each mapp.
    int* fd_mapp[total_count];
    for (int i = 0; i < total_count; i++) {
      fd_mapp[i] = calloc(2, sizeof(int));
      pipe(fd_mapp[i]);
    }
    // Create one input pipe for the reduce.
    int fd_reduce[2];
    pipe(fd_reduce);
    // Open the output file.
    int fd_open = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
    // Start a splitter process for each mapp.
    pid_t child_s[total_count];
    int i = 0;
    for (; i < total_count; i++) {
      pid_t child = fork();
      child_s[i] = child;
      if (child == 0) {
        char str[10];
        sprintf(str, "%d", i);
        close(fd_mapp[i][0]);
        dup2(fd_mapp[i][1], 1);
        execl("./splitter", "./splitter", input, argv[5], str, NULL);
        exit(1);
      }
    }

    // Start all the mapp processes.
    pid_t child_m[total_count];
    for (int i = 0; i < total_count; i++) {
      close(fd_mapp[i][1]);
      pid_t child = fork();
      child_m[i] = child;
      if (child == 0) {
        close(fd_reduce[0]);
        dup2(fd_mapp[i][0], 0);
        dup2(fd_reduce[1], 1);
        execl(mapp, mapp, NULL);
        exit(1);
      }
    }

    // Start the reduce process.
    close(fd_reduce[1]);
    pid_t child = fork();
    if (child == 0) {
      dup2(fd_reduce[0], 0);
      dup2(fd_open, 1);
      execl(reduce, reduce, NULL);
      exit(1);
    }
    close(fd_reduce[0]);
    close(fd_open);
    // Wait for the reduce to finish.
    i = 0;
    for (;i < total_count; i++) {
      int status;
      waitpid(child_s[i], &status, 0);
    }
    i = 0;
    for (; i < total_count; i++) {
      close(fd_mapp[i][0]);
      int status;
      waitpid(child_m[i], &status, 0);
    }
    int status;
    waitpid(child, &status, 0);
    // Print nonzero subprocess exit codes.
    if (status) print_nonzero_exit_status(reduce, status);
    // Count the number of lines in the output file.
    print_num_lines(output);
    i = 0;
    for (;i < total_count; i++) {
      free(fd_mapp[i]);
    }
    return 0;
}
