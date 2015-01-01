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

	j->si = 0;
	j->init_pos = 0;
	j->exec_pos = 0;
	j->loop_count = 0;
	j->runnable = false;

	j->st = st;
	if (j->st == NULL) { return NULL; }

	j->loop_st = bf_stack_init();
	if (j->loop_st == NULL) { return NULL; }

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

	bf_stack_free(j->loop_st);

	free(j);
	return 0;
}

static void print_mem(bf_jit *j, int max) {
	if (max < 0) { max = (j->mem_pages * PAGESIZE); }
	char *str = malloc(100);
	int len = 0;
	for (int i = 0; i < max; i++) {
		char *mem;
		if (i == (max - 1)) {
			asprintf(&mem, "%d", j->mem[i]);
		} else {
			asprintf(&mem, "%d, ", j->mem[i]);
		}
		memcpy(&str[len], mem, strlen(mem));
		len += strlen(mem);
	}
	err("mem: { %s }.", str);
}

static void print_exec(bf_jit *j) {
	char *str = malloc(100);
	int len = 0;
	int max = j->exec_pos;
	for (int i = 0; i < max; i++) {
		char *mem;
		if (i == (max - 1)) {
			asprintf(&mem, "%x", j->exec[i]);
		} else {
			asprintf(&mem, "%x, ", j->exec[i]);
		}
		memcpy(&str[len], mem, strlen(mem));
		len += strlen(mem);
	}
	err("exec: { %s }.", str);
}

// TODO: add/sub instructions should not rollover 4 bytes.

// actually emit code for program
static void emit_prog(bf_jit *j) {
	// check if something to do
	if (!j->mem_mod && !j->mem_disp) { return; }

	// if mem_mod needs coding
	if (j->mem_mod > 0) { emit_add(j, j->mem_mod); }
	else if (j->mem_mod < 0) { emit_sub(j, j->mem_mod); }
	j->mem_mod = 0;

	if (j->mem_disp > 0) { emit_add_rsi(j, j->mem_disp); }
	else if (j->mem_disp < 0) { emit_sub_rsi(j, j->mem_disp); }
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
			bf_stack_push(j->loop_st, (void *) (intptr_t) j->exec_pos);
		} else if (t->type == BF_TOK_RB) {
			j->loop_count--;
			if (j->loop_count < 0) { return 1; }
			int num = (int) (intptr_t) bf_stack_pop(j->loop_st);
			// emit only if looping something
			if (j->exec_pos - num) {
				emit_mov_rsi_rcx(j); // move current to rcx
				emit_sub(j, 1); // decrement current
				int diff = (num - j->exec_pos);
				emit_loop(j, diff); // loop instruction
			}
		} else if (t->type == BF_TOK_STE) {
			// if there is something to run
			if ((j->exec_pos - j->init_pos) && !j->loop_count) {
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

		// run if runnable
		if (j->runnable) {
			print_exec(j);
			((void(*)()) j->exec)();
			//emit_save_ptr(j);
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
