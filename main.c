
// simple brainfuck interpreter
#include <stdlib.h>
#include <pthread.h> // threading
#include <sys/mman.h> // memory mapping for code generation and execution

#include "lex.h"

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

int main() {
	lex *l = lex_init(10, 1);
	lex_state(l); // state machine loop.
	lex_free(l);
}
