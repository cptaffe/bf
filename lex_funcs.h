
#ifndef BF_LEX_FUNC_H_
#define BF_LEX_FUNC_H_

#include <stdio.h>

#include "bf.h"
#include "lex.h"

// provide threadable lex wrapper
#include "lex_threadable.h"

// data stored in lex->data
// used in these state functions.
typedef struct {
	int loop_count;
	bf_stack *st;
} bf_lex_data;

bf_lex_data *bf_lex_data_init();
void bf_lex_data_free(bf_lex_data *l);

// cleanup attribute compatible
static inline void bf_lex_data_free_c(bf_lex_data **ld) { bf_lex_data_free(*ld); }

// function signature for state functions

// serves as the default lexer.
void *bf_lex_all(lex *l);

// lexes operators ('+', '-', '<', '>', ',', '.')
void *bf_lex_op(lex *l);

// lexes loops ([, ])
void *bf_lex_loop(lex *l);

// lexes newlines (\n)
void *bf_lex_newline(lex *l);

#endif // BF_LEX_FUNC_H_
