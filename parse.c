
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"
#include "tok.h"

bf_parse *bf_parse_init(bf_stack *st) {
	bf_parse *p = malloc(sizeof(bf_parse));
	if (p == NULL) { return NULL; }

	p->st = st;
	p->tree = bf_astree_init_root();
	p->loop_count = 0;

	return p;
}

void bf_parse_free(bf_parse *p) {
	free(p);
}

int bf_parse_all(bf_parse *p) {
	while (bf_stack_alive(p->st)) {
		// use get, acts as a queue
		bf_tok *t = (bf_tok *) bf_stack_get(p->st);
		if (t == NULL) {
			return 1; // err
		}

		// handle token
		if (t->type == BF_TOK_LB) {
			// account for loop
			p->loop_count++;
			bf_astree *tree = bf_astree_init(BF_ASTREE_LOOP, t);
			if (tree == NULL) { return 2; }
			int cas = bf_astree_child_add(p->tree, tree);
			if (cas) { return 3; }

			// recurse
			int pas = bf_parse_all(p);
			if (pas) { return 4; }

			break;
		} else if (t->type == BF_TOK_RB) {
			// account for loop
			p->loop_count++;
			return 0;
		} else {
			bf_astree *tree = bf_astree_init(BF_ASTREE_ID, t);
			if (tree == NULL) { return 5; }
			int cas = bf_astree_child_add(p->tree, tree);
			if (cas) { return 6; }
			break;
		}
	}
	return 0;
}

// threadable state machine
void *bf_parse_threadable(void *ps) {
	bf_parse *p = (bf_parse *) ps;
	bf_parse_all(p);
	bf_stack_free(p->st);
	return p->tree;
}
