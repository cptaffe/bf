
#ifndef BF_LEX_H_
#define BF_LEX_H_

typedef struct {
	FILE *file;
	size_t size;
	int len;
	int llen; // lexed len
	char *lexed;
} lex;

// allocate and initiate lexer
lex *lex_init(size_t sz, FILE *file);

// free lexer
void lex_free(lex *l);

// run lexer state machine
void lex_state(lex *l);

#endif // BF_LEX_H_
