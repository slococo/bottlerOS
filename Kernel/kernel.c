#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include "video.h"
#include "keyboard.h"
#include "time.h"
#include "memManager.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *) 0x400000;
static void *const sampleDataModuleAddress = (void *) 0x500000;
static void *const memoryModuleAddress = (void *) 0x600000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
    return (void *) ((uint64_t) &endOfKernel + PageSize * 8 - sizeof(uint64_t));
}

void *initializeKernelBinary() {
    void *moduleAddresses[] = {
            sampleCodeModuleAddress,
            sampleDataModuleAddress
    };

    loadModules(&endOfKernelBinary, moduleAddresses);
    clearBSS(&bss, &endOfKernel - &bss);

    return getStackBase();
}

void load_idt();
uint64_t getRSP();
void initScheduler();
void _cli();
void _sti();
void haltcpu();
void forceTimer();

int main() {
    load_idt();

    initMemoryManager(memoryModuleAddress);
    initScheduler();

    char *argv[] = {"SampleCode"};
    enqueueProcess(sampleCodeModuleAddress, 1, 1, argv, NULL);
    clear();
    _sti();
    forceTimer();

    return EXIT_SUCCESS;
}