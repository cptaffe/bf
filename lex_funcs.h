
#ifndef BF_LEX_FUNC_H_
#define BF_LEX_FUNC_H_

#include "bf.h"

// data stored in lex->data
// used in these state functions.
struct lex_data {
	int loop_count;
	bf_stack *st;
};

typedef struct lex_data lex_data;

lex_data *lex_data_init();

void lex_data_free(lex_data *l);

// function signature for state functions

// serves as the default lexer.
void *lex_all(lex *l);

// lexes operators ('+', '-', '<', '>', ',', '.')
void *lex_op(lex *l);

// lexes loops ([, ])
void *lex_loop(lex *l);

#endif // BF_LEX_FUNC_H_
