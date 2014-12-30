
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "bytecode.h"
#include "tok.h"

// OS X compatibility
#ifndef PAGESIZE
#include <unistd.h>
#define PAGESIZE (getpagesize())
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#define MEM_PAGES 1

// bytecode emitter init
bf_bc *bf_bc_init(bf_stack *st, int fd) {
	bf_bc *b = malloc(sizeof(bf_bc));
	if (b == NULL) { return NULL; }
	if (fd < 0) {
		b->bc = mmap(NULL, MEM_PAGES * PAGESIZE, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	} else {
		b->fd = fd;
		ftruncate(b->fd, MEM_PAGES * PAGESIZE);
		b->bc = mmap(NULL, MEM_PAGES * PAGESIZE, PROT_WRITE, MAP_SHARED, b->fd, 0);
	}
	if (b->bc == MAP_FAILED) { return NULL; }
	b->pos = 0;

	b->st = st;
	return b;
}

// bytecode emitter free
void bf_bc_free(bf_bc *b) {
	munmap(b->bc, MEM_PAGES * PAGESIZE);
	ftruncate(b->fd, b->pos);
	free(b);
}

static inline size_t ret(uint8_t *mem) {
	mem[0] = 0x0;
	return 1;
}

static inline size_t gen(uint8_t *mem, bf_astree *t) {
	// null tree
	if (t == NULL || (t->type == BF_ASTREE_ROOT && t->chld_num == 0)) {
		return ret(mem);
	}

	size_t len = 0;

	if (t->type != BF_ASTREE_ROOT) {
		if (t->type == BF_ASTREE_ID) {
			bf_tok *tok = (bf_tok *) t->data;
			if (tok->type == BF_TOK_PLUS) {
				mem[len] = BF_BC_MUT; len++; // assign one byte
				int64_t ml = strlen(tok->msg);
				memmove(&mem[len], &ml, sizeof(ml)); // copy 8 bytes
				len += sizeof(ml);
			} else if (tok->type == BF_TOK_MINUS) {
				mem[len] = BF_BC_MUT; len++; // assign one byte
				int64_t ml = -strlen(tok->msg);
				memmove(&mem[len], &ml, sizeof(ml)); // copy 8 bytes
				len += sizeof(ml);
			} else if (tok->type == BF_TOK_GT) {
				mem[len] = BF_BC_AMUT; len++; // assign one byte
				int64_t ml = strlen(tok->msg);
				memmove(&mem[len], &ml, sizeof(ml)); // copy 8 bytes
				len += sizeof(ml);
			} else if (tok->type == BF_TOK_LT) {
				mem[len] = BF_BC_AMUT; len++; // assign one byte
				int64_t ml = -strlen(tok->msg);
				memmove(&mem[len], &ml, sizeof(ml)); // copy 8 bytes
				len += sizeof(ml);
			} else if (tok->type == BF_TOK_DOT) {
				mem[len] = 0x5; len++; // assign one byte
			} else if (tok->type == BF_TOK_COMMA) {
				mem[len] = 0x6; len++; // assign one byte
			} else {
				printf("unknown id.\n");
			}
		} else if (t->type == BF_ASTREE_LOOP) {
			size_t clen = len;
			for (int i = 0; i < t->chld_num; i++) {
				len += gen(&mem[len], t->chld[i]);
			}
			// insert loop instruction using clen
			mem[len] = BF_BC_JMP; len++; // assign one byte
			int64_t ml = (len - clen);
			memmove(&mem[len], &ml, sizeof(ml)); // copy 8 bytes
			len += sizeof(ml);
			printf("loop from %lu to %lu\n", clen, len);
		}
	} else {
		for (int i = 0; i < t->chld_num; i++) {
			len += gen(&mem[len], t->chld[i]);
		}
	}
	return len;
}

// threadable consumer
void *bf_bc_threadable(void *v) {
	bf_bc *b = (bf_bc *) v;

	while (bf_stack_alive(b->st) || !bf_stack_empty(b->st)) {

		// use get, acts as a queue
		bf_astree *t = (bf_astree *) bf_stack_get(b->st);
		if (t == NULL) {
			if (bf_stack_alive(b->st)) {
				return (void *) 1; // err
			} else {
				continue;
			}
		}

		intptr_t s = bf_bc_emit(b, t);
		if (s != 0) { return (void *) s; }
	}
	b->pos += ret(&b->bc[b->pos]);
	return NULL;
}

// generate bytecode
int bf_bc_gen(bf_bc *b, bf_astree *t) {
	b->pos += gen(&b->bc[b->pos], t);
	return 0;
}

// tree by tree state mutation
int bf_bc_emit(bf_bc *b, bf_astree *t) {
	bf_bc_gen(b, t);
	printf("len: %d\n", b->pos);

	// execute machine instructions
	//((void(*)(void)) j->exec)();

	// print instructions
	printf("exec'd: ");
	for (int i = 0; i < b->pos; i++) {
		printf("0x%hhx", b->bc[i]); // print hexadecimal char.
	}
	printf(".\n"); // buffer flush

	return 0;
}
