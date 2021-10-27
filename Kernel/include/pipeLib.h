#ifndef PIPELIB_H
#define PIPELIB_H

#include "semLib.h"

#define MAX_NAME 100
#define PIPE_MAX_SIZE 1024

typedef struct pipe_t {
    int fd;
    char buffer[PIPE_MAX_SIZE];
    int current;
    char name[MAX_NAME];
    sem_t * sem;
} pipe_t;

typedef struct node_t {
    pipe_t * pipe;
    struct node_t * next;
} node_t;

pipe_t * openPipe(char * name);
void writePipe(int fd, char c);
char readPipe(int fd);

#endif