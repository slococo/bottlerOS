// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>

extern char bss;
extern char endOfBinary;

int main(int argc, char *argv[]);

void *memset(void *destiny, int32_t c, uint64_t length);

int _start(int argc, char *argv[]) {
    memset(&bss, 0, &endOfBinary - &bss);

    return main(argc, argv);
}

void *memset(void *destination, int32_t c, uint64_t length) {
    uint8_t chr = (uint8_t) c;
    char *dst = (char *) destination;

    while (length--)
        dst[length] = chr;

    return destination;
}
