/**
 * Nonstop Networking
 * CS 241 - Fall 2019
 */
#include "common.h"
#include "format.h"
#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include "./includes/dictionary.h"
#include "./includes/vector.h"

typedef struct C_info {
	int status;
	verb command;
	char filename[255];
	char header[1024];
	size_t t;
} C_info;

static int epoll_fd;
static dictionary  *client_dic;
static char* dir_;
static vector *file_vec;
static dictionary  *file_size;

void sig_pipe(){}

void quit(char* errormsg){
	perror(errormsg);
	exit(1);
}

void epoll_mod(int client_fd) {
	struct epoll_event eve_temp;
  	eve_temp.events = EPOLLOUT;
  	eve_temp.data.fd = client_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &eve_temp);
}

void process_cmd(int client_fd, C_info *info) {
  //GET 
	if (info->command == GET) {
		int len = strlen(dir_) + strlen(info->filename) + 1;
		char path[len];
		memset(path, 0, len);
		sprintf(path, "%s/%s", dir_, info->filename);
		FILE *r_mode = fopen(path, "r");
		if (!r_mode) {
			info->status = -3;
			return;
		}
		write_to_socket(client_fd, "OK\n", 3);
		size_t size;
		size = *(size_t *) dictionary_get(file_size, info->filename);
		write_to_socket(client_fd, (char*)&size, sizeof(size_t));
		size_t w_count = 0;
    	while (w_count < size) {
      		size_t size_head;
      		if( (size-w_count) > 1024){
        		size_head = 1024;
      		}else{
        		size_head =  (size - w_count);
      		}
      		char buffer[size_head + 1];
      		fread(buffer, 1, size_head, r_mode);
      		write_to_socket(client_fd, buffer, size_head);
      		w_count += size_head;
    	}
		fclose(r_mode);
    // PUT
	} else if (info->command == PUT) {
		write_to_socket(client_fd, "OK\n", 3);
    // DELETE
	} else if (info->command == DELETE) {
		int len = strlen(dir_) + strlen(info->filename) + 2;
		char path[len];
		memset(path, 0, len);
		sprintf(path, "%s/%s", dir_, info->filename);
		if (remove(path) < 0) {
			info->status = -3;
			return;
		}
		size_t i = 0;
		VECTOR_FOR_EACH(file_vec, name, {
	        if (!strcmp((char *) name, info->filename)) break;
			i++;
	 	});
		size_t v_size= vector_size(file_vec);
		if (i == v_size) {
			info->status = -3;
			return;
		}
		vector_erase(file_vec, i);
		dictionary_remove(file_size, info->filename);
		write_to_socket(client_fd, "OK\n", 3);
    // LIST
	} else if (info->command == LIST) {
		write_to_socket(client_fd, "OK\n", 3);
		size_t size = 0;
		VECTOR_FOR_EACH(file_vec, name, {
	         size += strlen(name)+1;
	    });
		if (size) size--;
		write_to_socket(client_fd, (char*)& size, sizeof(size_t));
		VECTOR_FOR_EACH(file_vec, name, {
		        write_to_socket(client_fd, name, strlen(name));
				if (_it != _iend-1) write_to_socket(client_fd, "\n", 1);
		});
	}
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
  	free(dictionary_get(client_dic, &client_fd));
	dictionary_remove(client_dic, &client_fd);
  	shutdown(client_fd, SHUT_RDWR);
	close(client_fd);
}

// if command is PUT 
int read_PUT_info(int client_fd, C_info *info) {
	int len = strlen(dir_) + strlen(info->filename) + 2;
	char path[len];
	memset(path , 0, len);
	sprintf(path, "%s/%s", dir_, info->filename);
	FILE *r_mode = fopen(path, "r");
	FILE *w_mode = fopen(path, "w");
	//cannot open the file 
	if (!w_mode) {
		perror("fopen");
		return 1;
	}
	// read bytes from clients and store it 
	size_t size;
	read_from_socket(client_fd, (char*) & size, sizeof(size_t));
	size_t r_count = 0;
	while (r_count < size + 5) {
		size_t size_head;
    	if((size + 5 - r_count) > 1024){
      		size_head = 1024;
    	}else{
      		size_head = (size + 5 - r_count);
    	}
		char buffer[1025] = {0};
		ssize_t count = read_from_socket(client_fd, buffer, size_head);
		if (count == -1) continue;
		fwrite(buffer, 1, count, w_mode);
		r_count += count;
		if( count == 0) break;
	}
	fclose(w_mode);
	if (error_dectect(r_count, size)) {
		remove(path);
		return 1;
	}
	if (!r_mode) {
		vector_push_back(file_vec, info->filename);
	} else {
		fclose(r_mode);
	}
	dictionary_set(file_size, info->filename, &size);
	return 0;
}

void read_header(int client_fd, C_info *info) {
	size_t count = read_head(client_fd, info->header, 1024);
	//read bad request
	if (count == 1024) {
		info->status = -1;
		epoll_mod(client_fd);
		return;
	}
	//setting up the info struct for each command 
	if (!strncmp(info->header, "GET", 3)) {
		info->command = GET;
		strcpy(info->filename, info->header + 4);
		info->filename[strlen(info->filename) - 1] = '\0';
	} else if (!strncmp(info->header, "PUT", 3)) {
		info->command = PUT;
		strcpy(info->filename, info->header + 4);
		info->filename[strlen(info->filename)-1] = '\0';
		if (read_PUT_info(client_fd, info) != 0) {
			info->status = -2;
			epoll_mod(client_fd);
			return;
		}
	} else if (!strncmp(info->header, "DELETE", 6)) {
		info->command = DELETE;
		strcpy(info->filename, info->header + 7);
		info->filename[strlen(info->filename) - 1] = '\0';
	} else if (!strncmp(info->header, "LIST", 4)) {
		info->command = LIST;
	} else {
		print_invalid_response();
		info->status = -1;
		epoll_mod(client_fd);
		return;
	}
	info->status = 1;
	epoll_mod(client_fd);
}

void run_client(int client_fd) {
	C_info* client_info = dictionary_get(client_dic, &client_fd);
  	int status = client_info->status;
  	// processing header info
	// reading head info for new client 
	if (status == 0) {
		read_header(client_fd, client_info);
	}else if (status == 1) {
		process_cmd(client_fd, client_info);
	}else{
		//error ocurrs 
		write_to_socket(client_fd, "ERROR\n", 6);
		if (status == -1) write_to_socket(client_fd, err_bad_request, strlen(err_bad_request));			//err_bad_request
    	if (status == -2) write_to_socket(client_fd, err_bad_file_size, strlen(err_bad_file_size));		//err_bad_file_size
	  	if (status == -3) write_to_socket(client_fd, err_no_such_file, strlen(err_no_such_file));		//err_no_such_file
    //shutdown and remove client_fd from epoll and close connection 
	  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
	  free(client_info);
	  dictionary_remove(client_dic, &client_fd);
	  shutdown(client_fd, SHUT_RDWR);
	  close(client_fd);
	}
}

void shutdown_server() {
	close(epoll_fd);
	vector *infos = dictionary_values(client_dic);
	VECTOR_FOR_EACH(infos, info, {
    	free(info);
	});
	vector_destroy(infos);
	dictionary_destroy(client_dic);
	remove(dir_);
	vector_destroy(file_vec);
	dictionary_destroy(file_size);
	exit(1);
}

void run_server(char* port){
  	//create server
  	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE;     // passive server
	int s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(1);
	}
	int optval = 1;
	//reuesed socket
	if ( setsockopt(socket_fd, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof(optval))) {
    	quit("setsocketopt");
	}
	if ( bind(socket_fd, result->ai_addr, result->ai_addrlen) != 0 ) {
    	quit("bind");
	}
	if ( listen(socket_fd, 100) != 0 ) {
    	quit("listen");
	}
  	freeaddrinfo(result);

  	//create epoll
	epoll_fd = epoll_create(50);
	if (epoll_fd == -1) quit("epoll_create");
  	//server epoll event  with socket fd
	struct epoll_event eve;
  	eve.events = EPOLLIN;
  	eve.data.fd = socket_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &eve);
	struct epoll_event epoll_arr[100];
	while (1) {
		int num_events = epoll_wait(epoll_fd, epoll_arr, 100, 10000);
		if (num_events == -1) exit(1);
		if (num_events == 0) continue;
		for (int i = 0; i < num_events; i++) {
			if (epoll_arr[i].data.fd == socket_fd) {
				int client_fd = accept(socket_fd, NULL, NULL);
				if (client_fd < 0) quit("accept");
        		//client epoll event with client fd
				struct epoll_event eve2;
        		eve2.events = EPOLLIN;
        		eve2.data.fd = client_fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &eve2);
				C_info *client_info = calloc(1, sizeof(C_info));
        		dictionary_set(client_dic, &client_fd, client_info);
			} else {
				run_client(epoll_arr[i].data.fd);
			}
		}
	}
}



int main(int argc, char **argv) {
    // good luck!
  //handle SIGPIPE
  signal(SIGPIPE, sig_pipe);
  if(argc != 2){
    print_server_usage();
    exit(1);  
  }
  
  //handle SIGINT 
  struct sigaction act;
  memset(&act, '\0', sizeof(act));
  act.sa_handler = shutdown_server;
  if (sigaction(SIGINT, &act, NULL) != 0) quit("sigaction");

  //make tempory diractory
  char dirname[] = "XXXXXX";
  dir_ = mkdtemp(dirname);
  print_temp_directory(dir_);
  //setting up global variable
  client_dic = int_to_shallow_dictionary_create();
  file_size = string_to_unsigned_long_dictionary_create();
  file_vec = string_vector_create();
	
  run_server(argv[1]);

}

