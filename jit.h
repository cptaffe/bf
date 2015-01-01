
#ifndef BF_JIT_H_
#define BF_JIT_H_

#include <stdint.h>
#include <stdbool.h>

#include "stack.h"
#include "astree.h"
#include "tok.h"

// TODO: use bitfield flags to hold options.

typedef struct {

	// memory information
	int init_pos;
	int exec_pos;
	int exec_pages;
	int mem_pages;
	uint8_t *exec;
	uint8_t *mem;

	// incomint token stack
	bf_stack *st;

	// loop handling
	int loop_count;
	bf_stack *loop_st;

	// if code is runnable
	bool runnable;

	// to be generated program
	int mem_disp; // memory displacement
	int mem_mod; // memory modification

	// program state
	uint64_t si; // source index
} bf_jit;

// void *bf_jit_threadable(void *v);
bf_jit *bf_jit_init(bf_stack *st);
int bf_jit_free(bf_jit *j);
int bf_jit_emit(bf_jit *j, bf_tok *t);
void *bf_jit_threadable(void *v);

static inline void bf_jit_free_c(bf_jit **j) { bf_jit_free(*j); }

#endif // BF_JIT_H_
