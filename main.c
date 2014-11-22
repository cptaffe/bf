
// simple brainfuck interpreter
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // errorno
#include <string.h> // strerror
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution

extern int errno;

#include "lex.h"
#include "bf.h"

int main() {
	lex *l = lex_init(10, stdin);
	if (l == NULL) {fail("allocation of lex failed: %s.", strerror(errno));}
	lex_state(l); // state machine loop.
	lex_free(l);
}
