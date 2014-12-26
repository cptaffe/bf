
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "astree.h"

// init root node.
bf_astree *bf_astree_init_root() {
	bf_astree *t = malloc(sizeof(bf_astree));
	if (t == NULL) { return NULL; }

	t->type = BF_ASTREE_ROOT;

	return t;
}

// init tree
bf_astree *bf_astree_init(bf_astree_type_t type, void *data) {
	bf_astree *t = malloc(sizeof(bf_astree));
	if (t == NULL) { return NULL; }

	t->type = type;
	t->data = data;
	t->chld_num = 0;
	t->chld = NULL;

	return t;
}

// free tree
void bf_astree_free(bf_astree* t) {
	free(t);
}

int bf_astree_child_add(bf_astree *t, bf_astree *c) {
	// realloc
	t->chld_num++;
	if (t->chld_num == 1) {
		t->chld = malloc(sizeof(bf_astree));
	} else {
		t->chld = realloc(t->chld, sizeof(bf_astree) * t->chld_num);
	}
	if (t->chld == NULL) {
		// return error if errno is set, else 1.
		if (errno) {
			return errno;
		} else { return 1; }
	}

	// save
	t->chld[t->chld_num - 1] = c;

	return 0;
}

int bf_astree_child_rm(bf_astree *t, int index) {
	// check index is in range
	if (index < 0 && index >= t->chld_num) {
		return 1;
	}

	// check index is not last index
	if ((t->chld_num - 1) != index) {
		void *dest = memcpy(t->chld[index], t->chld[index + 1], (t->chld_num - index) * sizeof(bf_astree *));
		if (dest == NULL) {
			// return error if errno is set, else 1.
			if (errno) {
				return errno;
			} else { return 1; }
		}
	}
	t->chld_num--;
	return 0;
}
