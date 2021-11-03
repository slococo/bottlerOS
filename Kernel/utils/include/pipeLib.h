#ifndef PIPELIB_H
#define PIPELIB_H

#include "semLib.h"

#define MAX_NAME 100
#define PIPE_MAX_SIZE 1024

typedef struct pipe_t {
    int fd[2];
    char * buffer;
    int currentR;
    int currentW;
    char * name;
    sem_t * sem;
    sem_t * fullSem;
} pipe_t;

typedef struct node_t {
    pipe_t * pipe;
    struct node_t * next;
} node_t;

char openPipe(int * fds, char * name);
void writePipe(int fd, char c);
char readPipe(int fd);
char * pipes();
void closePipe(int fd);

#endif