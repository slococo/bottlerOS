#include "pipe.h"

int fds = 2;

node_t * firstPipe;

pipe_t * openPipe(char * name) {
    pipe_t * pipe = pvPortMalloc(sizeof(pipe_t));
    strcpy(pipe->name, name);
    pipe->fd = fds++;
    if ((pipe->sem = semOpen(SEM_NAME, 1)) == NULL)
        return NULL;

    return pipe;
}

void writePipe(int fd, char c) {
    node_t * prev = NULL;
    node_t * node = searchPipe(&prev, fd);

    semWait(node->pipe->sem);

    node->pipe->buffer[node->pipe->current++ % PIPE_MAX_SIZE] = c;

    semPost(node->pipe->sem);
}

char readPipe(int fd) {
    node_t * prev = NULL;
    node_t * node = searchPipe(&prev, fd);

    semWait(node->pipe->sem);

    char c = node->pipe->buffer[node->pipe->current-- % PIPE_MAX_SIZE];

    semPost(node->pipe->sem);

    return c;
}

void closePipe(int fd) {
    node_t * prev = NULL;
    node_t * del = searchPipe(&prev, fd);
    if (del == NULL)
        return;

    semWait(del->pipe->sem);
    if (prev != NULL)
        prev->next = del->next;
    else firstPipe->next = del->next;

    vPortFree(del->pipe);
    vPortFree(del);
    
    semPost(del->pipe->sem);
}

node_t * searchPipe(node_t ** previous, int fd) {
    node_t * curr = firstPipe;
    * previous = NULL;
    while (curr != NULL) {
        if (curr->pipe->fd == fd) {
            break;
        }
        * previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        * previous = NULL;
        return NULL;
    }
    if (curr == firstPipe) {
        * previous = NULL;
        return curr;
    }
    return curr;
}