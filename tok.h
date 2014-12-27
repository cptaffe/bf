
#ifndef BF_TOK_H_
#define BF_TOK_H_

typedef enum {
	BF_TOK_PLUS,
	BF_TOK_MINUS,
	BF_TOK_GT,
	BF_TOK_LT,
	BF_TOK_DOT,
	BF_TOK_COMMA,
	BF_TOK_LB,
	BF_TOK_RB,
	BF_TOK_STE // statement end
} bf_tok_type_t;

typedef struct {
	bf_tok_type_t type;
	char *msg;
} bf_tok;

bf_tok *bf_tok_init(int type, char *msg);
void bf_tok_free(bf_tok *t);

#endif // BF_TOK_H_
