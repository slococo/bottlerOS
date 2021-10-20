#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include "video.h"
#include "keyboard.h"
#include "time.h"
#include "pcb.h"
#include "memManager.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const memoryModuleAddress = (void*)0x600000;

typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

void load_idt();
uint64_t getRSP();
void printBottlerAndWait();

#include "test_util.h"

#ifdef BUDDY
#define MAX_BLOCKS 128
#define MAX_MEMORY 800
#else
#define MAX_BLOCKS 42//1024/24 //128
#define MAX_MEMORY 352//1024-42x16 //Should be around 80% of memory managed by the MM
#endif

typedef struct MM_rq{
  void *address;
  uint32_t size;
}mm_rq;

void test_mm(){
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

	int i = 1000;
  while (i-- != 0){
//   while (1){
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while(rq < MAX_BLOCKS && total < MAX_MEMORY){
      mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;
      mm_rqs[rq].address = pvPortMalloc(mm_rqs[rq].size);
    //   mm_rqs[rq].address = memMalloc(mm_rqs[rq].size);
	  if (mm_rqs[rq].address == NULL) {
		  for (int rqAux = 0; rqAux < rq; rqAux++) {
		ncPrintDec(mm_rqs[rqAux].size );
		ncPrint(" - ");
		  }
    	// printStringLen(13, "malloc() -- ERROR", 20); 
		// new_line();
		ncPrint("Malloc -- null");
			ncNewline();
	  }
      total += mm_rqs[rq].size;
      rq++;
    }
	
	// 	ncPrint("libre dps de malloc: ");
	// ncPrintDec(xPortGetFreeHeapSize());
	// 		ncNewline();
	// 	ncPrint("cumpli maximo");
	// 		ncNewline();

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
    		// printStringLen(13, "memCheck() -- ERROR", 20); 
			// new_line();
		ncPrint("memCheck -- null");
			ncNewline();
		}

    // Free
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address != NULL)
        vPortFree(mm_rqs[i].address);
	}
        // memFree(mm_rqs[i].address);
	
	// 	ncPrint("libre dps de free: ");
	// ncPrintDec(xPortGetFreeHeapSize());
	// 		ncNewline();
		ncPrint("termine un loop regio");
			ncNewline();

	// if (i == 5)
	// wait(3);
	// ncClear();

    // printStringLen(13, "ALL GOOD", 9); 
	// new_line();
  } 
}
int main() {
	load_idt();

	// if (initMemoryManager(memoryModuleAddress, memoryModuleAddress + sizeof(void *)) == -1) {
    // 	printStringLen(13, "createMemoryManager() -- ERROR", 31); 
	// 	new_line();
	// 	return EXIT_FAILURE;
	// }

	initMemoryManager(memoryModuleAddress);

	#ifndef BUDDY
	// SACAR DESPUÉS! ES SOLO PARA TESTEO... CORRER DE A UNO!
	// if (testOne() == EXIT_FAILURE)
	// 	return EXIT_FAILURE;
	// if (testTwo() == EXIT_FAILURE)
		// return EXIT_FAILURE;
	#endif
		// test_mm();
	
	saveSampleRSP(getRSP());

	printBottlerAndWait();

	((EntryPoint)sampleCodeModuleAddress)();
	return EXIT_SUCCESS;
}

void printBottlerAndWait() {
	printStringLen(4, "                                                                             ", 80); new_line();
    printStringLen(4, "                                   (%(                                     ", 80); new_line();
	printStringLen(15, "      Welcome to", 17);
    printStringLen(4, "                  %%%%%                                      ", 80); new_line();
	printStringLen(15, "        BottlerOS", 18);
    printStringLen(4, "                  %%%                                      ", 80); new_line();
    printStringLen(12, "                              %%%%%%%%%%%%%                                 ", 80); new_line();
    printStringLen(12, "                          %%%%%%%%%%%%%%%%%%%%%                             ", 80); new_line();
    printStringLen(12, "                        %%%%%%%           %%%%%%%                           ", 80); new_line();
    printStringLen(12, "                      %%%%%                   %%%%%                         ", 80); new_line();
    printStringLen(14, "                     %%%%%                     %%%%%                        ", 80); new_line();
    printStringLen(14, "                    %%%%% ", 27); 
	printStringLen(14, " %%%%           %%%% ", 22); 
	printStringLen(14, " %%%%%                       ", 30); new_line();
    printStringLen(14, "                    %%%%%  ", 28); 
	printStringLen(14, " (%             %(  ", 21); 
	printStringLen(14, " %%%%%                       ", 30); new_line();
    printStringLen(14, "                    %%%%%%%                   %%%%%%%                       ", 80); new_line();
    printStringLen(2, "                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                       ", 80); new_line();
    printStringLen(2, "                 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                    ", 80); new_line();
    printStringLen(2, "                %%%%%%%%%%%%%%%%%%%%%%% ", 41); 
	printStringLen(12, " %  %* ", 8); 
	printStringLen(2, " %%%%%%%%%                   ", 30); new_line();
    printStringLen(2, "                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                   ", 80); new_line();
    printStringLen(9, "                %%**%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/%%                   ", 80); new_line();
    printStringLen(9, "                %%* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% /%%                   ", 80); new_line();
    printStringLen(9, "                %%* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% /%%                   ", 80); new_line();
    printStringLen(9, "                %%%  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%%                   ", 80); new_line();
    printStringLen(13, "              ,%%%%%   %%%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%.                 ", 80); new_line();
    printStringLen(13, "                %%.         %%%%%%%%%%%%%%%%%         .%%                   ", 80); new_line();
    printStringLen(13, "                              %%%%%%%%%%%%%                                 ", 80); new_line();
    printStringLen(13, "                                 %%%%%%%                                    ", 80); new_line();
    printStringLen(13, "                                                                            ", 80); new_line();
	
	wait(3);
	
	clear();
}