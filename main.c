
// simple brainfuck interpreter
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // errorno
#include <string.h> // strerror
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution
#include <fcntl.h>

#include "lex.h"
#include "jit.h"
#include "lex_funcs.h"
#include "bf.h"
#include "tok.h"

#define THREADS 2

#define err_fail(...) char *__errstr = strerror(errno); err(__VA_ARGS__, __errstr); free(__errstr); return EXIT_FAILURE;

int main(int argc, char **argv) {
	FILE *file = stdin;

	// argument checking
	if (argc > 2) {
		// print usage instructions on improper usage
		fprintf(stderr, "usage: bf <file>\n");
	} else if (argc == 2) {
		file = fopen(argv[1], "r");
		if (file == NULL) {
			err_fail("file open failed: %s.");
		}
	}

	// init lexer
	lex *l = lex_init(10, file, bf_lex_all);
	if (l == NULL) {
		err_fail("lex alloc failed: %s.");
	}

	// init lex data
	l->data = (void *) bf_lex_data_init();
	if (l->data == NULL) {
		err_fail("lex_data alloc failed: %s.");
	}

	// init jit
	bf_jit *j = bf_jit_init(((bf_lex_data *) l->data)->st);
	if (j == NULL) {
		err_fail("j alloc failed: %s.");
	}

	// lexer state machine thread, check lex_state_threadable create success
	pthread_t threads[THREADS] = {0};
	int lstcs = pthread_create(&threads[0], NULL, lex_state_threadable, (void *) l);
	if (lstcs) {
		err_fail("creating lex_state_threadable thread failed: %s.");
	}

	// jit machine
	int jcs = pthread_create(&threads[1], NULL, bf_jit_threadable, (void *) j);
	if (jcs) {
		err_fail("creating bf_jit_threadable thread failed: %s.");
	}

	// wait for threads, check pthread join success
	for (int i = 0; i < THREADS; i++) {
		int ths; // return value
		int pjs = pthread_join(threads[i], (void *) &ths);
		if (pjs) {
			// TODO: why does pthread[1] fail with "No such process"?
			char *err = strerror(pjs);
			err("joining pthread #%d thread failed: %s.", i, err);
			free(err);
			return EXIT_FAILURE;
		} else if (ths) {
			err("pthread returned error code %d.", ths);
			return EXIT_FAILURE;
		}
	}

	// free lex data
	bf_jit_free(j);
	lex_free(l);
	bf_lex_data_free(l->data);
}
