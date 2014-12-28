
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> // intptr_t

#include "parse.h"
#include "bf.h" // err

bf_parse *bf_parse_init(bf_stack *st) {
	bf_parse *p = malloc(sizeof(bf_parse));
	if (p == NULL) { return NULL; }

	p->st = st;
	p->out = bf_stack_init();
	p->ctn = bf_stack_init();
	p->tree = bf_astree_init_root();
	p->loop_count = 0;

	return p;
}

void bf_parse_free(bf_parse *p) {
	free(p);
}

// return current tree, set self to new tree.
bf_astree *bf_parse_emit_tree(bf_parse *p) {
	bf_astree *t = p->tree;
	p->tree = bf_astree_init_root();
	if (p->tree == NULL) { return NULL; }
	return t;
}

// tok by tok parser, returns tree if full statement is ready.
bf_astree *bf_parse_tok(bf_parse *p, bf_tok *t) {
	// handle token
	if (t->type == BF_TOK_LB) {
		// account for loop
		p->loop_count++;
		bf_astree *tree = bf_astree_init(BF_ASTREE_LOOP, t);
		if (tree == NULL) { return NULL; }
		int cas = bf_astree_child_add(p->tree, tree);
		if (cas) { return NULL; }

		// push
		bf_stack_push(p->ctn, p->tree); // save old tree ptr
		p->tree = tree; // new tree ptr

		return NULL;

	} else if (t->type == BF_TOK_RB) {
		// account for loop
		p->loop_count--;
		p->tree = (bf_astree *) bf_stack_pop(p->ctn); // pop old tree

	} else if (t->type == BF_TOK_STE) {
		// check that loop_count is 0
		if (p->loop_count == 0) {
			// save current tree to stack, begin new tree.
			return bf_parse_emit_tree(p);
		}
		return NULL;
	} else {
		bf_astree *tree = bf_astree_init(BF_ASTREE_ID, t);
		if (tree == NULL) { return NULL; }
		int cas = bf_astree_child_add(p->tree, tree);
		if (cas) { return NULL; }
	}
	return NULL;
}

// feeds tokens from stack to bf_parse_tok
int bf_parse_do(bf_parse *p) {
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

		bf_astree *tr = bf_parse_tok(p, t);
		if (tr != NULL) { bf_stack_push(p->out, (void *) tr); }
	}

	// is at a returnable point
	if (p->loop_count == 0) {
		if (p->tree == NULL) {
			bf_astree *t = bf_parse_emit_tree(p);
			if (t == NULL) { return 8; }
			bf_astree_rec_free(t);
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
	intptr_t s = bf_parse_do(p);
	if (s) { return (void *) s; } // err
	bf_stack_kill(p->out); // poison stack
	return NULL;
}
