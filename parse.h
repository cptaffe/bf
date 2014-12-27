
#ifndef BF_PARSE_H_
#define BF_PARSE_H_

#include "astree.h"
#include "stack.h"

struct bf_parse {
	int loop_count;
	bf_stack *st;
	bf_stack *out;
	bf_astree *tree;
};

typedef struct bf_parse bf_parse;

void bf_parse_state(bf_parse *p);
void *bf_parse_threadable(void *ps);
bf_parse *bf_parse_init(bf_stack *st);
void bf_parse_free(bf_parse *p);
int bf_parse_all(bf_parse *p);

#endif // BF_PARSE_H_
