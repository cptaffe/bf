
// simple brainfuck interpreter

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <stdint.h> // integer types
#include <unistd.h> // read/write
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution

enum {
	TOK_PLUS,
	TOK_MINUS,
	TOK_GT,
	TOK_LT,
	TOK_DOT,
	TOK_COMMA
};

typedef struct {
	int type;
	char *msg;
} tok;

// concurrent, mutex-based channel
// similar to channels in Go
typedef struct {
	void *data;
	// some way to know if its been read
	pthread_mutex_t lock;
} chan;

// init channel
chan *chan_init() {
	chan *c = malloc(sizeof (chan));
	return c;
}

void chan_send(void *d) {
	// do stuff... idk.
}

void chan_free(chan *c) {
	free(c);
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

void lex_dump(lex *l) {
	l->llen -= l->len; // preserve offset
	if (l->llen > 0) {
		// copy back preserved
		memcpy(l->lexed, &l->lexed[l->len], l->llen);
	}
	l->len = 0; // reset length
}

char *lex_emit(lex *l) {
	char *copy = malloc(l->len + 1);
	memcpy(copy, l->lexed, l->len);
	copy[l->len] = '\0'; // null terminate
	lex_dump(l);
	return copy;
}

char lex_next(lex *l) {
	if (l->len >= l->size) {
		l->size *= 2;
		l->lexed = realloc(l->lexed, l->size);
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
	char gc; // gotten char
	if ((gc = lex_peek(l)) == EOF && !(gc == '>' || gc == '<' || gc == '+' || gc == '-')) {
		return NULL; // err, must exit
	}

	char c;
	while ((c = lex_next(l)) != EOF && c == gc) {
		// printf("got: %c\n", c);
	}
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
			lex_next(l); lex_dump(l); // eat unknown
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
