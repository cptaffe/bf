
#ifndef BF_JIT_H_
#define BF_JIT_H_

#include <stdint.h>

#include "stack.h"
#include "astree.h"

// TODO: use bitfield flags to hold options.

typedef struct {
	int exec_pos;
	int exec_pages;
	int mem_pages;
	uint8_t *exec;
	uint8_t *mem;
	bf_stack *st;
	uint64_t ptr;
} bf_jit;

// void *bf_jit_threadable(void *v);
bf_jit *bf_jit_init(bf_stack *st);
int bf_jit_free(bf_jit *j);
int bf_jit_emit(bf_jit *j, bf_astree *t);
void *bf_jit_threadable(void *v);

static inline void bf_jit_free_c(bf_jit **j) { bf_jit_free(*j); }

#endif // BF_JIT_H_
