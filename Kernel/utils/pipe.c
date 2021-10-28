#include "pipe.h"

static int fdIndex = 2;

node_t * firstPipe;

char openPipe(int fds[2], char * name) {
    pipe_t * pipe = pvPortMalloc(sizeof(pipe_t));
    pipe->name = pvPortMalloc(MAX_NAME);
    pipe->buffer = pvPortMalloc(PIPE_MAX_SIZE);
    strcpy(pipe->name, name);
    
    if ((pipe->sem = semOpen(SEM_NAME, 1)) == NULL)
        return EXIT_FAILURE;

    node_t * node = pvPortMalloc(sizeof(node_t));
    node->pipe = pipe;
    node->next = firstPipe;
    firstPipe = node;

    pipe->currentR = 0;
    pipe->currentW = 0;
    pipe->fd[0] = fdIndex++;
    pipe->fd[1] = fdIndex++;
    fds[0] = pipe->fd[0];
    fds[1] = pipe->fd[1];

    return EXIT_SUCCESS;
}

node_t * searchPipe(node_t ** previous, int fd) {
    node_t * curr = firstPipe;
    * previous = NULL;
    while (curr != NULL) {
        if (curr->pipe->fd[0] == fd || curr->pipe->fd[1] == fd) {
            break;
        }
        * previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        * previous = NULL;
        return NULL;
    }
    return curr;
}

node_t * searchWPipe(node_t ** previous, int fd) {
    node_t * curr = firstPipe;
    * previous = NULL;
    while (curr != NULL) {
        if (curr->pipe->fd[1] == fd) {
            break;
        }
        * previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        * previous = NULL;
        return NULL;
    }
    return curr;
}

node_t * searchRPipe(node_t ** previous, int fd) {
    node_t * curr = firstPipe;
    * previous = NULL;
    while (curr != NULL) {
        if (curr->pipe->fd[0] == fd) {
            break;
        }
        * previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        * previous = NULL;
        return NULL;
    }
    return curr;
}

void writePipe(int fd, char c) {
    node_t * prev = NULL;
    node_t * node = searchWPipe(&prev, fd);

    semWait(node->pipe->sem);

    node->pipe->buffer[node->pipe->currentW++ % PIPE_MAX_SIZE] = c;

    semPost(node->pipe->sem);
}

char readPipe(int fd) {
    node_t * prev = NULL;
    node_t * node = searchRPipe(&prev, fd);

    semWait(node->pipe->sem);

    char c = node->pipe->buffer[node->pipe->currentR++ % PIPE_MAX_SIZE];

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
    else firstPipe = del->next;

    semClose(del->pipe->sem);
    vPortFree(del->pipe->fd);
    vPortFree(del->pipe->name);
    vPortFree(del->pipe->buffer);
    vPortFree(del->pipe);
    vPortFree(del);
}