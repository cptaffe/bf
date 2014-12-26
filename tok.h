
#ifndef BF_TOK_H_
#define BF_TOK_H_

enum {
	TOK_PLUS,
	TOK_MINUS,
	TOK_GT,
	TOK_LT,
	TOK_DOT,
	TOK_COMMA,
	TOK_LB,
	TOK_RB
};

typedef struct {
	int type;
	char *msg;
} tok;

tok *tok_init(int type, char *msg);
void tok_free(tok *t);

#endif // BF_TOK_H_
