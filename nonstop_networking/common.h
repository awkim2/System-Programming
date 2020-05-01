/**
 * Nonstop Networking
 * CS 241 - Fall 2019
 */
#pragma once
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#define LOG(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0);

typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;
ssize_t read_from_socket(int socket, char *buffer, size_t count);
ssize_t write_to_socket(int socket, const char *buffer, size_t count);
size_t read_head(int socket, char *buffer, size_t count);
int error_dectect(size_t size1, size_t size2);
