
#ifndef BF_LEX_H_
#define BF_LEX_H_

typedef struct {
	int fd;
	size_t size;
	int len;
	int llen; // lexed len
	char *lexed;
} lex;

lex *lex_init(size_t sz, int fd);
void lex_free(lex *l);
void lex_state(lex *l);

#endif // BF_LEX_H_
