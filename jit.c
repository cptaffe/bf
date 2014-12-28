#include <stdlib.h>
#include <stdint.h>
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

#include "jit.h"
#include "jit_arch.h"
#include "astree.h"

#define EXEC_PAGES 2
#define MEM_PAGES 2

// global jit as of this moment
bf_jit *bf_current_jit; // bad practice, but it's the only way.

void mem_handler(int sig, siginfo_t *si, void *unused) {
	printf("Got SIGSEGV at address: 0x%lx\n",
	(long) si->si_addr);

	// elongate mmap'd memory.
	bf_jit *j = bf_current_jit;

	// if sigsegv'd outside the page range.
	if ((long) si->si_addr > ((j->mem_pages + 1) * PAGESIZE) && ((j->mem_pages + 1) * PAGESIZE) < (long) si->si_addr) {
		exit(EXIT_FAILURE);
	}

	// remove memprotect on last page
	int mps = mprotect(&j->mem[MEM_PAGES * PAGESIZE], PAGESIZE, PROT_READ | PROT_WRITE);
	if (mps != 0) { exit(EXIT_FAILURE); }

	// add new page, must allocate at
	j->mem = mmap(&j->mem[j->mem_pages * PAGESIZE], PAGESIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE | MAP_FIXED, -1, 0);
	if (j->mem == NULL) { exit(EXIT_FAILURE); }
	j->mem_pages++;

	// memprotect new last page
	mps = mprotect(&j->mem[MEM_PAGES * PAGESIZE], PAGESIZE, PROT_NONE);
	if (mps != 0) { exit(EXIT_FAILURE); }
}

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

	// allocate MEM_PAGES pages.
	j->mem_pages = MEM_PAGES;
	j->mem = mmap(NULL, (MEM_PAGES + 1) * PAGESIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (j->mem == NULL) { return NULL; }

	// memprotect last page & set signal handler
	int mps = mprotect(&j->mem[MEM_PAGES * PAGESIZE], PAGESIZE, PROT_NONE);
	if (mps != 0) { return NULL; }

	// assign memory handler
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = mem_handler;
	int sas = sigaction(SIGSEGV, &sa, NULL);
	if (sas) { return NULL; }

	// allocate EXEC_PAGES pages
	j->exec_pages = EXEC_PAGES;
	j->exec = mmap(NULL, EXEC_PAGES * PAGESIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (j->exec == NULL) { return NULL; }

	// globally accessible (only one instance ever.)
	bf_current_jit = j;

	return j;
}

int bf_jit_free(bf_jit *j) {
	int mus = munmap(j->exec, EXEC_PAGES * PAGESIZE);
	if (mus) { return mus; }

	mus = munmap(j->mem, (MEM_PAGES + 1) * PAGESIZE);
	if (mus) { return mus; }

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
