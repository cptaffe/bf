
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> // intptr_t

#include "parse.h"
#include "tok.h"
#include "bf.h" // err

bf_parse *bf_parse_init(bf_stack *st) {
	bf_parse *p = malloc(sizeof(bf_parse));
	if (p == NULL) { return NULL; }

	p->st = st;
	p->out = bf_stack_init();
	p->tree = bf_astree_init_root();
	p->loop_count = 0;

	return p;
}

void bf_parse_free(bf_parse *p) {
	free(p);
}

// push tree onto stack
static int shove_tree(bf_parse *p) {
	bf_stack_push(p->out, (void *) p->tree);
	p->tree = bf_astree_init_root();
	if (p->tree == NULL) { return 1; }
	return 0;
}

int bf_parse_all(bf_parse *p) {
	while (bf_stack_alive(p->st) || !bf_stack_empty(p->st)) {

		// use get, acts as a queue
		bf_tok *t = (bf_tok *) bf_stack_get(p->st);
		if (t == NULL) {
			if (bf_stack_alive(p->st)) {
				return 1; // err
			} else {
				continue;
			}
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

		} else if (t->type == BF_TOK_RB) {
			// account for loop
			p->loop_count--;
			return 0;
		} else if (t->type == BF_TOK_STE) {
			// check that loop_count is 0
			if (p->loop_count == 0) {
				// save current tree to stack, begin new tree.
				int i = shove_tree(p);
				if (i) { return 5; }
			}
		} else {
			bf_astree *tree = bf_astree_init(BF_ASTREE_ID, t);
			if (tree == NULL) { return 6; }
			int cas = bf_astree_child_add(p->tree, tree);
			if (cas) { return 7; }
		}
	}

	// is at a returnable point
	if (p->loop_count == 0) {
		if (p->tree == NULL) {
			int i = shove_tree(p);
			if (i) { return 8; }
		}
		return 0;
	} else {
		err("syntax: unmatched bracket.");
		return 9;
	}
}

// threadable state machine
void *bf_parse_threadable(void *ps) {
	bf_parse *p = (bf_parse *) ps;
	// intptr_t avoids int -> void * err.
	intptr_t s = bf_parse_all(p);
	if (s) { return (void *) s; } // err
	return NULL;
}
