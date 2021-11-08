// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phyloLib.h"

philosopher_t *firstPhil;
sem_t *mutex;
int philoCount = STARTING;
 
void printState() {
    philosopher_t * phil = firstPhil;
    do {
        if (phil->state == EATING)
            putChar('E');
        else putChar('.');
        phil = phil->right;
    } while (phil != firstPhil);
    newline();
}

void test(philosopher_t * phil)
{
    if (phil->state == HUNGRY && phil->left->state != EATING && phil->right->state != EATING) {
        phil->state = EATING;
        sys_semPost(phil->sem);
    }
}

void addPhilo() {
    philoCount++;

    philosopher_t * new = sys_malloc(sizeof(philosopher_t));
    new->argv = sys_malloc(sizeof(char *) * ARGV_SIZE);
    new->buffer = sys_malloc(sizeof(char) * BUFF_SIZE);
    new->state = THINKING;
    char * aux = sys_malloc(sizeof(char) * 20);
    aux[0] = '\0';
    aux = strcat(aux, "filosofo");
    char buffer[2] = {0};
    aux = strcat(aux, itoa(philoCount, buffer, 10));
    new->sem = sys_semOpen(aux, 0);

    new->argv[0] = "filosofo";
    strcpy(new->buffer, itoa((uint64_t) new, new->buffer, 10));
    new->argv[1] = new->buffer;

    new->left = firstPhil->left;
    new->right = firstPhil;

    firstPhil->left->right = new;
    firstPhil->left = new;

    new->pid = sys_loadProcess(philosopher, 0, 2, new->argv, NULL);
}

void removePhilo() {
    philoCount--;

    philosopher_t * del = firstPhil->left;
    del->left->right = del->right;
    del->right->left = del->left;

    sys_semClose(del->sem);
    sys_free(del->buffer);
    sys_free(del->argv);
    sys_free(del);

    sys_kill(del->pid);
}

void take_fork(philosopher_t * phil) {
    sys_semWait(mutex);
 
    phil->state = HUNGRY;
    test(phil);
 
    sys_semPost(mutex);
 
    sys_semWait(phil->sem);
}
 
void put_fork(philosopher_t * phil) {
    sys_semWait(mutex);
 
    phil->state = THINKING;
 
    test(phil->left);
    test(phil->right);
 
    sys_semPost(mutex);
}
 
void philosopher(int argc, char ** argv) {
    if (argc != 2) {
        sys_exit();
    }
    
    philosopher_t * i = (philosopher_t *) ((uint64_t) atoi(argv[1]));

    while (1) {
        sys_sleep(1);
 
        take_fork(i);
        printState();
 
        sys_sleep(1);
 
        put_fork(i);
        printState();
    }
    sys_exit();
}

void phylo(int argc, char ** argv) {
    if (argc == 1)
        philoCount = STARTING;
    else if (argc == 2) {
        philoCount = atoi(argv[1]);
        if (philoCount < 1 || philoCount > MAX_PHILO_SIZE) {
            printStringLen("amount of philosofers must be between 1 and 10\n", 48);
            sys_exit();
        }
    }
    else sys_exit();

    mutex = sys_semOpen("Mutex", 1);

    philosopher_t * left = NULL;
    for (int i = 0; i < philoCount; i++) {
        philosopher_t * phil = sys_malloc(sizeof(philosopher_t));
        phil->argv = sys_malloc(sizeof(char *) * 2);
        phil->buffer = sys_malloc(20);
        phil->state = THINKING;
        char * aux = sys_malloc(sizeof(char) * 20);
        aux[0] = '\0';
        aux = strcat(aux, "filosofo");
        char buffer[2] = {0};
        aux = strcat(aux, itoa(i, buffer, 10));
        phil->sem = sys_semOpen(aux, 0);

        phil->argv[0] = "filosofo";
        strcpy(phil->buffer, itoa((uint64_t) phil, phil->buffer, 10));
        phil->argv[1] = phil->buffer;

        if (left != NULL) {
            phil->left = left;
            phil->left->right = phil;
        }

        if (firstPhil == NULL) {
            firstPhil = phil;
        }

        left = phil;
    }

    firstPhil->left = left;
    firstPhil->left->right = firstPhil;


    philosopher_t * phil = firstPhil;

    do {
        phil->pid = sys_loadProcess(philosopher, 0, 2, phil->argv, NULL);
        phil = phil->right;
    } while (phil != firstPhil);
    
    char c;
    while ((c = getChar()) != 0 && c != -1) {
        if (c == 'a') {
            addPhilo();
        }
        else if (c == 'r') {
            removePhilo();
        }
        else if (c == 'q') {
            end();
        }
    }
    sys_exit();
}

void freePhilo(philosopher_t * phil) {
    sys_semClose(phil->sem);
    sys_free(phil);
    sys_free(phil->buffer);
    sys_free(phil->argv);
    sys_kill(phil->pid);
}

void end() {
    sys_semWait(mutex);
    philosopher_t * phil = firstPhil->right;
    do {
       freePhilo(phil);
       phil = phil->right;
    } while (phil != firstPhil);


    freePhilo(firstPhil);
    sys_semPost(mutex);

    sys_semClose(mutex);
    sys_exit();
}

/*
 * Inspirados en "Operating Systems - Design and Implementation" de Tanenbaum
 * Sección 2.3, pág. 91
*/