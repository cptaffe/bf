
// simple brainfuck interpreter
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // errorno
#include <string.h> // strerror
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution
#include <fcntl.h>

#include "lex.h"
#include "parse.h"
#include "jit.h"
#include "lex_funcs.h"
#include "bf.h"
#include "tok.h"

int main(int argc, char **argv) {
	FILE *file = stdin;

	// argument checking
	if (argc > 2) {
		// print usage instructions on improper usage
		fprintf(stderr, "usage: bf <file>\n");
	} else if (argc == 2) {
		file = fopen(argv[1], "r");
		if (file == NULL) {
			fail("file open failed: %s.", strerror(errno));
		}
	}

	// init lexer
	lex *l __attribute__((cleanup(lex_free_c))) = lex_init(10, file, bf_lex_all);
	if (l == NULL) {
		fail("lex alloc failed: %s.", strerror(errno));
	}

	// init lex data
	l->data = (void *) bf_lex_data_init();
	if (l->data == NULL) {
		fail("lex_data alloc failed: %s.", strerror(errno));
	}

	// init jit
	bf_jit *j __attribute__((cleanup(bf_jit_free_c))) = bf_jit_init(((bf_lex_data *) l->data)->st);
	if (j == NULL) {
		fail("j alloc failed: %s.", strerror(errno));
	}

	// lexer state machine thread, check lex_state_threadable create success
	const int threads_len = 2;
	pthread_t threads[threads_len];
	int lstcs = pthread_create(&threads[0], NULL, lex_state_threadable, (void *) l);
	if (lstcs) {
		fail("creating lex_state_threadable thread failed: %s.", strerror(lstcs));
	}

	// jit machine
	int jcs = pthread_create(&threads[1], NULL, bf_jit_threadable, (void *) j);
	if (jcs) {
		fail("creating bf_jit_threadable thread failed: %s.", strerror(jcs));
	}

	// wait for threads, check pthread join success
	for (int i = 0; i < threads_len; i++) {
		int ths; // return value
		int pjs = pthread_join(threads[i], (void *) &ths);
		if (pjs) {
			// TODO: why does pthread[1] fail with "No such process"?
			// err("joining pthread #%d thread failed: %s.", i, strerror(pjs));
		} else if (ths) {
			err("pthread returned error code %d.", ths);
		}
	}

	// free lex
	bf_lex_data_free(l->data);
}
