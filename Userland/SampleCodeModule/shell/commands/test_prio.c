// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_prio.h"

#define MINOR_WAIT 10000000                               // TODO: To prevent a process from flooding the screen
#define WAIT      1000000000                              // TODO: Long enough to see theese processes beeing run at least twice

void bussy_wait(uint64_t n){
  uint64_t i;
  for (i = 0; i < n; i++);
}

void endless_loop2(){
  uint64_t pid = sys_getPid();

  while(1){
    char buffer[10];
    printStringLen(itoa(pid, buffer, 10), 10);
    // sys_sleep(1);
    bussy_wait(MINOR_WAIT);
  }

  sys_exit();
}

#define TOTAL_PROCESSES 3

void test_prio(int argc, char ** argv){
  uint64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  for(i = 0; i < TOTAL_PROCESSES; i++) {
    char *argv[] = {"test_prio"};
    pids[i] = sys_loadProcess(endless_loop2, 0, 1, argv, NULL);
  }

  bussy_wait(WAIT);
  // sys_sleep(5);
  printStringLen("\nCHANGING PRIORITIES...\n", 25);

  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        sys_nice(pids[i], 19); //lowest priority 
        break;
      case 1:
        sys_nice(pids[i], 0); //medium priority
        break;
      case 2:
        sys_nice(pids[i], -20); //highest priority
        break;
    }
  }

  bussy_wait(WAIT);
// sys_sleep(5);
  printStringLen("\nBLOCKING...\n", 14);

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_block(pids[i]);

  printStringLen("CHANGING PRIORITIES WHILE BLOCKED...\n", 38);
  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        sys_nice(pids[i], 0); //medium priority
        break;
      case 1:
        sys_nice(pids[i], 0); //medium priority
        break;
      case 2:
        sys_nice(pids[i], 0); //medium priority
        break;
    }
  }

  printStringLen("UNBLOCKING...\n", 15);

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_unblock(pids[i]);

  bussy_wait(WAIT);
// sys_sleep(5);
  printStringLen("\nKILLING...\n", 13);

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_kill(pids[i]);
    
  sys_exit();
}