#include "pipe.h"

static int fdIndex = 2;

node_t *firstPipe;

char openPipe(int fds[2], char *name) {
    pipe_t *pipe = pvPortMalloc(sizeof(pipe_t));
    pipe->name = pvPortMalloc(MAX_NAME);
    pipe->buffer = pvPortMalloc(PIPE_MAX_SIZE);
    strcpy(pipe->name, name);

    if ((pipe->sem = semOpen(SEM_NAME, 1)) == NULL)
        return EXIT_FAILURE;

    if ((pipe->fullSem = semOpen(FULL_SEM_NAME, 0)) == NULL)
        return EXIT_FAILURE;

    node_t *node = pvPortMalloc(sizeof(node_t));
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

node_t *searchPipe(node_t **previous, int fd) {
    node_t *curr = firstPipe;
    *previous = NULL;
    while (curr != NULL) {
        if (curr->pipe->fd[0] == fd || curr->pipe->fd[1] == fd) {
            break;
        }
        *previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        *previous = NULL;
        return NULL;
    }
    return curr;
}

node_t *searchWPipe(node_t **previous, int fd) {
    node_t *curr = firstPipe;
    *previous = NULL;
    while (curr != NULL) {
        if (curr->pipe->fd[1] == fd) {
            break;
        }
        *previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        *previous = NULL;
        return NULL;
    }
    return curr;
}

node_t *searchRPipe(node_t **previous, int fd) {
    node_t *curr = firstPipe;
    *previous = NULL;
    while (curr != NULL) {
        if (curr->pipe->fd[0] == fd) {
            break;
        }
        *previous = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        *previous = NULL;
        return NULL;
    }
    return curr;
}

void writePipe(int fd, char c) {
    node_t *prev = NULL;
    node_t *node = searchWPipe(&prev, fd);

    semWait(node->pipe->sem);

    node->pipe->buffer[node->pipe->currentW++ % PIPE_MAX_SIZE] = c;

    semPost(node->pipe->sem);

    semPost(node->pipe->fullSem);
}

char readPipe(int fd) {
    node_t *prev = NULL;
    node_t *node = searchRPipe(&prev, fd);

    semWait(node->pipe->fullSem);

    semWait(node->pipe->sem);

    char c = node->pipe->buffer[node->pipe->currentR++ % PIPE_MAX_SIZE];

    semPost(node->pipe->sem);

    return c;
}

void closePipe(int fd) {
    node_t *prev = NULL;
    node_t *del = searchPipe(&prev, fd);
    if (del == NULL)
        return;

    semWait(del->pipe->sem);
    if (prev != NULL)
        prev->next = del->next;
    else firstPipe = del->next;

    semClose(del->pipe->sem);
    semClose(del->pipe->fullSem);
    vPortFree(del->pipe->fd);
    vPortFree(del->pipe->name);
    vPortFree(del->pipe->buffer);
    vPortFree(del->pipe);
    vPortFree(del);
}

void getGenPipeData(char **out, char *written, char toAdd, char *in, char isLast) {
    char copied = strcpy(*out, in);
    *out += copied;
    *out += addSpaces(*out, toAdd - copied);
    *written += toAdd;
    if (!isLast) {
        *out += addSpaces(*out, 2);
        *written += 2;
    }
}

char getPipeData(char *out, node_t *node) {
    if (node == NULL)
        return EXIT_FAILURE;

    char written = 0;

    char flag = 0;
    for (int j = 0; j < MAX_NAME_SIZE; j++) {
        if (!flag && node->pipe->name[j] == 0)
            flag = 1;
        else if (flag)
            out += addSpaces(out, 1);
        else
            *out++ = node->pipe->name[j];
    }
    written += MAX_NAME_SIZE;

    out += addSpaces(out, 2);
    written += 2;

    char buffer[10];
    getGenPipeData(&out, &written, MAX_ATTR_SIZE, itoa(node->pipe->fd[0], buffer, 10, 2), 0);
    getGenPipeData(&out, &written, MAX_ATTR_SIZE, itoa(node->pipe->fd[1], buffer, 16, 10), 0);
    char *aux = getEntering(node->pipe->sem);
    getGenPipeData(&out, &written, MAX_PIDS_SIZE, aux, 1);
    vPortFree(aux);

    return written;
}

char *pipes() {
    char *ans = pvPortMalloc((fdIndex / 2) * PIPE_DATA_MAX_SIZE);
    char *ret = ans;

    char *info = "name       fdIn    fdOut    pids\n";
    ans += strcpy(ans, info);

    node_t *aux = firstPipe;
    while (aux != NULL) {
        char writtenChars = getPipeData(ans, aux);
        if (writtenChars == EXIT_FAILURE)
            return NULL;
        ans += writtenChars - 1;
        *ans++ = '\n';
        aux = aux->next;
    }
    *--ans = 0;

    return ret;
}