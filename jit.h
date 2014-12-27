
#ifndef BF_JIT_H_
#define BF_JIT_H_

#include "stack.h"

// TODO: use bitfield flags to hold options.

typedef struct {
	bf_stack *st;
	void *exec;
	void *mem;
} bf_jit;

void *bf_jit_threadable(void *v);
bf_jit *bf_jit_init(bf_stack *st);
int bf_jit_free(bf_jit *j);
int bf_jit_emit();

#endif // BF_JIT_H_
