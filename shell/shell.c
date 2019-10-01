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
#include <ctype.h>
#include <dirent.h> 


extern char *optarg;
extern int optind, opterr, optopt;


static int fail = 0;


typedef struct process {
    char *command;
    pid_t pid;
} process;

static vector *process_vector;

process* process_create(char* buffer, pid_t pid){
    process *new_proc = calloc(1, sizeof(process));
    new_proc->command = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(new_proc->command, buffer);
    new_proc->pid = pid;
    return new_proc;
}

process_info* info_create(char* command, pid_t pid){
  process_info *info = calloc(1, sizeof(process_info));
  info->command = calloc(strlen(command)+1, sizeof(char));
  strcpy(info->command, command);
  info->pid = pid;
  char path[40];
  char line[1000];
  char* it;
  snprintf(path, 40, "/proc/%d/status", pid);
  FILE *fp = fopen(path,"r");
  if (!fp) {
      print_script_file_error();
      exit(1);
  }
  while(fgets(line, 100, fp)) {
      if(!strncmp(line, "State:", 6)) {
        it = line + 7;
        while(isspace(*it)) ++it;
        info->state = *it;
      } else if (!strncmp(line, "Threads:", 8)) {
        it = line + 9;
        while(isspace(*it)) ++it;
        info->nthreads = strtol(it, NULL, 10);
      } else if (!strncmp(line, "VmSize:", 7)) {
        it = line + 8;
        while(isspace(*it)) ++it;
        info->vsize = strtol(it, NULL, 10);
      }
  }
  fclose(fp);

  snprintf(path, 40, "/proc/%d/stat", pid);
  FILE *fp2 = fopen(path,"r");
  if (!fp2) {
      print_script_file_error();
      exit(1);
  }
  fgets(line, 1000, fp2);
  fclose(fp2);
  it = strtok(line, " ");
  int i = 0;
  unsigned long utime, stime;
  unsigned long long starttime;
  while(it != NULL)
	{
    if (i == 13) {
      utime = strtol(it, NULL, 10);
    } else if (i == 14) {
      stime = strtol(it, NULL, 10);
    } else if (i == 21) {
      starttime = strtol(it, NULL, 10);
    }
    it = strtok(NULL, " ");
	i++;
	}

  char buffer_cpu[100];
  unsigned long total_seconds_cpu = (utime+stime)/sysconf(_SC_CLK_TCK);
  if (!execution_time_to_string(buffer_cpu, 100, total_seconds_cpu/60, total_seconds_cpu%60)) exit(1);
  info->time_str = calloc(strlen(buffer_cpu)+1, sizeof(char));
  strcpy(info->time_str, buffer_cpu);

  FILE *fp3 = fopen("/proc/stat","r");
  if (!fp3) {
      print_script_file_error();
      exit(1);
  }
  unsigned long btime;
  while(fgets(line, 100, fp3)) {
      if(!strncmp(line, "btime", 5)) {
        it = line + 6;
        while(isspace(*it)) ++it;
        btime = strtol(it, NULL, 10);
      }
  }
  fclose(fp3);
  char buffer_start[100];
  time_t total_seconds_start = starttime/sysconf(_SC_CLK_TCK) + btime;
  struct tm *tm_info = localtime(&total_seconds_start);
  if (!time_struct_to_string(buffer_start, 100, tm_info)) exit(1);
  info->start_str = calloc(strlen(buffer_start)+1, sizeof(char));
  strcpy(info->start_str, buffer_start);
  return info;
}

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
    // background extern
    if(buffer[strlen(buffer) -1] == '&'){
        if(buffer[strlen(buffer) -2] == ' '){
            buffer[strlen(buffer) -2] = 0;
        }
        char **spl_comm = strsplit(buffer, " ");
        int pid = fork();
        signal(SIGCHLD, clear);
        if(pid == -1){
            print_fork_failed();
            fail = 1;
        }else if(pid == 0){
            //child
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
            //parent
            process *pro = process_create(buffer, pid);
            vector_push_back(process_vector, pro);
            }
    }else{
        // normal extern 
        int pid = fork();
        if(pid == -1){
            print_fork_failed();
            fail = 1;
            return;
        }else if(pid == 0){
            //child
            int childpid = getpid();
            print_command_executed(childpid);
            execvp(spl[0], &spl[0]);
            print_exec_failed(spl[0]);
            exit(1);
        }else{
            //parent
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
        comm1[strlen(comm1) - 1] = 0;
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
        comm1[strlen(comm1) - 1] = 0;
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

unsigned long file_getposition(FILE *fd){
    char line[100];
    char *it;
    unsigned long pos;
    while(fgets(line, 100, fd)) {
        if(!strncmp(line, "pos:", 4)) {
                it = line + 4;
                while(isspace(*it)) ++it;
                pos = strtol(it, NULL, 10);
        }
    } 
    return pos;
}

void command_ps(){
    print_process_info_header();
    size_t i = 0;
    for (; i < vector_size(process_vector); i++) {
        if(kill(((process *) vector_get(process_vector, i))->pid,0) != -1){
            process *pro = (process *) vector_get(process_vector, i);
            process_info *prcs_info = info_create(pro->command, pro->pid);
            print_process_info(prcs_info);
        }
    }
}

void command_pfd(pid_t pid){
    size_t i = 0;
    for (; i < vector_size(process_vector); i++) {
        process *pro = (process *) vector_get(process_vector, i);
        if (pro->pid == pid) {
            char path[30];
            snprintf(path, 30, "/proc/%d/fdinfo", pid);
            DIR *d = opendir(path);
            if (!d) {
                print_script_file_error();
                return;
            }
            print_process_fd_info_header();
            struct dirent* dir;
            char line[50];
            unsigned long pre;
            while((dir = readdir(d))){
                unsigned long fd = strtol(dir->d_name, NULL, 10);
                if(fd == pre) continue;
                snprintf(line,50,"/proc/%d/fdinfo/%lu",pid, fd);
                FILE *fp4 = fopen(line,"r");
                unsigned long file_pos = file_getposition(fp4);
                fclose(fp4);
                char realpath[100];
                char li[30]; 
                int link = 0;
                snprintf(li, 30, "/proc/%d/fd/%zu", pid, fd);
                link = readlink(li, realpath, 100);
                realpath[link] = '\0';
                print_process_fd_info(fd,file_pos, realpath);
                pre = fd;
            }
            return;
    }
  }
    print_no_process_found(pid);
}
void command_stop(pid_t pid){
    size_t i = 0;
//    if(kill(pid,0) != -1){
        for (;i < vector_size(process_vector); i++) {
            process *temp = (process*) vector_get(process_vector, i);
            if(temp->pid == pid){
                kill(pid, SIGTSTP);
                print_stopped_process(pid, temp->command);
                return;
            }
        }
    // }
    print_no_process_found(pid);
}

void command_kill(pid_t pid){
    size_t i = 0;
    // if(kill(pid,0) != -1){
        for (;i < vector_size(process_vector); i++) {
            process *temp = (process*) vector_get(process_vector, i);
            if(temp->pid == pid){
                kill(pid, SIGTERM);
                print_killed_process(pid, temp->command);
                return;
            }
        }
    // }
    print_no_process_found(pid);
}

void command_cont(pid_t pid){
    size_t i = 0;
    //if(kill(pid,0) != -1){
        for (;i < vector_size(process_vector); i++) {
            process *temp = (process*) vector_get(process_vector, i);
            if (temp->pid == pid){
                kill(pid, SIGCONT);
                return;
            }
        }
    //}
    print_no_process_found(pid);
}
int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
    //bad input
    if(!(argc == 1 || argc == 3 || argc == 5)){
        print_usage();
        exit(1);
    }
    signal(SIGINT, SIG_IGN);
    int pid = getpid();
    process_vector = shallow_vector_create();
    process* shell = process_create(argv[0], pid);
    vector_push_back(process_vector, shell);
    char cwd[PATH_MAX+1];
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
        
        if(!strcmp(buffer,"ps")){
            command_ps();
            continue;
        }
        if(!strncmp(buffer, "pfd", 3)){
            pid_t pid1;
            int status1 = sscanf(buffer+3, "%d",&pid1);
            if(status1 != 1){
                print_invalid_command(buffer);
            }else{
                command_pfd(pid1);
            }
            continue;
        }
        if(!strncmp(buffer, "stop", 4)){
            pid_t pid2; 
            int status2 = sscanf(buffer + 4, "%d", &pid2);
            if(status2 != 1){
                print_invalid_command(buffer);
            }else{
                command_stop(pid2);
            }
            continue;
        }

        if(!strncmp(buffer, "kill", 4)){
            pid_t pid4; 
            int status4 = sscanf(buffer + 4, "%d", &pid4);
            if(status4 != 1){
                print_invalid_command(buffer);
            }else{
                command_kill(pid4);
            }
            continue;
        }

        if(!strncmp(buffer, "cont", 4)) {
            pid_t pid3; 
            int status3 = sscanf(buffer + 4, "%d", &pid3);
            if(status3 != 1){
                print_invalid_command(buffer);
            }else{
                command_cont(pid3);
            }
            continue;
        }
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