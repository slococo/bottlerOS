// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_sync.h"

#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID "sem"
#define ITERS "2000"

static int global;

void slowInc(int *p, int inc){
  int aux = *p;
  aux += inc;
  sys_quitCPU();
  *p = aux;
}

void inc(int argc, char ** argv) {
  if (argc != 4) {
    printStringLen("inc recibe 4 argumentos\n", 25);
    sys_exit();
    return;
  }

  uint64_t semDir = (uint64_t) atoi(argv[1], 14);
  int64_t value = atoi(argv[2], 10);
  uint64_t N = atoi(argv[3], 10);
  uint64_t i;

  for (i = 0; i < N; i++){
    if (semDir) sys_semWait((sem_t *) semDir);
    slowInc(&global, value);
    if (semDir) sys_semPost((sem_t *) semDir);
  }

  char buffer[10] = {0};
  printStringLen("Final value: ", 14);
  printStringLen(itoa(global, buffer, 10), 10);
  newline();
  sys_exit();
}

void test_sync(int argc, char ** argv){
  uint64_t i;

  global = 0;

  printStringLen("CREATING PROCESSES...(WITH SEM)\n", 33);

  sem_t * sem = sys_semOpen(SEM_ID, 1);
  char semDir[10] = {0};
  itoa((uint64_t) sem, semDir, 10);

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    char * argv1[] = {"inc", semDir, "1", ITERS};
    char * argv2[] = {"inc", semDir, "-1", ITERS};
    sys_loadProcess(inc, 0, 4, argv1, NULL);
    sys_loadProcess(inc, 0, 4, argv2, NULL);
  }

  sys_wait();
  sys_semClose(sem);

  sys_exit();
}

void test_no_sync(int argc, char ** argv){
  uint64_t i;

  global = 0;

  printStringLen("CREATING PROCESSES...(WITHOUT SEM)\n", 36);

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    char * argv1[] = {"inc", "0", "1", ITERS};
    char * argv2[] = {"inc", "0", "-1", ITERS};
    sys_loadProcess(inc, 0, 4, argv1, NULL);
    sys_loadProcess(inc, 0, 4, argv2, NULL);
  }

  sys_exit();
}
