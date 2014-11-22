
#ifndef BF_TOK_H_
#define BF_TOK_H_

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

#endif // BF_TOK_H_
