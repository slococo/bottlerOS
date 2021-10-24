#include "libc.h"

void printString(char * string) {
    int len = strlen(string);
    sys_write(1, string, len);
}

void printStringLen(char * string, int len) {
    sys_write(1, string, len);
}

void printStringError(char * string) {
    int len = strlen(string);
    sys_write(2, string, len);
}

int strlen(const char * s) {
    int i = 0;
    for (i = 0; s[i] != '\0'; i++);
    return i;
}

int strcmp(const char * s1, const char * s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

void putChar(char c){
    char buffer = c;
    sys_write(1, &buffer, 1);
}

void new_line() {
    putChar('\n');
}

void winClear() {
    sys_write(1, "\e\f", 2);
}

char* gtoa(int value, char* buffer, int base, int length) {
   if (base < 2 || base > 32) {
        return buffer;
    }
 
    int n = abs(value);
 
    int i = 0;
    while (n && i < length) {
        int r = n % base;
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
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

char getChar() {
    char buffer[1] = {0};
    if (sys_read(0, buffer, 1) <= 0)
        return -1;
    return buffer[0];
}

void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
char* reverse(char *buffer, int i, int j) {
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}

int abs(int value) {
    return value < 0 ? -value : value;
}
 
char* itoa(int value, char* buffer, int base) {
   if (base < 2 || base > 32) {
        return buffer;
    }
 
    int n = abs(value);
 
    int i = 0;
    while (n) {
        int r = n % base;
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
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

int atoi(char * string, int length) {
    int res = 0, i = 0;
    while (i < length) {
        res = res * 10 + string[i++] - '0';
    }
    return res;
}

int pow(int base, int exponent) {
	int result = 1;
	for (; exponent > 0; exponent--) {
		result = result * base;
	}
	return result;
}

char isFloat(char * str) {
    int dot = 0;
    if (*str == '-')
        str++;
    while (*str != 0) {
        if (*str == '.' && !dot)
            dot = 1;
        else if (!isNumber(*str) || (*str == '.' && dot)) {
            return 0;
        }
        str++;
    }
    return 1;
}

float atof(char *arr) {
    float val = 0;
    int afterdot=0;
    float scale=1;
    int neg = 0; 

    if (*arr == '-') {
        arr++;
        neg = 1;
    }
    while (*arr) {
        if (afterdot) {
            scale = scale/10;
            val = val + (*arr-'0')*scale;
        } else {
            if (*arr == '.') 
                afterdot++;
            else
                val = val * 10.0 + (*arr - '0');
        }
        arr++;
    }
    if(neg) return -val;
    else return  val;
}

char * strstrip(char * s, char c) {
    while (*s != 0) {
        if (*s != c)
            break;
        s++;
    }
    return s;
}

char *strtok(char * s, char delim) {
    char *ptr = s;

    if (s == 0) {
        return 0;
    }
    
    int flag = 0;
    while (*ptr != 0) {
        if (*ptr == delim) {
            flag = 1;
            *ptr = 0;
        }
        else if (flag == 1)
            return ptr;
        ptr++;
    }
    return ptr;
}

char isNumber(char c) {
    return c <= '9' && c >= '0';
}

// Tomada y modificada de https://github.com/antongus/stm32tpl/blob/master/ftoa.c

#define MAX_PRECISION	(10)
static const double rounders[MAX_PRECISION + 1] =
{
	0.5,				// 0
	0.05,				// 1
	0.005,				// 2
	0.0005,				// 3
	0.00005,			// 4
	0.000005,			// 5
	0.0000005,			// 6
	0.00000005,			// 7
	0.000000005,		// 8
	0.0000000005,		// 9
	0.00000000005		// 10
};

void ftoa(double f, char * buf, int precision) {
	char * ptr = buf;
	char * p = ptr;
	char * p1;
	char c;
	long intPart;

	if (precision > MAX_PRECISION)
		precision = MAX_PRECISION;

	if (f < 0) {
		f = -f;
		*ptr++ = '-';
	}

	if (precision < 0) {
		if (f < 1.0) precision = 6;
		else if (f < 10.0) precision = 5;
		else if (f < 100.0) precision = 4;
		else if (f < 1000.0) precision = 3;
		else if (f < 10000.0) precision = 2;
		else if (f < 100000.0) precision = 1;
		else precision = 0;
	}

	if (precision)
		f += rounders[precision];

	intPart = f;
	f -= intPart;

	if (!intPart)
		*ptr++ = '0';
	else {
		p = ptr;

		while (intPart) {
			*p++ = '0' + intPart % 10;
			intPart /= 10;
		}

		p1 = p;

		while (p > ptr) {
			c = *--p;
			*p = *ptr;
			*ptr++ = c;
		}

		ptr = p1;
	}

	if (precision && ((int) (f * pow(10, precision))) != 0) {
		*ptr++ = '.';

		while (precision-- && ((int) (f * pow(10, precision))) != 0) {
			f *= 10.0;
			c = f;
			*ptr++ = '0' + c;
			f -= c;
		}
	}

	*ptr = 0;
}