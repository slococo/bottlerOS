#include "phylo.h"
#include <stddef.h>
#include <stdint.h>
 
#define ARGV_SIZE 2
#define BUFF_SIZE 20
#define MAX_PHILO_SIZE 3
#define MAX_NAME_SIZE 10
#define STARTING 2

// typedef uint64_t sem_t;
// typedef struct pid_t {
//     int pid;
//     struct pid_t * next;
// } pid_t;

// typedef struct sem_t {
//     unsigned int value;
//     // char name[MAX_NAME];
//     char * name;
//     pid_t * entering;
//     pid_t * last;
// } sem_t;

int * state;
typedef enum states {EATING = 0, HUNGRY, THINKING} states;

typedef struct philosopher_t {
    int debug;
    char ** argv;
    char * buffer;
    sem_t * sem;
    int pid;
    states state;

    struct philosopher_t * left;
    struct philosopher_t * right;
} philosopher_t;

philosopher_t * firstPhil;

sem_t * mutex;

int philoCount = STARTING;
 
void printState() {
    philosopher_t * phil = firstPhil;
    do {
        if (phil->state == EATING)
            putChar('E');
        else putChar('.');
        phil = phil->right;
    } while (phil != firstPhil);
    putChar('\n');
}

void test(philosopher_t * phil)
{
    if (phil->state == HUNGRY && phil->left->state != EATING && phil->right->state != EATING) {
        phil->state = EATING;
 
        // sys_sleep(2);
 
        sys_semPost(phil->sem);
    }
}

void philosopher(int argc, char ** argv);

void addPhilo() {
    philoCount++;

    philosopher_t * new = sys_malloc(sizeof(philosopher_t *));
    new->argv = sys_malloc(sizeof(char *) * ARGV_SIZE);
    new->buffer = sys_malloc(sizeof(char ) * BUFF_SIZE);
    new->state = THINKING;
    new->sem = sys_semOpen("filosofo", 0);

    new->argv[0] = "filosofo";
    new->argv[1] = itoa((uint64_t) new, new->argv[1], 10);

    new->left = firstPhil->left;
    new->right = firstPhil;

    firstPhil->left->right = new;
    firstPhil->left = new;

    new->pid = sys_loadProcess(philosopher, 1, 2, new->argv, NULL);
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
 
    // sys_sleep(1);
}
 
void put_fork(philosopher_t * phil) {
 
    sys_semWait(mutex);
 
    phil->state = THINKING;
 
    test(phil->left);
    test(phil->right);
 
    sys_semPost(mutex);
}
 
void philosopher(int argc, char ** argv)
{
    if (argc != 2) {
        sys_exit();
    }
    
    philosopher_t * i = (philosopher_t *) ((uint64_t) atoi(argv[1], -1));

    while (1) {
        // sys_sleep(1);
 
        take_fork(i);
        // printState();
 
        // sys_sleep(1);
 
        put_fork(i);
        // printState();
    }
    sys_exit();
}

void phylo(int argc, char ** argv) {
    if (argc == 1)
        philoCount = STARTING;
    else if (argc == 2) {
        philoCount = atoi(argv[1], -1);
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
        phil->debug = i;
        phil->argv = sys_malloc(sizeof(char *) * 2);
        phil->buffer = sys_malloc(20);
        phil->state = THINKING;
        phil->sem = sys_semOpen("filosofo", 0);

        phil->argv[0] = "filosofo";
        // phil->argv[1] = itoa((uint64_t) phil, phil->argv[1], 10);
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
        phil->pid = sys_loadProcess(philosopher, 1, 2, phil->argv, NULL);
        phil = phil->right;
    } while (phil != firstPhil);
    
    // char c;
    // while (1)  {
    //     while ((c = getChar()) != 0 && c != -1) {
    //         if (c == 'a') {
    //             addPhilo();
    //         }
    //         else if (c == 'r') {
    //             removePhilo();
    //         }
    //         else if (c == 'q') {
    //             break;
    //         }
    //     }
    // }

    // phil = firstPhil;
    // do {
    //     sys_semClose(phil);
    //     sys_free(phil);
    //     sys_free(phil);
    //     sys_free(phil);

    //     sys_kill(phil->pid);
    //     phil = phil->right;
    // } while (phil != firstPhil); // PROBLEMA

    sys_exit();
}


// La primera es muy similar a:

/*
 * Taken from "Operating Systems - Design and Implementation" by Tanenbaum
 * Section 2.3, page 91, Figure 2-10
 * https://gist.github.com/codelance/4186161
*/