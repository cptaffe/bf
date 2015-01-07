
#include <stdio.h>
#include <stdlib.h> // for exit
#include <stdarg.h> // ... args
#include <string.h>

void bf_err(char *file, char *func, int line, char *fmt, ...) {
	va_list ap;
	char *str;
	va_start(ap, fmt);
	int asret = vasprintf(&str, fmt, ap);
	va_end(ap);

	if (!(asret < 0)) {
		fprintf(stderr, "%s:%d::%s: %s\n", file, line, func, str);
		free(str);
	}
}
