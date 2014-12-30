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
#include "tok.h"
#include "bytecode.h"

#define EXEC_PAGES 1
#define MEM_PAGES 1

bf_jit *bf_jit_init(void *bc) {
	// alloc
	bf_jit *j = malloc(sizeof(bf_jit));
	if (j == NULL) { return NULL; }

	j->exec_pos = 0;

	// allocate MEM_PAGES pages.
	j->mem_pages = MEM_PAGES;
	j->mem = mmap(NULL, MEM_PAGES * PAGESIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (j->mem == NULL) { return NULL; }

	// allocate EXEC_PAGES pages
	j->exec_pages = EXEC_PAGES;
	j->exec = mmap(NULL, EXEC_PAGES * PAGESIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (j->exec == NULL) { return NULL; }

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
