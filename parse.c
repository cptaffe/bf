
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"
#include "tok.h"

bf_parse *bf_parse_init(bf_stack *st, void *(* func)(struct bf_parse *)) {
	bf_parse *p = malloc(sizeof(bf_parse));
	if (p == NULL) { return NULL; }

	p->st = st;
	p->func = func;
	p->tree = bf_astree_init_root();
	p->loop_count = 0;

	return p;
}

void bf_parse_free(bf_parse *p) {
	free(p);
}

void *bf_parse_all(bf_parse *p) {
	while (bf_stack_alive(p->st)) {
		// use get, acts as a queue
		bf_tok *t = (bf_tok *) bf_stack_get(p->st);
		if (t == NULL) {
			return NULL; // err
		}
		printf("lex'd: %s.\n", t->msg);
		bf_tok_free(t);
		free(t->msg);
	}
	return NULL;
}

// state machine runs functions,
void bf_parse_state(bf_parse *p) {
	// loop until a state function returns NULL.
	while (p->func != NULL) {
		// type conversion for simplicity.
		p->func = (void *(*)(bf_parse *)) p->func(p);
	}
}

// threadable state machine
void *bf_parse_threadable(void *ps) {
	bf_parse *p = (bf_parse *) ps;
	bf_parse_state(p);
	bf_stack_free(p->st);
	return NULL;
}
