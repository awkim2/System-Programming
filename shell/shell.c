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

// struct vector {
//     /* The function callback for the user to define the way they want to copy
//      * elements */
//     copy_constructor_type copy_constructor;

//     /* The function callback for the user to define the way they want to destroy
//      * elements */
//     destructor_type destructor;

//     /* The function callback for the user to define the way they a default
//      * element to be constructed */
//     default_constructor_type default_constructor;

//     /* Void pointer to the beginning of an array of void pointers to arbitrary
//      * data. */
//     void **array;

//     /**
//      * The number of elements in the vector.
//      * This is the number of actual objects held in the vector,
//      * which is not necessarily equal to its capacity.
//      */
//     size_t size;

//     /**
//      * The size of the storage space currently allocated for the vector,
//      * expressed in terms of elements.
//      */
//     size_t capacity;
// };


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
    print_command(vector_get(history, temp));
    return vector_get(history, temp);
}


char* buildin_prefix(vector* history, char* buffer){
    int i = vector_size(history)-1;
    for(; i >= 0; i--){
        if(strstr(vector_get(history,i),buffer)){
            print_command(vector_get(history, i));
            return vector_get(history, i);
        }
    }
    print_no_history_match();
    return NULL;
}

void clear(int signal){
    int status;
    while(waitpid((pid_t) (-1), &status, WNOHANG) >0){}
}

void extern_comm(char** spl, char* buffer){
    if(buffer[strlen(buffer) -1] == '&'){
        char **comm = strsplit(buffer, "&");
        char **spl_comm = strsplit(comm[0], " ");
        int pid = fork();
        signal(SIGCHLD, clear);
        if(pid == -1){
            print_fork_failed();
            fail = 1;
        }else if(pid == 0){
            int child = getpid();
            if (setpgid(child, child) == -1) {
                    print_setpgid_failed();
                    fail = 1;
                    exit(1);
            }
            print_command_executed(getpid());
            int status = execvp(spl_comm[0],&spl_comm[0]);
            if(status == -1){
                print_exec_failed(spl_comm[0]);
                fail = 1;
            }
            exit(1);
        }else{
            return;
            }
    }else{
        int pid = fork();
        if(pid == -1){
            print_fork_failed();
            fail = 1;
            return;
        }else if(pid == 0){
            int childpid = getpid();
            print_command_executed(childpid);
            execvp(spl[0], &spl[0]);
            print_exec_failed(spl[0]);
            exit(1);
        }else{
            int status;
            waitpid(pid,&status,0);
            if(!(WIFEXITED(status) && WEXITSTATUS(status) == 0)){
                print_wait_failed();
                fail = 1;
            }
        }
    }
}

int command_set(char** spl, vector* history, char* buffer){

        if(!strcmp(spl[0], "cd")){
            buildin_cd(spl[1]);
            return 1;
        }

        if(!strcmp(spl[0], "!history")){
            buildin_history(history);
            return 1;
        }
    return 0;

}
void logic_and(char* buffer){
        fail = 0;
        char** logicspl = strsplit(buffer, "&&");
        char* comm1 = logicspl[0];
        char* comm2 = logicspl[2];
        if (comm2[0] == ' ') memmove(comm2, comm2+1, strlen(comm2));
        char** comm1_spl = strsplit(comm1, " ");
        char** comm2_spl = strsplit(comm2, " ");
        if(!strcmp(comm1_spl[0], "cd")){
            buildin_cd(comm1_spl[1]);
        }else{
            extern_comm(comm1_spl, comm1);
            
        }
        
        if(fail == 0){
            if(!strcmp(comm2_spl[0], "cd")){
                buildin_cd(comm2_spl[1]);
            }else{
                extern_comm(comm2_spl, comm2);
            }
       }

}


void logic_or(char* buffer){
        fail = 0;
        char** logicspl = strsplit(buffer, "||");
        char* comm1 = logicspl[0];
        char* comm2 = logicspl[2];
        if (comm2[0] == ' ') memmove(comm2, comm2+1, strlen(comm2));
        char** comm1_spl = strsplit(comm1, " ");
        char** comm2_spl = strsplit(comm2, " ");
        if(!strcmp(comm1_spl[0], "cd")){
            buildin_cd(comm1_spl[1]);
        }else{
            extern_comm(comm1_spl, comm1);
        }

        if(fail == 1){
            if(!strcmp(comm2_spl[0], "cd")){
                buildin_cd(comm2_spl[1]);
            }else{
                extern_comm(comm2_spl, comm2);
            }
       }
}

void logic_both(char* buffer){
        char** logicspl = strsplit(buffer, ";");
        char* comm1 = logicspl[0];
        char* comm2 = logicspl[1];
        if (comm2[0] == ' ') memmove(comm2, comm2+1, strlen(comm2));
        char** comm1_spl = strsplit(comm1, " ");
        char** comm2_spl = strsplit(comm2, " ");

        if(!strcmp(comm1_spl[0], "cd")){
            buildin_cd(comm1_spl[1]);
        }else{
            extern_comm(comm1_spl, comm1);
        }

        if(!strcmp(comm2_spl[0], "cd")){
            buildin_cd(comm2_spl[1]);
        }else{
            extern_comm(comm2_spl, comm2);
        }

}

int logic_set(char* buffer){
        //&&
        if(strstr(buffer, "&&")){
            logic_and(buffer);
            return 1;
        }
        //||
        if(strstr(buffer, "||")){
            logic_or(buffer);
            return 1;
        }
        //;
        if(strstr(buffer, ";")){
            logic_both(buffer);
            return 1;
        }
        return 0;
}

void vv(){

}

vector* read_file(char* file){
    FILE* fd = fopen(get_full_path(file), "r");
    if (fd == NULL) print_script_file_error();
    vector* comm_vec = string_vector_create();
    char* buffer = NULL;
    size_t len = 0;
    while(getline(&buffer, &len, fd) != -1) {
        if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer)-1] = 0;
        vector_push_back(comm_vec, buffer);
    }
    fclose(fd);
    return comm_vec;
}

char* load_history(char* file, vector* history){
    FILE* fd = fopen(get_full_path(file), "r");
    if (fd == NULL){
        print_script_file_error();
        fd = fopen(file, "w");
        fclose(fd);
        return get_full_path(file);
    }else{
        char* buffer = NULL;
        size_t len = 0;
        while(getline(&buffer, &len, fd) != -1){
            if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = 0;
            vector_push_back(history, buffer);
        }
    fclose(fd);
    return get_full_path(file);
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
    int res = 0;
    char* history_file_path = NULL;
    int is_h = 0;
    while((res = getopt(argc, argv, "f:h:")) != -1){
        if(res == 'f'){
            vector* comm_vec = read_file(optarg);
            size_t i = 0;
            for(; i < vector_size(comm_vec); i++) {
                char* fcomm = vector_get(comm_vec, i);
                print_command(fcomm);
                char** fsplcomm = NULL;
                fsplcomm = strsplit(fcomm, " ");
                if (logic_set(fcomm) == 1){
                    vector_push_back(history, fcomm);
                    continue;
                }
                if(command_set(fsplcomm, history, fcomm) == 1){
                    vector_push_back(history, fcomm);
                    continue;
                }
                extern_comm(fsplcomm, fcomm);
                vector_push_back(history, fcomm);
                continue;
            }
            return 0;
        }else if(res == 'h'){
           history_file_path = load_history(optarg, history);
           is_h =1;
        }   
    }

    while(1){
        signal(SIGINT, vv);
        if (getcwd(cwd, sizeof(cwd))){
            print_prompt(cwd, pid);
        }

        len = getline(&buffer, &buffer_size, stdin);
        buffer[len - 1] = 0;
        if(len == 1){
            vector_push_back(history, buffer);
            continue;
        }
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
        //||
        if(strstr(buffer, "||")){
            logic_or(buffer);
            vector_push_back(history,buffer);
            continue;
        }
        //;
        if(strstr(buffer, ";")){
            logic_both(buffer);
            vector_push_back(history,buffer);
            continue;
        }
        //cd
        if(!strcmp(spl[0], "cd")){
            buildin_cd(spl[1]);
            vector_push_back(history, buffer);
            continue;
        }
        //!history
        if(!strcmp(spl[0], "!history")){
            buildin_history(history);
            continue;
        }
        // #
        if(buffer[0] == '#'){
            char* comm = NULL;
            comm = buildin_index(history, buffer);
            if(comm){
                char** splcomm = NULL;
                splcomm = strsplit(comm, " ");
                if (logic_set(comm) == 1){
                    vector_push_back(history, comm);
                    continue;
                }
                if(command_set(splcomm, history, comm) == 1){
                    vector_push_back(history, comm);
                    continue;
                }
                extern_comm(splcomm, comm);
                vector_push_back(history, comm);
                continue;
            }
            continue;
        }
        // !
        if(buffer[0] == '!' && strcmp(buffer, "!history")){
            char* comm2 = NULL;
            comm2 = buildin_prefix(history, buffer+1);
            if(comm2){
                char** splcomm2 = NULL;
                splcomm2 = strsplit(comm2, " ");
                if (logic_set(comm2) == 1){
                    vector_push_back(history, comm2);
                    continue;
                }
                if(command_set(splcomm2, history, comm2) == 1){
                    vector_push_back(history, comm2);
                    continue;
                }
                extern_comm(splcomm2, comm2);
                vector_push_back(history, comm2);
                continue;
            }
            continue;
        }
        // external 
        extern_comm(spl, buffer);
        vector_push_back(history, buffer);
        
    }
        if(is_h == 1){
            FILE* fd = fopen(history_file_path, "w");
            size_t i = 0;
            for(; i < vector_size(history); i++){
                fprintf(fd, "%s\n", vector_get(history, i));
            }
        }

    return 0;
}
