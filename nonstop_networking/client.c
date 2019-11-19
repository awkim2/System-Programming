/**
 * Nonstop Networking
 * CS 241 - Fall 2019
 */
#include "common.h"
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

char **parse_args(int argc, char **argv);
verb check_args(char **args);


void read_response_from_server(char **args, int socket, verb method) {
    char* ok = "OK\n";
    char* err = "ERROR\n";
    char* res = calloc(1,strlen(ok)+1);
    size_t read_by = read_from_socket(socket, res, strlen(ok));
    //if response from server is OK
    if (strcmp(res, ok) == 0) {
        fprintf(stdout, "%s", res);
        // DELETE or PUT responses OK
        if (method == DELETE || method == PUT) {
            print_success();
        }
        // GET responses OK
        if (method == GET) {
            FILE *local_file = fopen(args[4], "a+");
            if (!local_file) {
                perror("fopen");
                exit(-1);
            }
            size_t size;
            read_from_socket(socket, (char *)&size, sizeof(size_t));
            size_t rtotal = 0;
            size_t r_size;
            while (rtotal < size + 5) {
                if ((size + 5 - rtotal) > 1024){
                    r_size = 1024;
                }else{
                    r_size = size + 5 - rtotal;
                }
                char buffer[1025] = {0};
                size_t rcount = read_from_socket(socket, buffer, r_size);
                fwrite(buffer, 1, rcount, local_file);
                rtotal += rcount;
                if (rcount == 0) break;
            }
            //error detect
            if (rtotal == 0 && rtotal != size) {
                print_connection_closed();
                exit(-1);
            } else if (rtotal < size) {
                print_too_little_data();
                exit(-1);
            } else if (rtotal > size) {
                print_received_too_much_data();
                exit(-1);
            }
            fclose(local_file);
        }
        // LIST responses OK
        if (method == LIST) {
            size_t size;
            read_from_socket(socket, (char*)&size, sizeof(size_t));
            char buffer[size + 6];
            memset(buffer, 0, size + 6);
            read_by = read_from_socket(socket, buffer, size + 5);
            //error detect
            if (read_by == 0 && read_by != size) {
                print_connection_closed();
                exit(-1);
            } else if (read_by < size) {
                print_too_little_data();
                exit(-1);
            } else if (read_by > size) {
                print_received_too_much_data();
                exit(-1);
            }
            fprintf(stdout, "%zu%s", size, buffer);
        }
        //if response from server is ERROR
  } else {
        res = realloc(res, strlen(err)+1);
        read_from_socket(socket, res + read_by, strlen(err) - read_by);
        if (strcmp(res, err) == 0) {
            fprintf(stdout, "%s", res);
            char error_msg[20] = {0};
            if (!read_from_socket(socket, error_msg, 20)) print_connection_closed();
            print_error_message(error_msg);
        } else {
            print_invalid_response();
        }
    }
    free(res);
}

int main(int argc, char **argv) {
    // Good luck!
    //parse and check argv
    char** args = parse_args(argc, argv);
    verb method = check_args(args);

    //build connection 
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    char* host = args[0];
    char* port = args[1];
    int add_info = getaddrinfo(host, port, &hints, &result);
    if (add_info) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(add_info));
        exit(1);
    }
    int sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock_fd == -1) {
        perror("socket");
        exit(1);
    }
    int connection = connect(sock_fd, result->ai_addr, result->ai_addrlen);
    if (connection == -1) {
        perror("connect");
        exit(1);
    } 
    freeaddrinfo(result);
    
    //write method to server
    //write 1st line command to server
    char* str;
    if(method == LIST){
        str = calloc(1, strlen(args[2])+2);
        sprintf(str, "%s\n", args[2]);
    }else{
        str = calloc(1, strlen(args[2])+strlen(args[3])+3);
        sprintf(str, "%s %s\n", args[2], args[3]);
    }
    ssize_t len = strlen(str);
    ssize_t wcount = write_to_socket(sock_fd, str, len);
    if(wcount < len){
        print_connection_closed();
        exit(-1);
    }
    free(str);

    //if method is PUT, write size and binary data to server
    if(method == PUT){
        struct stat statbuf;
        int status = stat(args[4], &statbuf);
        if(status == -1) exit(-1);
        //write size
        size_t size = statbuf.st_size;
        write_to_socket(sock_fd, (char*)&size, sizeof(size_t));
        //write data
        FILE* local_file = fopen(args[4], "r");
        if(!local_file) exit(-1);
        ssize_t w_size;
        size_t wtotal = 0;
        while (wtotal < size) {
            if((size - wtotal) > 1024 ){
                w_size = 1024;
            }else{
                w_size = size - wtotal;
            }
            char buffer[w_size + 1];
            fread(buffer, 1, w_size, local_file);
            if (write_to_socket(sock_fd, buffer, w_size) < w_size) {
                print_connection_closed();
                exit(-1);
            }
            wtotal += w_size;
        }
        fclose(local_file);
    } 

    //shutdown the WRITE end of socket
    int status2 = shutdown(sock_fd, SHUT_WR);
    if(status2 != 0) perror("shutdown");
    //get response from server
    read_response_from_server(args, sock_fd, method);
    //shut down READ end of socket
    shutdown(sock_fd, SHUT_RD);

    //close socket and free memory
    close(sock_fd);
    free(args);
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
