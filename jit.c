
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>

#include <stdio.h>

#include "jit.h"
#include "jit_arch.h"
#include "astree.h"

#ifndef BF_JIT_ARCH_H_
#error jit_arch.h not included
#else

#define EXEC_SIZE 128
#define MEM_SIZE 128

// threadable consumer
void *bf_jit_threadable(void *v) {
	bf_jit *j = (bf_jit *) v;

	while (bf_stack_alive(j->st) || !bf_stack_empty(j->st)) {

		// use get, acts as a queue
		bf_astree *t = (bf_astree *) bf_stack_get(j->st);
		if (t == NULL) {
			if (bf_stack_alive(j->st)) {
				return (void *) 1; // err
			} else {
				continue;
			}
		}

		intptr_t s = bf_jit_emit(j, t);
		if (s != 0) { return (void *) s; }
	}
	return NULL;
}


bf_jit *bf_jit_init(bf_stack *st) {
	// alloc
	bf_jit *j = malloc(sizeof(bf_jit));
	if (j == NULL) { return NULL; }

	j->st = st;
	j->mem = malloc(MEM_SIZE); // 128 bytes
	if (j->mem == NULL) { return NULL; }
	j->exec = mmap(NULL, EXEC_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (j->exec == NULL) { return NULL; }

	return j;
}

int bf_jit_free(bf_jit *j) {
	int mus = munmap(j->exec, EXEC_SIZE);
	if (mus) { return mus; }
	free(j->mem);
	free(j);
	return 0;
}

// tree by tree state mutation
int bf_jit_emit(bf_jit *j, bf_astree *t) {
	char inst[] = {RET, 0x0};
	j->exec = memcpy(j->exec, inst, 1); // copy one byte
	if (j->exec == NULL) { return 1; }
	void(*func)(void) = (void(*)(void)) j->exec;
	func();
	printf("mem: ");
	for (int i = 0; i < sizeof(inst); i++) {
		if (inst[i] == 0) {break;}
		printf("0x%hhx", inst[i]); // print hexadecimal char.
	}
	printf(".\n"); // buffer flush
	return 0;
}

#endif // BF_JIT_ARCH_H_ defined
