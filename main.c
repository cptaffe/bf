
// simple brainfuck interpreter
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // errorno
#include <string.h> // strerror
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution

#include "lex.h"
#include "lex_funcs.h"
#include "bf.h"

void *lex_state_threadable(void *l) {
	lex_state((lex *) l);
	return NULL;
}

int main(int argc, char **argv) {

	FILE *file;

	// argument checking
	if (argc > 2) {
		// print usage instructions on improper usage
		fprintf(stderr, "usage: bf <file>\n");
	} else if (argc == 1) {
		file = stdin;
	} else if (argc == 2) {
		file = fopen(argv[1], "r");
		if (file == NULL) {
			fail("file open failed: %s.", strerror(errno));
		}
	}

	// init lexer
	lex *l = lex_init(10, stdin, lex_all);
	if (l == NULL) {
		fail("lex alloc failed: %s.", strerror(errno));
	}

	// init lex data
	l->data = lex_data_init();
	if (l->data == NULL) {
		fail("lex_data alloc failed: %s.", strerror(errno));
	}

	// lexer state machine thread
	pthread_t lexer_thread;
	int pcs = pthread_create(&lexer_thread, NULL, lex_state_threadable, (void *) l);
	if (pcs != 0) {
		fail("creating lex_state_threadable thread failed: %s.", strerror(errno));
	}

	// wait for threads
	int pjs = pthread_join(lexer_thread, NULL);
	if (pjs != 0) {
		fail("joining lex_state_threadable thread failed: %s.", strerror(errno));
	}

	// free lex
	lex_free(l);
}
