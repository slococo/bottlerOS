#include "libc.h"
#include "quadratic.h"
#include "shell.h"

void quadratic(double a, double b, double c) {
    double sol1, sol2;
    if (a == 0) {
        printStringLen("First argument cannot be 0\n", 28);
        return;
    }

    int cond = quadSolver(a, b, c, &sol1, &sol2);

	if (cond == 0) {
		printStringLen("Not real!\n", 11);
        return;
    }
	
	char buffer[30] = {0};
	printStringLen("Roots for ", 11);
    ftoa(a, buffer, 10);
    printString(buffer);
    printStringLen("x^2 + ", 7);
    ftoa(b, buffer, 10);
    printString(buffer);
    printString("x + ");
    ftoa(c, buffer, 10);
    printString(buffer);
    new_line();
	ftoa(sol1, buffer, 10);
	printString(buffer);
	printStringLen(" and ", 6);
    char buffer2[30] = {0};
	ftoa(sol2, buffer2, 10);
	printString(buffer2);
    new_line();
}