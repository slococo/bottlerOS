#include "libc.h"
#include "quadratic.h"
#include "shell.h"

void quadratic(char * window, int * offset, double a, double b, double c) {
    double sol1, sol2;
    if (a == 0) {
        addText("First argument cannot be 0", window, offset);
        printWindow(window);
        substractLine(window, offset);
        return;
    }

    int cond = quadSolver(a, b, c, &sol1, &sol2);

	if (cond == 0) {
		addText("Not real!", window, offset);
        printWindow(window);
        substractLine(window, offset);
        return;
    }
	
	char buffer[30] = {0};
	addText("Roots for ", window, offset);
    ftoa(a, buffer, 10);
    addText(buffer, window, offset);
    addText("x^2 + ", window, offset);
    ftoa(b, buffer, 10);
    addText(buffer, window, offset);
    addText("x + ", window, offset);
    ftoa(c, buffer, 10);
    addText(buffer, window, offset);
    substractLine(window, offset);
	ftoa(sol1, buffer, 10);
	addText(buffer, window, offset);
	addText(" and ", window, offset);
    char buffer2[30] = {0};
	ftoa(sol2, buffer2, 10);
	addText(buffer2, window, offset);
    printWindow(window);
    substractLine(window, offset);
}