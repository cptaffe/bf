
#include <stdio.h>
#include <stdlib.h> // for exit
#include <stdarg.h> // ... args
#include <string.h>

static char *format(char *fmt) {
	// append newline to fmt
	int len = strlen(fmt);

	if (len < 0) {
		return NULL; // error
	} else {
		// add one for newline
		char *msg = malloc(len + 1);

		if (msg == NULL) {
			return NULL; // error
		} else {

			// copy message
			msg = memcpy(msg, fmt, len);

			if (msg == NULL) {
				return NULL; // error
			} else {
				// append newline, null terminate
				msg[len] = '\n';
				msg[len + 1] = '\0';

				return msg;
			}
		}
	}
}

// takes a string message, prints it to stderr,
// then exits the process with a failure.
void bf_fail(char *fmt, ...)  {

	// handle failure message
	va_list args;
	va_start (args, fmt);

	char *msg;
	if ((msg = format(fmt)) == NULL) {
		fprintf(stderr, "fail: formatting of error message failed\n.");
		va_end(args);
	} else {
		// print error.
		vfprintf (stderr, msg, args);
		va_end (args);
		free(msg);
	}

	// exit unhappily.
	exit(EXIT_FAILURE);
}

// takes a string message, prints it to stderr,
void bf_err(char *fmt, ...) {

	// handle failure message
	va_list args;
	va_start (args, fmt);

	char *msg;
	if ((msg = format(fmt)) == NULL) {
		fprintf(stderr, "err: formatting of error message failed\n.");
		va_end(args);
	} else {
		// print error.
		vfprintf (stderr, msg, args);
		va_end (args);
		free(msg);
	}
}
