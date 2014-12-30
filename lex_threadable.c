
#include "stack.h"
#include "lex_funcs.h"

// threadable wrapper for lex_state
void *lex_state_threadable(void *lx) {
	lex *l = (lex *) lx;
	lex_state(l);
	// poison stack, wake consumer.
	bf_stack_kill(((bf_lex_data *) l->data)->st);
	return NULL;
}
