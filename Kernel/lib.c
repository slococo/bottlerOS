// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "lib.h"

void *memset(void *destination, int32_t c, uint64_t length) {
    uint8_t chr = (uint8_t) c;
    char *dst = (char *) destination;

    while (length--)
        dst[length] = chr;

    return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length) {
    uint64_t i;

    if ((uint64_t) destination % sizeof(uint32_t) == 0 &&
        (uint64_t) source % sizeof(uint32_t) == 0 &&
        length % sizeof(uint32_t) == 0) {
        uint32_t *d = (uint32_t *) destination;
        const uint32_t *s = (const uint32_t *) source;

        for (i = 0; i < length / sizeof(uint32_t); i++)
            d[i] = s[i];
    } else {
        uint8_t *d = (uint8_t *) destination;
        const uint8_t *s = (const uint8_t *) source;

        for (i = 0; i < length; i++)
            d[i] = s[i];
    }

    return destination;
}

void swap(char *x, char *y) {
    char t = *x;
    *x = *y;
    *y = t;
}

char *reverse(char *buffer, int i, int j) {
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

int abs(int value) {
    return value < 0 ? -value : value;
}

char addSpaces(char *str, char qty) {
    char aux = qty;
    while (qty-- > 0) {
        *str++ = ' ';
    }
    return aux;
}

char *itoa(int value, char *buffer, int base, int length) {
    if (base < 2 || base > 32) {
        return buffer;
    }

    int n = abs(value);
    int i = 0;
    while (n && i < length) {
        int r = n % base;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        } else {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    if (i == 0) {
        buffer[i++] = '0';
    }

    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';
    return reverse(buffer, 0, i - 1);
}

int strcpy(char *strDest, const char *strSrc) {
    int i = 0;
    while ((*strDest++ = *strSrc++)) {
        i++;
    }
    return i;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}