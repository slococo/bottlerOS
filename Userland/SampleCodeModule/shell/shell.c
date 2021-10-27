#include "libc.h"
#include "help.h"
#include "time.h"
#include "shell.h"
#include "clear.h"
#include "inforeg.h"
#include "printmem.h"
#include "excDiv.h"
#include "excOP.h"
#include "quadratic.h"
#include "cpu_id.h"
#include "ps.h"
#include "semCom.h"

#define SIZE 100
#define MAX_ARGS 5

#define COLS 80
#define ROWS 25

const int len = 9;
char *commands_void[] = {"help", "time", "inforeg", "excdiv", "excop", "clear", "cpufeatures", "ps", "sem"};
void (*func []) () = {help, time, inforeg, excdiv, excop, clear, cpufeatures, ps, sem};

void scanfNoPrint(char * buffer) {
    char c;
    int i = 0;
    while ((c = getChar()) != '\n' && i < SIZE - 1) {
        if (c != -1) {
            if (c == '\b' && i > 0) {
                buffer[--i] = ' ';
                backspace();
            }
            else if (c != 0 && c != '\b') { 
                buffer[i++] = c;
                putChar(c);
            }
        }
    }
    buffer[i] = '\0';
}

void shell(int argc, char *argv[]) {
    while (1) {
        int comm_flag = 0;
        printStringLen("$> ", 3);
        char buffer[SIZE] = {0};
        scanfNoPrint(buffer);
        new_line();
        char* tokens[SIZE] = {0};
        tokens[0] = strstrip(buffer, ' ');
        for (int i = 1; i < MAX_ARGS; i++) {
            tokens[i] = strtok(tokens[i - 1], ' ');
        }
        for (int i = 0; i < len; i++) {
            if (!strcmp(tokens[0], commands_void[i])) {
                if (*tokens[1] != 0)
                    incorrect_arg(tokens[0]);
                else
                    (*func[i])();
                comm_flag = 1;
            }
        }
        if (!strcmp(tokens[0], "quadratic")) {
            if (*tokens[4] != 0 || *tokens[3] == 0)
                incorrect_arg(tokens[0]);
            else if (!isFloat(tokens[1]) || !isFloat(tokens[2]) || !isFloat(tokens[3]))
                incorrect_arg(tokens[0]);
            else
                quadratic(atof(tokens[1]), atof(tokens[2]), atof(tokens[3]));
            comm_flag = 1;
        }
        if (!strcmp(tokens[0], "printmem")) {
            if (*tokens[2] != 0 || *tokens[1] == 0)
                incorrect_arg(tokens[0]);
            else {
                int length = strlen(tokens[1]);
                printmem(atoi(tokens[1], length));
            }
            comm_flag = 1;
        }

        if (!comm_flag) {
            if (*tokens[0] != 0)
                incorrect_comm(tokens[0]);
        }
    }
}

void incorrect_comm(char * buffer) {
    printString(buffer);
    printStringLen(" is not a BottlerShell command\n", 32);
}

void incorrect_arg(char * command) {
    printStringLen("Incorrect arguments for command ", 33);
    printString(command);
    new_line();
}