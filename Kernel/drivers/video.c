#include <stdint.h>
#include "video.h"

static uint8_t * const video = (uint8_t*) 0xB8000;
static uint8_t * currentVideo = (uint8_t*) 0xB8000;
static const int width = 80;
static const int height = 25;
static int currentX = 0;
static int currentY = 0;

int limitX[2] = {0, 80};
int limitY[2] = {0, 25};

void increment() {
	currentX++;
	if (currentX >= limitX[1]) {
		currentY++;
		currentX = limitX[0];
		if (currentY >= limitY[1]) 
			currentY = limitY[0];
	}
}

int atoi(char * string, int length) {
    int res = 0, i = 0;
    while (i < length) {
        res = res * 10 + string[i++] - '0';
    }
    return res;
}

// "\e\f" para clear
char checkIfEscapeSequence(const char * bufferAux) {
	if (*bufferAux == '\e') {
		bufferAux++;
		if (*bufferAux == '\f') {
			bufferAux++;
			clear();
		}
		return 1;
	}
	return 0;
}

int printStringLen(int color, const char * string, int maxLen) {
	int i = 0;
	
	while (*string != '\0' && i <= maxLen) {
		if (currentX >= limitX[1]) {
			currentX = limitX[0];
			currentY++;
		}
		if (currentY >= limitY[1]) {
			currentY = limitY[0];
		}

		if (*string == '\n') {
			new_line();
			// return i;
			string++;
			i++;
			continue;
		}
		else if (*string == '\b') {
			backspace();
			return i;
		}
		else if (checkIfEscapeSequence(string)) {
			return i;
		}

        *(video + currentY * width * 2 + currentX * 2) = *string++;
        *(video + currentY * width * 2 + currentX * 2 + 1) = color;
		increment();
		i++;
    }

	return i;
}

void backspace() {
	if (video + currentY * width * 2 + currentX * 2 > video) {
		currentX--;
		if (currentX < limitX[0]) {
			currentX = limitX[1];
			if (currentY > limitY[0])
				currentY--;
		}
	}
	*(video + currentY * width * 2 + currentX * 2) = ' ';
}

void new_line() {
	currentX = limitX[0];
	currentY++;
	if (currentY == limitY[1])
		currentY = limitY[0];
}

void clear() {
	for (int i = limitX[0]; i < (limitX[1] - limitX[0]) * 2 * (limitY[1] - limitY[0]); i++) {	
		printStringLen(15, " ", 1);
	}

	currentX = limitX[0];
	currentY = limitY[0];
}