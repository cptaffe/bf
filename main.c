
// simple brainfuck interpreter
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // errorno
#include <string.h> // strerror
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution

extern int errno;

#include "lex.h"
#include "lex_funcs.h"
#include "bf.h"

int main() {
	lex *l = lex_init(10, stdin, lex_all);
	if (l == NULL) {
		err("allocation of lex failed: %s.", strerror(errno));
	} else {

		// init lex data
		l->data = lex_data_init();

		if (l->data == NULL) {
			err("allocation of lex_data failed: %s.", strerror(errno));
		} else {

			// state machine loop.
			lex_state(l);
		}

		// free lex
		lex_free(l);
	}
}
