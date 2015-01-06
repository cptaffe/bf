
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
		char *msg = malloc(len + 2);

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
static void bf_err(char *fmt, ...) {

	// handle failure message
	va_list args;
	va_start (args, fmt);

	char *msg;
	if ((msg = format(fmt)) == NULL) {
		fprintf(stderr, "err: formatting of error message failed\n.");
		va_end(args);
	} else {
		// print error.
		vfprintf(stderr, msg, args);
		va_end (args);
		free(msg);
	}
}

void bf_err_stat(char *file, char *func, int line, char *fmt, ...) {
	va_list ap;
	char *str;
	va_start(ap, fmt);
	int asret = vasprintf(&str, fmt, ap);
	va_end(ap);

	if (!(asret < 0)) {
		#ifdef DEBUG
		bf_err("%s:%d::%s: %s", file, line, func, str);
		#else
		bf_err("%s: %s", func, str);
		#endif
		free(str);
	}
}
