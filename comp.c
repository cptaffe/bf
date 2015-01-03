
#include <stdlib.h>
#include <string.h>
#include <pthread.h> // threading

#include "comp.h"
#include "lex_funcs.h"
#include "bf.h"

bf_comp *bf_comp_init(FILE *file) {
	bf_comp *c = malloc(sizeof(bf_comp));
	if (c == NULL) {
		return NULL;
	}

	// init lexer
	if (!(c->l = lex_init(10, file, bf_lex_all))) {
		return NULL;
	}

	// init lex data
	if (!(c->l->data = (void *) bf_lex_data_init())) {
		return NULL;
	}

	// init jit
	if (!(c->j = bf_jit_init(((bf_lex_data *) c->l->data)->st))) {
		return NULL;
	}

	return c;
}

int bf_comp_free(bf_comp *c) {
	// can fail
	if (bf_jit_free(c->j)) {
		return 1;
	}

	// cannot fail
	bf_lex_data_free(c->l->data);
	lex_free(c->l);

	free(c);

	return 0;
}

int bf_comp_run(bf_comp *c) {
	pthread_t threads[2] = {0};

	// create lexer thread
	if (pthread_create(&threads[0], NULL, lex_state_threadable, (void *) c->l)) {
		return 1;
	}

	// create jit thread
	if (pthread_create(&threads[1], NULL, bf_jit_threadable, (void *) c->j)) {
		return 2;
	}

	// wait for threads, check pthread join success
	for (int i = 0; i < 2; i++) {
		int ths; // return value
		if (!pthread_join(threads[i], (void *) &ths)) {
			return 3;
		} else if (ths) {
			return 4;
		}
	}
	return 5;
}
