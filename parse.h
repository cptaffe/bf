
#ifndef BF_PARSE_H_
#define BF_PARSE_H_

#include "astree.h"
#include "stack.h"
#include "tok.h"

struct bf_parse {
	int loop_count;
	bf_stack *st;
	bf_stack *out;
	bf_stack *ctn; // current tree nodes
	bf_astree *tree;
};

typedef struct bf_parse bf_parse;

int bf_parse_do(bf_parse *p);
void *bf_parse_threadable(void *ps);
bf_parse *bf_parse_init(bf_stack *st);
void bf_parse_free(bf_parse *p);
bf_astree *bf_parse_tok(bf_parse *p, bf_tok *t);
bf_astree *bf_parse_emit_tree(bf_parse *p);

#endif // BF_PARSE_H_
