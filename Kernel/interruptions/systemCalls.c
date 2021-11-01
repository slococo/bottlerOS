#include "systemCalls.h"

uint64_t write(uint64_t fd, uint64_t buffer, uint64_t length) {
    char *bufferAux = (char *) buffer;
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
    char *bufferAux = (char *) buffer;
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
                return 0;
                // break;
            }
            readBytes++;
            bufferAux++;
            // blockIO();
        }
    } else {
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