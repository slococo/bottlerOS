#include "keyboard.h"

static unsigned char kbdus[250];
static unsigned char kbdus_sh[250];
// static unsigned char kbdsp_sh[250];
// static unsigned char kbdsp[250];

#define SIZE 30
unsigned char buffer[SIZE] = {0};
unsigned char * current = buffer;
unsigned char * last = buffer;

void saveChar(unsigned char c) {
    *last++ = c;
    if (last - buffer == SIZE)
        last = buffer;
}

char flagChangeAlt = 1;
char flagChangeF1 = 1;
unsigned char flag = 1;

void testKeyboardInterrupt(unsigned char c) {
    if (c == 0x2A || c == 0x36) {
        flag = 0;
        return;
    }
    else if (c == 0xAA || c == 0xB6) {
        flag = 1;
        return;
    }
  
    else if (c == 0x3A) {
        flag = ~(flag | 0xFE);
    }

    else if (c == 0x38) { //ALT
        flagChangeAlt = 0;
    }
    else if (c == 0x3B) { //F1
		  flagChangeF1 = 0;
    }
    
    if (flagChangeAlt == 0 && flagChangeF1 == 0) {
        flagChangeAlt = 1;
        flagChangeF1 = 1;
        saveChar('\v');
        return;
    }
    
    else if (c == 0xB8) {
        flagChangeAlt = 1;
        return;
    }
    else if (c == 0xBB) {
        flagChangeF1 = 1;
        return;
    }

    if (flag == 0) {
        c = kbdus_sh[c];
        if (c != 0)
        saveChar(c);
    }
    else {
        c = kbdus[c];
        if (c != 0)
        saveChar(c);
    }
}

unsigned char getKeyFromBuffer() {
	if (current - buffer == SIZE) {
		current = buffer;
    }

	char aux = *current;
    *current++ = 0;
    return aux;
}

void keyboard_handler() {
    unsigned char c = getCharInterrupt();
    testKeyboardInterrupt(c);
}

// static unsigned char kbdsp[250] = {
//     0,  124, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
//     '9', '0', '\'', 168, '\b',	/* Backspace */
//     '\t',			/* Tab */
//     'q', 'w', 'e', 'r',	/* 19 */
//     't', 'y', 'u', 'i', 'o', 'p', 96, '+', '\n',	/* Enter key */
//     0,			/* 29   - Control */
//     'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 164,	/* ñ */
//     '{', '}',   0,		/* Left shift */
//     '<', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
//     'm', ',', '.', '-',   0,				/* Right shift */
//     '*',
//     0,	/* Alt */
//     ' ',	/* Space bar */
//     0,	/* Caps lock */
//     0,	/* 59 - F1 key ... > */
//     0,   0,   0,   0,   0,   0,   0,   0,
//     0,	/* < ... F10 */
//     0,	/* 69 - Num lock*/
//     0,	/* Scroll Lock */
//     0,	/* Home key */
//     0,	/* Up Arrow */
//     0,	/* Page Up */
//   '-',
//     0,	/* Left Arrow */
//     0,
//     0,	/* Right Arrow */
//   '+',
//     0,	/* 79 - End key*/
//     0,	/* Down Arrow */
//     0,	/* Page Down */
//     0,	/* Insert Key */
//     0,	/* Delete Key */
//     0,   0,   0,
//     0,	/* F11 Key */
//     0,	/* F12 Key */
//     0,	/* All other keys are undefined */
// };

static unsigned char kbdus[250] = {
  0,  27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
  '9', '0', '-', '=', '\b',    /* Backspace */
  '\t',            /* Tab */
  'q', 'w', 'e', 'r',    /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    /* Enter key */
    0,            /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',    /* 39 */
 '\'', '`',   0,        /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
  'm', ',', '.', '/',   0,                /* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

// static unsigned char kbdsp_sh[250] =
// {
//     0,  167, 21, '\"', '#', '$', '%', '&', '/', '(',	/* 9 */
//   ')', '=', '?', 173, '\b',	/* Backspace */
//   '\t',			/* Tab */
//   'Q', 'W', 'E', 'R',   /* 19 */
//   'T', 'Y', 'U', 'I', 'O', 'P', 0, '*', '\n', /* Enter key */
//     0,          /* 29   - Control */
//   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 165, /* 39 */
//   '[', ']',   0,        /* Left shift */
//   '>', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 49 */
//   'M', ';', ':', '_',   0,              /* Right shift */
//   '*',
//     0,	/* Alt */
//   ' ',	/* Space bar */
//     0,	/* Caps lock */
//     0,	/* 59 - F1 key ... > */
//     0,   0,   0,   0,   0,   0,   0,   0,
//     0,	/* < ... F10 */
//     0,	/* 69 - Num lock*/
//     0,	/* Scroll Lock */
//     0,	/* Home key */
//     0,	/* Up Arrow */
//     0,	/* Page Up */
//   '-',
//     0,	/* Left Arrow */
//     0,
//     0,	/* Right Arrow */
//   '+',
//     0,	/* 79 - End key*/
//     0,	/* Down Arrow */
//     0,	/* Page Down */
//     0,	/* Insert Key */
//     0,	/* Delete Key */
//     0,   0,   '>',
//     0,	/* F11 Key */
//     0,	/* F12 Key */
//     0,	/* All other keys are undefined */
// };

static unsigned char kbdus_sh[250] =
{
   0,  27, '!', '\"', '#', 0 /* shift+4 */, '%', '&', '/', '(',	/* 9 */
  ')', '=', '?', '`', '\b',	/* Backspace */
  '\t',			/* Tab */
 'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,          /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
 '\'', '>',   0,        /* Left shift */
 '*', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 49 */
  'M', ';', ':', '_',   0,              /* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   '>',
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};