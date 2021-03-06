
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tok.h"
#include "bf.h" // err

// init a token, returns allocated token or null.
bf_tok *bf_tok_init(bf_tok_type_t type, char *msg) {
	bf_tok *t = (bf_tok *) malloc(sizeof(bf_tok));
	if (t == NULL) {
		return NULL;
	} else {
		t->type = type;
		t->msg = msg;
		return t;
	}
}

void bf_tok_free(bf_tok *t) {
	free (t);
}
