#include "scheduler.h"

typedef struct queueCDT * queueADT;

void queue(queueADT * my_queue, processADT * process);
char isEmpty(queueADT * my_queue);
processADT * dequeue(queueADT * my_queue);
queueADT createQueue();