
// simple brainfuck interpreter

#include <stdio.h>
#include <stdlib.h>

// memset
#include <string.h>

// integer types
#include <stdint.h>

// read/write
#include <unistd.h>

// memory mapping for code generation and execution
#include <sys/mman.h>

typedef struct {
	int type;
	char *msg;
} tok;

typedef struct {
	size_t size;
	int len;
	char *lexed;
} lex;

// init lexer
lex *lex_init(size_t sz) {
	lex *l = malloc(sizeof (lex));
	l->size = sz;
	l->len = 0;
	l->lexed = malloc(sz); // number of bytes
	return l;
}

// free lexer
void lex_free(lex *l) {
	free(l->lexed);
	free(l);
}

char *lex_emit(lex *l) {
	memcpy()
}

char lex_next(lex *l) {
	if (l->len >= l->size) {
		l->len *= 2;
		realloc(l->lexed, l->len);
	}
	char c = getchar();
	if (c != EOF) {l->lexed[l->len++] = c;}
	return c;
}

void lex_back(lex *l) {
	if (l->len > 0) {
		l->len--;
	}
}

char lex_peek(lex *l) {
	char c = lex_next(l);
	lex_back(l);
	return c;
}

void *lex_gt(lex *l) {
	while ((c = lex_next(l)) != EOF && c == '>') {

	}
	lex_back(l);
}

void *lex_all(lex *l) {
	char c;
	while ((c = lex_peek(l)) != EOF) {
		switch(c) {
			case '>':
				printf("ha!\n");
				return (void *) lex_gt;
		}
	}
	return NULL;
}

// finite state machine!
void lex_state(lex *l) {
	void *func = lex_all;
	while (func != NULL) {
		func = ((void *(*)(lex *)) func)(l);
	}
}

int main() {

	lex *l = lex_init(10);
	lex_state(l); // state machine loop.
	lex_free(l);
}
