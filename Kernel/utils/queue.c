#include "queue.h"
#include "memManager.h"
#include <stdint.h>

typedef struct nodeT {
    processADT * process;
    struct nodeT * next;
} nodeT;

typedef struct queueCDT {
    nodeT * first;
    nodeT * last;
    int length;
} queueCDT;

queueADT createQueue() {
    queueADT aux = pvPortMalloc(sizeof(queueCDT));
    if (aux == NULL)
        return NULL;
    aux->first = NULL;
    aux->last = NULL;
    aux->length = 0;
    return aux;
}

void queue(queueADT * my_queue, processADT * process) {
    if (my_queue == NULL)
        return;
    nodeT * new_node = pvPortMalloc(sizeof(nodeT));
    if (new_node == NULL)
        return;
    new_node->process = process;
    new_node->next = NULL;
    (*my_queue)->last->next = new_node;
    (*my_queue)->last = new_node;
    (*my_queue)->length++;
}

processADT * dequeue(queueADT * my_queue) {
    if (my_queue == NULL || isEmpty(my_queue))
        return NULL;
    nodeT * aux = (*my_queue)->first;
    (*my_queue)->first = (*my_queue)->first->next;
    processADT * ans = aux->process;
    vPortFree(aux);
    return ans;
}

char isEmpty(queueADT * my_queue) {
    return (*my_queue)->length == 0;
}