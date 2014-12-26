
#include <stdlib.h>
#include <errno.h>

#include "stack.h"

// link insert
bf_link *bf_link_push(bf_link *lnk, void *data) {
	bf_link *nlnk = malloc(sizeof(bf_link));
	if (nlnk == NULL) { return NULL; }
	nlnk->next = lnk;
	nlnk->data = data;
	return nlnk;
}

// pop from top
void *bf_link_pop(bf_link *lnk) {
	if (lnk != NULL) {
		bf_link *l = lnk->next;
		free(lnk);
		return l;
	} else {
		return NULL;
	}
}

// recursive link traversal
// returns NULL for empty list, else last node.
void *bf_link_pop_bottom(bf_link *lnk) {
	// root is only node
	if (lnk->next == NULL) {
		return NULL;
	}

	// traverse list
	while (lnk->next->next != NULL) {
		lnk = lnk->next;
	}

	free(lnk->next);
	lnk->next = NULL;

	return lnk;
}

void *bf_link_top(bf_link *lnk) {
	return lnk->data;
}

void *bf_link_bottom(bf_link *lnk) {
	while (lnk->next != NULL) {
		lnk = lnk->next;
	}
	return lnk->data;
}

bool bf_link_empty(bf_link *lnk) {
	return (lnk == NULL);
}
