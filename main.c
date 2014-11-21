
// simple brainfuck interpreter

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <stdint.h> // integer types
#include <unistd.h> // read/write
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution

typedef struct {
	int type;
	char *msg;
} tok;

// concurrent, mutex-based channel
// similar to channels in Go
typedef struct {
	void *data;
	pthread_mutex_t lock;
} chan;

// init channel
chan *chan_init() {
	chan *c = malloc(sizeof (chan));
	return c;
}

typedef struct {
	size_t size;
	int len;
	int llen; // lexed len
	char *lexed;
} lex;

// init lexer
lex *lex_init(size_t sz) {
	lex *l = malloc(sizeof (lex));
	l->size = sz;
	l->len = 0;
	l->llen = 0;
	l->lexed = malloc(sz); // number of bytes
	return l;
}

// free lexer
void lex_free(lex *l) {
	free(l->lexed);
	free(l);
}

char *lex_emit(lex *l) {
	char *copy = malloc(l->len + 1);
	memcpy(copy, l->lexed, l->len);
	copy[l->len] = '\0'; // null terminate
	l->len = 0; // reset length
	l->llen = 0;
	return copy;
}

char lex_next(lex *l) {
	if (l->len >= l->size) {
		l->size *= 2;
		realloc(l->lexed, l->size);
	}
	if (l->llen > l->len) {
		return l->lexed[l->len++];
	} else {
		char c = getchar();
		// printf("next! got: %c\n", c);
		if (c != EOF) {
			l->lexed[l->len++] = c;
			l->llen++; // increase lexed length
		}
		return c;
	}
}

void lex_back(lex *l) {
	if (l->len > 0) {
		l->len--;
	}
	// printf("back! dropped: %c\n", l->lexed[l->len]);
}

char lex_peek(lex *l) {
	char c = lex_next(l);
	lex_back(l);
	return c;
}

void *lex_all(lex *l);

void *lex_gt(lex *l) {
	char c;
	while ((c = lex_next(l)) != EOF && (c == '>' || c == '<' || c == '+' || c == '-')) {}
	lex_back(l);
	printf("lexed: %s\n", lex_emit(l));
	return lex_all;
}

void *lex_all(lex *l) {
	char c;
	while ((c = lex_peek(l)) != EOF) {
		if (c == '>' || c == '<' || c == '+' || c == '-') {
			return lex_gt;
		} else {
			return NULL;
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
