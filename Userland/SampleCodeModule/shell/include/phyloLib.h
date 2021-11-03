#ifndef PHYLOLIB_H
#define PHYLOLIB_H

#include <stddef.h>
#include <stdint.h>
#include "libc.h"
#include "system.h"
#include "phylo.h"
 
#define ARGV_SIZE 2
#define BUFF_SIZE 20
#define MAX_PHILO_SIZE 3
#define MAX_NAME_SIZE 10
#define STARTING 5

int * state;
typedef enum states {EATING = 0, HUNGRY, THINKING} states;

typedef struct philosopher_t {
    char ** argv;
    char * buffer;
    sem_t * sem;
    int pid;
    states state;

    struct philosopher_t * left;
    struct philosopher_t * right;
} philosopher_t;

void philosopher(int argc, char ** argv);
void freePhilo(philosopher_t * phil);
void end();

#endif