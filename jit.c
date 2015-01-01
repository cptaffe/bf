#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

// OS X compatibility
#ifndef PAGESIZE
#include <unistd.h>
#define PAGESIZE (getpagesize())
#endif

// Linux compatibiity
#ifndef MAP_ANON
#define MAP_ANON MAP_ANONYMOUS
#endif

#include "bf.h"
#include "jit.h"
#include "jit_arch.h"
#include "tok.h"

#define EXEC_PAGES 1
#define MEM_PAGES 1

bf_jit *bf_jit_init(bf_stack *st) {
	// alloc
	bf_jit *j = malloc(sizeof(bf_jit));
	if (j == NULL) { return NULL; }

	j->ptr = 0;
	j->exec_pos = 0;
	j->st = st;

	// allocate MEM_PAGES pages.
	j->mem_pages = MEM_PAGES;
	j->mem = mmap(NULL, MEM_PAGES * PAGESIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (j->mem == MAP_FAILED) { return NULL; }

	// allocate EXEC_PAGES pages
	j->exec_pages = EXEC_PAGES;
	j->exec = mmap(NULL, EXEC_PAGES * PAGESIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (j->exec == MAP_FAILED) { return NULL; }

	// purposeful fault
	fprintf(stderr, "page size: %d.\n", PAGESIZE);

	return j;
}

int bf_jit_free(bf_jit *j) {
	int mus = munmap(j->exec, j->exec_pages * PAGESIZE);
	if (mus) { return mus; }

	mus = munmap(j->mem, j->mem_pages * PAGESIZE);
	if (mus) { return mus; }

	free(j);
	return 0;
}

static inline void emit_init(bf_jit *j) {
	// mov mem ptr to rsi
	char mov[] = {MOV_64};
	memcpy(&j->exec[j->exec_pos], mov, sizeof(mov));
	j->exec_pos += sizeof(mov);
	memcpy(&j->exec[j->exec_pos], &j->mem, sizeof(j->mem));
	j->exec_pos += sizeof(j->mem);
}

static inline void emit_add(bf_jit *j, uint8_t num) {
	char add[] = {ADD_LIT8};
	memcpy(&j->exec[j->exec_pos], add, sizeof(add));
	j->exec_pos += sizeof(add);
	memcpy(&j->exec[j->exec_pos], &num, sizeof(num));
	j->exec_pos += sizeof(num);
}

static inline void emit_add_rsi(bf_jit *j, uint8_t num) {
	char add[] = {ADD_LIT8_RSI};
	memcpy(&j->exec[j->exec_pos], add, sizeof(add));
	j->exec_pos += sizeof(add);
	memcpy(&j->exec[j->exec_pos], &num, sizeof(num));
	j->exec_pos += sizeof(num);
}

static inline void emit_sub_rsi(bf_jit *j, uint8_t num) {
	char add[] = {SUB_LIT8_RSI};
	memcpy(&j->exec[j->exec_pos], add, sizeof(add));
	j->exec_pos += sizeof(add);
	memcpy(&j->exec[j->exec_pos], &num, sizeof(num));
	j->exec_pos += sizeof(num);
}

static inline void emit_sub(bf_jit *j, uint8_t num) {
	char add[] = {SUB_LIT8};
	memcpy(&j->exec[j->exec_pos], add, sizeof(add));
	j->exec_pos += sizeof(add);
	memcpy(&j->exec[j->exec_pos], &num, sizeof(num));
	j->exec_pos += sizeof(num);
}

static inline void emit_mov_rsi_rcx(bf_jit *j) {
	char mov[] = {MOV_RSI_RCX};
	memcpy(&j->exec[j->exec_pos], mov, sizeof(mov));
	j->exec_pos += sizeof(mov);
}

static inline void emit_ret(bf_jit *j) {
	char ret[] = {RET};
	memcpy(&j->exec[j->exec_pos], ret, sizeof(ret));
	j->exec_pos += sizeof(ret);
}

static inline void emit_loop(bf_jit *j, uint8_t num) {
	char loop[] = {LOOP};
	memcpy(&j->exec[j->exec_pos], loop, sizeof(loop));
	j->exec_pos += sizeof(loop);
	memcpy(&j->exec[j->exec_pos], &num, sizeof(num));
	j->exec_pos += sizeof(num);
}

// write syscall
static inline void emit_write(bf_jit *j, uint64_t fd) {
	// rax = 0x2000004
	char mov[] = {MOV_64_RAX};
	memcpy(&j->exec[j->exec_pos], mov, sizeof(mov));
	j->exec_pos += sizeof(mov);
	uint64_t n = 0x2000004;
	memcpy(&j->exec[j->exec_pos], &n, sizeof(n));
	j->exec_pos += sizeof(n);

	// rdi = 1 (stdout)
	char mov1[] = {MOV_64_RDI};
	memcpy(&j->exec[j->exec_pos], mov1, sizeof(mov1));
	j->exec_pos += sizeof(mov1);
	memcpy(&j->exec[j->exec_pos], &fd, sizeof(fd));
	j->exec_pos += sizeof(fd);

	// rsi is already pointed at the correct memory

	char mov2[] = {MOV_64_RDI};
	memcpy(&j->exec[j->exec_pos], mov2, sizeof(mov2));
	j->exec_pos += sizeof(mov2);
	n = 1;
	memcpy(&j->exec[j->exec_pos], &n, sizeof(n));
	j->exec_pos += sizeof(n);

	char syscall[] = {SYSCALL};
	memcpy(&j->exec[j->exec_pos], syscall, sizeof(syscall));
	j->exec_pos += sizeof(syscall);
}

static inline void emit_save_ptr(bf_jit *j) {
	char mov[] = {MOV_64_RAX};
	memcpy(&j->exec[j->exec_pos], mov, sizeof(mov));
	j->exec_pos += sizeof(mov);
	memcpy(&j->exec[j->exec_pos], &j->ptr, sizeof(j->ptr));
	j->exec_pos += sizeof(j->ptr);
	char mov2[] = {MOV_SAVE_RSI};
	memcpy(&j->exec[j->exec_pos], mov2, sizeof(mov2));
	j->exec_pos += sizeof(mov2);
}

// TODO: add/sub instructions should not rollover 4 bytes.

// emits executable code from bytecode
int bf_jit_emit(bf_jit *j, bf_astree *t) {
	// null tree
	if (t == NULL || (t->type == BF_ASTREE_ROOT && t->chld_num == 0)) {
		return 0;
	}

	if (t->type != BF_ASTREE_ROOT) {
		if (t->type == BF_ASTREE_ID) {
			bf_tok *tok = (bf_tok *) t->data;
			if (tok->type == BF_TOK_PLUS) {
				emit_add(j, strlen(tok->msg));
			} else if (tok->type == BF_TOK_MINUS) {
				emit_sub(j, strlen(tok->msg));
			} else if (tok->type == BF_TOK_GT) {
				emit_add_rsi(j, strlen(tok->msg));
			} else if (tok->type == BF_TOK_LT) {
				emit_sub_rsi(j, strlen(tok->msg));
			} else if (tok->type == BF_TOK_DOT) {
				emit_write(j, 1);
			} else if (tok->type == BF_TOK_COMMA) {
				err("unsupported as of yet.");
			} else {
				err("unknown id.");
			}
		} else if (t->type == BF_ASTREE_LOOP) {
			int num = j->exec_pos;
			for (int i = 0; i < t->chld_num; i++) {
				bf_jit_emit(j, t->chld[i]);
			}
			// insert loop instruction using clen
			emit_mov_rsi_rcx(j); // move current to rcx
			emit_sub(j, 1); // decrement current
			emit_loop(j, (num - j->exec_pos)); // loop instruction
		}
	} else {
		for (int i = 0; i < t->chld_num; i++) {
			bf_jit_emit(j, t->chld[i]);
		}
	}
	return 0;
}

// threadable consumer
void *bf_jit_threadable(void *v) {
	bf_jit *j = (bf_jit *) v;

	err("exec: %p", j->exec);
	err("mem: %p", j->exec);

	while (bf_stack_alive(j->st) || !bf_stack_empty(j->st)) {

		// use get, acts as a queue
		bf_astree *t = (bf_astree *) bf_stack_get(j->st);
		if (!t) {
			if (bf_stack_alive(j->st)) {
				err("bf_stack_get returned error.");
				return (void *) 1; // err
			} else {
				continue;
			}
		}

		// generate and run
		emit_init(j);
		intptr_t s = bf_jit_emit(j, t);
		if (s != 0) {
			err("bf_bc_emit returned error code %ld.", s);
			return (void *) s;
		}
		emit_ret(j);
		((void(*)()) j->exec)();
		j->exec_pos = 0;
		emit_save_ptr(j);

		// print ptr location
		err("ptr: %llu", j->ptr);
	}

	char *str = malloc(100);
	int len = 0;
	for (int i = 0; i < (j->mem_pages * PAGESIZE); i++) {
		char *mem;
		asprintf(&mem, "%d, ", j->mem[i]);
		memcpy(&str[len], mem, strlen(mem));
		len += strlen(mem);
	}
	err("mem: { %s }.", str);

	if (j->mem[0] > 0) {
		err("success!: %u = 1.", j->mem[0]);
	} else {
		err("failure!: %u != 1.", j->mem[0]);
	}

	return NULL;
}
