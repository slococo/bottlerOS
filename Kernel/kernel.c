#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include "video.h"
#include "keyboard.h"
#include "time.h"
#include "pcb.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

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

int main() {
	load_idt();

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

	saveSampleRSP(getRSP());

	((EntryPoint)sampleCodeModuleAddress)();
	return 0;
}