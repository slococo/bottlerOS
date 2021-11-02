#include "test_processes.h"
#include "ps.h"

#define MAX_PROCESSES 10 //Should be around 80% of the the processes handled by the kernel

enum State {ERROR, RUNNING, BLOCKED, KILLED};

typedef struct P_rq{
  uint32_t pid;
  enum State state;
}p_rq;


void endless_loop(){
  while(1);
  sys_exit();
}

void debugPSA() {
  return;
}

void test_processes(int argc, char ** argv){
  p_rq p_rqs[MAX_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;

  int i = 0;
  while (1) {
    // Create MAX_PROCESSES processes
    for(rq = 0; rq < MAX_PROCESSES; rq++){
        char *argv[] = {"endless"};
        p_rqs[rq].pid = sys_loadProcess(endless_loop, 0, 1, argv, NULL);  // TODO: Port this call as required
        sys_nice(p_rqs[rq].pid, 19);
        i++;

      if (p_rqs[rq].pid == -1){                           // TODO: Port this as required
        printStringLen("Error creating process\n", 24);               // TODO: Port this as required
        return;
      }else{
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0){

      for(rq = 0; rq < MAX_PROCESSES; rq++){
        action = GetUniform(2) % 2; 

        switch(action){
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED){
              sys_kill(p_rqs[rq].pid);
              p_rqs[rq].state = KILLED; 
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING){
              if(sys_block(p_rqs[rq].pid) == -1){          // TODO: Port this as required
                printStringLen("Error blocking process\n", 24);       // TODO: Port this as required
                return;
              }
              p_rqs[rq].state = BLOCKED; 
            }
            break;
        }
      }

      // Randomly unblocks processes
      for(rq = 0; rq < MAX_PROCESSES; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2){
          if(sys_unblock(p_rqs[rq].pid) == -1){            // TODO: Port this as required
            printStringLen("Error unblocking process\n", 26);         // TODO: Port this as required
            return;
          }
          p_rqs[rq].state = RUNNING; 
        }
    }
    if (i % 10000 == 0) {
      printStringLen("Procesos: ", 11);
      char buffer[4];
      printString(itoa(i, buffer, 10));
      newline();
    }
  }
  sys_exit();
}