
#ifndef BF_JIT_H_
#define BF_JIT_H_

#include <stdint.h>

#include "stack.h"

// TODO: use bitfield flags to hold options.

typedef struct {
	bf_stack *st;
	// byte indexed
	int exec_pages;
	int mem_pages;
	uint8_t *exec;
	uint8_t *mem;
} bf_jit;

void *bf_jit_threadable(void *v);
bf_jit *bf_jit_init(bf_stack *st);
int bf_jit_free(bf_jit *j);
int bf_jit_emit();

#endif // BF_JIT_H_
