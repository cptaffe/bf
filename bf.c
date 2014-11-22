
#include <stdio.h>
#include <stdlib.h> // for exit
#include <stdarg.h> // ... args

// takes a string message, prints it to stderr,
// then exits with a failure.
void fail(char *fmt, ...) {

	// handle failure message
	va_list args;
	va_start (args, fmt);
	vfprintf (stderr, fmt, args);
	va_end (args);

	// exit unhappily.
	exit(EXIT_FAILURE);
}

// takes a string message, prints it to stderr,
void err(char *fmt, ...) {

	// handle failure message
	va_list args;
	va_start (args, fmt);
	vfprintf (stderr, fmt, args);
	va_end (args);
}
