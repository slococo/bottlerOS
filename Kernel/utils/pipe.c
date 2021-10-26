#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "lib.h"
#include "scheduler.h"
#include "memManager.h"
#include "semaphore.h"

#define MAX_SEM 100
#define MAX_NAME 100
#define PIPE_MAX_SIZE 1024
#define SEM_NAME "Pipes"

typedef struct pipe_t {
    char buffer[PIPE_MAX_SIZE];
    int current;
    char name[MAX_NAME];
    sem_t * sem;
} pipe_t;

typedef struct node_t {
    pipe_t * pipe;
    struct node_t * next;
} node_t;

node_t * first;

// char initPipes() {
    // if ((sem = semOpen(SEM_NAME)) == NULL)
    //     return EXIT_FAILURE;
    // return EXIT_SUCCESS;
// }

pipe_t * openPipe(char * name) {
    // semWait(sem);

    pipe_t * pipe = pvPortMalloc(sizeof(pipe_t));
    strcpy(pipe->name, name);
    if ((pipe->sem = semOpen(SEM_NAME)) == NULL)
        return NULL;

    return pipe;
    // semPost(sem);
}

void writePipe(pipe_t * pipe, char c) {
    if (!exists(pipe))
        return;

    semWait(pipe->sem);

    pipe->buffer[pipe->current++ % PIPE_MAX_SIZE] = c;

    semPost(pipe->sem);
}

char readPipe(pipe_t * pipe) {
    if (!exists(pipe))
        return -1;

    semWait(pipe->sem);

    char c = pipe->buffer[pipe->current-- % PIPE_MAX_SIZE];

    semPost(pipe->sem);

    return c;
}

void closePipe(pipe_t * pipe) {
    node_t * prev = NULL;
    node_t * del = search(&prev, pipe);
    if (del == NULL)
        return;

    semWait(pipe->sem);
    if (prev != NULL)
        prev->next = del->next;
    else first->next = del->next;

    vPortFree(pipe);
    vPortFree(del);
    
    semPost(pipe->sem);
}

int exists(pipe_t * pipe) {
    node_t * prev = NULL;
    return search(&prev, pipe) != NULL;
}

node_t * search(node_t ** previous, pipe_t * pipe) {
    node_t * curr = first;
    * previous = NULL;
    while (curr != NULL) {
        if (curr->pipe == pipe) {
            break;
        }
        * previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        * previous = NULL;
        return NULL;
    }
    if (curr == first) {
        * previous = NULL;
        return curr;
    }
    return curr;
}