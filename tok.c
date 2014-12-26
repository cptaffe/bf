
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tok.h"
#include "bf.h" // err

// init a token, returns allocated token or null.
tok *tok_init(int type, char *msg) {
	tok *t = malloc(sizeof(tok));
	if (t == NULL) {
		return NULL;
	} else {
		t->type = type;
		t->msg = msg;
		return t;
	}
}

void tok_free(tok *t) {
	free (t);
}
