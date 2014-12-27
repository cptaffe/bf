
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#include "jit.h"
#include "jit_arch.h"

#define EXEC_SIZE 128
#define MEM_SIZE 128

// threadable consumer
void *bf_jit_threadable(void *v) {
	bf_jit *j = (bf_jit *) v;
	intptr_t s = bf_jit_emit(j);
	return (void *) s;
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

int bf_jit_emit(bf_jit *j) {
	return 0;
}
