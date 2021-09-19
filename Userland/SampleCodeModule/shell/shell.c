#include "libc.h"
#include "help.h"
#include "clear.h"
#include "time.h"
#include "shell.h"
#include "inforeg.h"
#include "printmem.h"
#include "excDiv.h"
#include "excOP.h"
#include "quadratic.h"
#include "cpu_id.h"
#include "change.h"

#define SIZE 100
#define MAX_ARGS 5

#define COLS 40
#define ROWS 25

const int len = 8;
char *commands_void[] = {"help", "time", "inforeg", "excdiv", "excop", "clear", "cpufeatures", "change"};
void (*func []) (char *, int *) = {help, time, inforeg, excdiv, excop, clear, cpufeatures, change};

void substractLine(char * window, int * offset) {
    for (int i = 0; i < ROWS - 1; i++) {
        for (int j = 0; j < COLS; j++) {
            window[i * COLS + j] = window[(i + 1) * COLS + j];
        }
    }
    for (int i = 0; i < COLS; i++) {
        window[(ROWS - 1) * COLS + i - 1] = ' ';
        window[ROWS * COLS] = 0;
    }
    *offset = (ROWS - 1) * COLS;
}

void addText(char * buffer, char * window, int * offset) {
    while (*buffer != 0) {
        if (*offset == ROWS * COLS - 1) substractLine(window, offset);
        window[(*offset)++] = *buffer++;
    }
}

void printWindow(char * window) {
    printString(window);
}

void scanfNoPrint(char * buffer, int maxSize, char * window, int * offset) {
    char c;
    int i = 0;
    while ((c = getChar()) != '\n' && i < maxSize - 1) {
        if (c != -1) {
            if (c == '\v')
                sys_switchContext();
            else if (c == '\b' && i > 0) {
                buffer[--i] = ' ';
                window[--(*offset)] = ' ';
                printWindow(window);
            }
            else if (c != 0 && c != '\b') { 
                buffer[i++] = c;
                if (*offset == ROWS * COLS - 1) substractLine(window, offset);
                window[(*offset)++] = c;
                printWindow(window);
            }
        }
    }
    buffer[i] = '\0';
    if (*offset == ROWS * COLS - 1) substractLine(window, offset);
    window[*offset] = ' ';
}

void clearWindow(char * window, int * offset) {
    for (int i = 0; i <= ROWS * COLS; i++) {
        window[i] = ' ';
    }
    window[ROWS * COLS] = 0;
    *offset = (ROWS - 1) * COLS;
    printWindow(window);
}

void shell() {
    char window[ROWS * COLS + 1] = {[0 ... ROWS * COLS - 1] = ' ', 0};
    int offset = (ROWS - 1) * COLS;

    printWindow(window);
    while (1) {
        int comm_flag = 0;
        addText("$> ", window, &offset);
        printWindow(window);
        char buffer[SIZE] = {0};
        scanfNoPrint(buffer, SIZE, window, &offset);
        substractLine(window, &offset);
        char* tokens[SIZE] = {0};
        tokens[0] = strstrip(buffer, ' ');
        for (int i = 1; i < MAX_ARGS; i++) {
            tokens[i] = strtok(tokens[i - 1], ' ');
        }
        for (int i = 0; i < len; i++) {
            if (!strcmp(tokens[0], commands_void[i])) {
                if (*tokens[1] != 0)
                    incorrect_arg(tokens[0], window, &offset);
                else
                    (*func[i])(window, &offset);
                comm_flag = 1;
            }
        }
        if (!strcmp(tokens[0], "quadratic")) {
            if (*tokens[4] != 0 || *tokens[3] == 0)
                incorrect_arg(tokens[0], window, &offset);
            else if (!isFloat(tokens[1]) || !isFloat(tokens[2]) || !isFloat(tokens[3]))
                incorrect_arg(tokens[0], window, &offset);
            else
                quadratic(window, &offset, atof(tokens[1]), atof(tokens[2]), atof(tokens[3]));
            comm_flag = 1;
        }
        if (!strcmp(tokens[0], "printmem")) {
            if (*tokens[2] != 0 || *tokens[1] == 0)
                incorrect_arg(tokens[0], window, &offset);
            else {
                int length = strlen(tokens[1]);
                printmem(window, &offset, atoi(tokens[1], length));
            }
            comm_flag = 1;
        }

        if (!comm_flag) {
            if (*tokens[0] != 0)
                incorrect_comm(tokens[0], window, &offset);
        }
    }
}

void incorrect_comm(char * buffer, char* window, int * offset) {
    addText(buffer, window, offset);
    addText(" is not a BottlerShell command", window, offset);
    printWindow(window);
    substractLine(window, offset);
}

void incorrect_arg(char * command, char* window, int * offset) {
    addText("Incorrect arguments for command ", window, offset);
    addText(command, window, offset);
    printWindow(window);
    substractLine(window, offset);
}