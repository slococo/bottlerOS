#include <stdint.h>
#include "video.h"
#include "keyboard.h"
#include "time.h"
#include "pcb.h"
#include "pipeLib.h"
#include "schedulerLib.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define STDOUT_COLOR 0x0f
#define STDERR_COLOR 0x04 

uint64_t write(uint64_t fd, uint64_t buffer, uint64_t length) {
    char * bufferAux = (char *) buffer;
    int color = STDOUT_COLOR;

    fd = getFdOut();

    if (fd != STDOUT) {
        int i = 0;
	    while (bufferAux[i] != '\0' && i <= length) {
            writePipe(fd, bufferAux[i++]);
        }
        return i;
    }

    return printStringLen(color, bufferAux, (int) length);
}

uint64_t read(uint64_t fd, uint64_t buffer, uint64_t length) {
    char * bufferAux = (char *) buffer;
    int readBytes = 0;

    if (!isForeground())
        return 0;

    fd = getFdIn();

    if (fd == STDIN) {
        while (length-- > 0) {
            *bufferAux = getKeyFromBuffer();
            if (*bufferAux == 0) {
                bufferAux--;
                length++;
                readBytes--;
                blockIO();
            }
            if (*bufferAux == '\v') {
                return -1;
                // break;
            }
            readBytes++;
            bufferAux++;
            // blockIO();
        }
    }
    else {
        while (length-- > 0) {
            *bufferAux = readPipe(fd);
            if (*bufferAux == 0)
                break;
            bufferAux++;
            readBytes++;
        }
    }

	return readBytes;
}

// void createProcess(void (*fn)) {
//     newProcess(fn);
// }