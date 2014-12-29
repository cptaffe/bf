
// simple brainfuck interpreter
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // errorno
#include <string.h> // strerror
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution

#include "lex.h"
#include "parse.h"
#include "bytecode.h"
#include "lex_funcs.h"
#include "bf.h"
#include "tok.h"

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
	lex *l = lex_init(10, stdin, bf_lex_all);
	if (l == NULL) {
		fail("lex alloc failed: %s.", strerror(errno));
	}

	// init lex data
	l->data = (void *) bf_lex_data_init();
	if (l->data == NULL) {
		fail("lex_data alloc failed: %s.", strerror(errno));
	}

	// init parser
	bf_parse *p = bf_parse_init(((lex_data *) l->data)->st);
	if (p == NULL) {
		fail("parse alloc failed: %s.", strerror(errno));
	}

	// init jit
	bf_bc *b = bf_bc_init(p->out);
	if (b == NULL) {
		fail("jit alloc failed: %s.", strerror(errno));
	}

	// lexer state machine thread, check lex_state_threadable create success
	pthread_t threads[3];
	int lstcs = pthread_create(&threads[0], NULL, lex_state_threadable, (void *) l);
	if (lstcs) {
		fail("creating lex_state_threadable thread failed: %s.", strerror(lstcs));
	}

	// parser, check parse_state_threadable create success
	int pcs = pthread_create(&threads[1], NULL, bf_parse_threadable, (void *) p);
	if (pcs) {
		fail("creating bf_parse_threadable thread failed: %s.", strerror(pcs));
	}

	// btecode emittance
	int bcs = pthread_create(&threads[1], NULL, bf_bc_threadable, (void *) b);
	if (bcs) {
		fail("creating bf_jit_threadable thread failed: %s.", strerror(pcs));
	}

	// wait for threads, check pthread join success
	for (int i = 0; i < 2; i++) {
		int ths; // return value
		int pjs = pthread_join(threads[i], (void *) &ths);
		if (pjs) {
			// TODO: why does pthread[1] fail with "No such process"?
			// err("joining pthread #%d thread failed: %s.", i, strerror(pjs));
		} else if (ths) {
			err("pthread returned error #%d.", ths);
		}
	}

	// free lex
	bf_lex_data_free(l->data);
	lex_free(l);

	// free parse
	bf_parse_free(p);
}
