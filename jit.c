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
#include "jit_emit.h" // emit functions

#define EXEC_PAGES 1
#define MEM_PAGES 1

bf_jit *bf_jit_init(bf_stack *st) {
	// alloc
	bf_jit *j = malloc(sizeof(bf_jit));
	if (j == NULL) { return NULL; }

	j->init_pos = 0;
	j->exec_pos = 0;
	j->loop_count = 0;
	j->runnable = false;

	j->mem_mod = 0;
	j->mem_disp = 0;

	j->st = st;
	if (j->st == NULL) { return NULL; }

	j->loop_st = bf_stack_init();
	if (j->loop_st == NULL) { return NULL; }

	// allocate MEM_PAGES pages.
	j->mem_pages = MEM_PAGES;
	if ((j->mem = mmap(NULL,
		MEM_PAGES * PAGESIZE,
		PROT_READ | PROT_WRITE,
		MAP_ANON | MAP_PRIVATE,
		-1,
		0)) == MAP_FAILED) { return NULL; }

	j->si = (int64_t) (intptr_t) j->mem;

	// allocate EXEC_PAGES pages
	j->exec_pages = EXEC_PAGES;
	if ((j->exec = mmap(NULL,
		EXEC_PAGES * PAGESIZE,
		PROT_EXEC | PROT_READ | PROT_WRITE,
		MAP_ANON | MAP_PRIVATE,
		-1,
		0)) == MAP_FAILED) { return NULL; }

	// purposeful fault
	fprintf(stderr, "page size: %d.\n", PAGESIZE);

	return j;
}

int bf_jit_free(bf_jit *j) {

	// unmap mmap'd regions
	int err;
	if ((err = munmap(j->exec, j->exec_pages * PAGESIZE))) { return err; }
	if ((err = munmap(j->mem, j->mem_pages * PAGESIZE))) { return err; }

	// free alloc'd regions
	bf_stack_free(j->loop_st);
	free(j);

	return 0;
}

static char *mem_print(void *mem, int beg, int end, char *fmt) {
	size_t alloc_len = 10;
	char *str = malloc(alloc_len);
	int len = 0;
	for (int i = beg; i < end; i++) {

		// format fmt
		char *s = NULL;
		if (i != (end - 1)) {
			if (asprintf(&s, "%s, ", fmt) < 0) { return NULL; } // asprintf failed
		} else { s = fmt; }

		// print
		char *a_str = NULL;
		int ret = asprintf(&a_str, s, ((uint8_t *) mem)[i]);
		if (ret < 0) { return NULL; } // asprintf failed.

		// reallocate to size
		if (alloc_len < (len + ret)) {
			alloc_len = len + ret;
			if (!(str = realloc(str, alloc_len))) { return NULL; }
		}

		// copy
		memcpy(&str[len], a_str, ret);
		free(a_str); a_str = NULL;
		len += ret;
	}

	// null terminate
	alloc_len++;
	if (!(str = realloc(str, alloc_len))) { return NULL; }
	str[alloc_len - 1] = 0; // null

	return str; // return string.
}

static inline void print_mem(bf_jit *j, int max) {
	// print to string
	char *str = mem_print(j->mem, 0,
	(max == 0 ? ((j->mem_pages * PAGESIZE) - 1): max), "%d");

	if (str == NULL) { return; } // mem_print failed
	err("mem: { %s }.", str);
	free(str);
}

static inline void print_exec(bf_jit *j) {
	if (j->exec_pos == 0) { return; } // nothing to print

	// print to string
	char *str = mem_print(j->exec, 0, j->exec_pos, "0x%x");

	if (str == NULL) { return; } // mem_print failed
	err("exec: { %s }.", str);
	free(str);
}

// TODO: add/sub instructions should not rollover 4 bytes.

// actually emit code for program
static inline void emit_prog(bf_jit *j) {
	// check if something to do
	if (!j->mem_mod && !j->mem_disp) { return; }

	// if mem_mod is non-zero
	if (j->mem_mod != 0) { emit_add(j, j->mem_mod); }
	j->mem_mod = 0;

	// if mem_disp is non-zero
	// 16 bit scaling
	if (j->mem_disp != 0) { emit_add_rsi(j, j->mem_disp); }
	j->mem_disp = 0;
}

// emits executable code from bytecode
int bf_jit_emit(bf_jit *j, bf_tok *t) {
	if (t->type == BF_TOK_PLUS || t->type == BF_TOK_MINUS) {
		if (j->mem_disp) { emit_prog(j); } // clear state
		if (t->type == BF_TOK_PLUS) {
			j->mem_mod += strlen(t->msg);
		} else {
			j->mem_mod -= strlen(t->msg);
		}
	} else if (t->type == BF_TOK_GT || t->type == BF_TOK_LT) {
		if (j->mem_disp) { emit_prog(j); } // clear state
		if (t->type == BF_TOK_GT) {
			j->mem_disp += strlen(t->msg);
		} else {
			j->mem_disp -= strlen(t->msg);
		}
	} else {
		emit_prog(j);
		if (t->type == BF_TOK_DOT) {
			emit_write(j, 1);
		} else if (t->type == BF_TOK_COMMA) {
			err("unsupported as of yet.");
		} else if (t->type == BF_TOK_LB) {
			j->loop_count++;
			bf_stack_push(j->loop_st, (void *) (intptr_t) &j->exec_pos);
		} else if (t->type == BF_TOK_RB) {
			j->loop_count--;
			if (j->loop_count < 0) { return 1; }
			int num = (int) (intptr_t) bf_stack_pop(j->loop_st);
			// emit only if looping something
			if (j->exec_pos - num) {
				// emit_mov_rcx_rsi(j); // move current to rcx
				emit_sub(j, 1); // decrement current
				emit_loop(j, (j->exec_pos - num)); // loop instruction
			}
		} else if (t->type == BF_TOK_STE) {
			// if there is something to run
			if ((j->exec_pos - j->init_pos) && !j->loop_count) {
				emit_save_ptr(j, &j->si);
				emit_ret(j);
				j->runnable = true;
			}
		}
	}
	return 0;
}

// threadable consumer
void *bf_jit_threadable(void *v) {
	bf_jit *j = (bf_jit *) v;

	emit_init(j); // init jit (first instructions)

	err("exec: %p", j->exec);
	err("mem: %p", j->exec);

	while (bf_stack_alive(j->st) || !bf_stack_empty(j->st)) {

		// use get, acts as a queue
		bf_tok *t = (bf_tok *) bf_stack_get(j->st);
		if (!t) {
			if (bf_stack_alive(j->st)) {
				err("bf_stack_get returned error.");
				return (void *) 1; // err
			} else {
				continue;
			}
		}

		// generate
		intptr_t s = bf_jit_emit(j, t);
		if (s != 0) {
			err("bf_bc_emit returned error code %ld.", s);
			return (void *) s;
		}

		// free tok
		bf_tok_free(t);

		// run if runnable
		if (j->runnable) {
			// emit ptr saving instructions
			print_exec(j);
			((void(*)()) j->exec)();
			err("pos is: %llx.", j->si);
			j->runnable = false;
			print_mem(j, 5);
			j->exec_pos = 0; // reset
			emit_init(j); // init jit (first instructions)
		}

		// print ptr location
		// err("ptr: %llu", j->si);
	}

	return NULL;
}
