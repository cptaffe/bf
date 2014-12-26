
#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"

// link insert
static bf_link *bf_link_push(bf_link *lnk, void *data) {
	bf_link *nlnk = malloc(sizeof(bf_link));
	if (nlnk == NULL) { return NULL; }
	nlnk->next = lnk;
	nlnk->data = data;
	return nlnk;
}

// pop from top
static void *bf_link_pop(bf_link *lnk) {
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
static void *bf_link_pop_bottom(bf_link *lnk) {
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

static void *bf_link_top(bf_link *lnk) {
	return lnk->data;
}

static void *bf_link_bottom(bf_link *lnk) {
	while (lnk->next != NULL) {
		lnk = lnk->next;
	}
	return lnk->data;
}

static bool bf_link_empty(bf_link *lnk) {
	return (lnk == NULL);
}

// initialize stack
bf_stack *bf_stack_init() {
	// alloc
	bf_stack *stk = malloc(sizeof(bf_stack));
	if (stk == NULL) { return NULL; }

	stk->good = true; // it lives.
	stk->lnk = NULL;

	// init conditional & mutex
	int pcs = pthread_cond_init(&stk->cond, NULL);
	if (pcs) { return NULL; }
	int pms = pthread_mutex_init(&stk->mut, NULL);
	if (pms) { return NULL; }

	return stk;
}

// free stack, must be empty.
int bf_stack_free(bf_stack *stk) {

	// dealloc all links
	if (stk->lnk != NULL) {
		return false;
	}

	// destroy conditional & mutex
	int cds = pthread_cond_destroy(&stk->cond);
	if (cds) { return cds; }
	int mds = pthread_mutex_destroy(&stk->mut);
	if (mds) { return mds; }

	free(stk);
	return 0;
}

// is the stack empty?
bool bf_stack_empty(bf_stack *stk) {
	return bf_link_empty(stk->lnk);
}

// push ptr to stack
void bf_stack_push(bf_stack *stk, void *data) {
	// lock
	pthread_mutex_lock(&stk->mut);

	// do work
	stk->lnk = bf_link_push(stk->lnk, data);

	// signal & unlock
	pthread_cond_signal(&stk->cond);
	pthread_mutex_unlock(&stk->mut);
}

// pop ptr from stack
void *bf_stack_pop(bf_stack *stk) {
	// lock & wait for signal
	pthread_mutex_lock(&stk->mut);
	while (bf_stack_empty(stk) && bf_stack_alive(stk)) {
		pthread_cond_wait(&stk->cond, &stk->mut);
	}
	if (!bf_stack_alive(stk)) { return NULL; } // die on dead

	// get data
	void *data = bf_link_top(stk->lnk);
	stk->lnk = bf_link_pop(stk->lnk);

	// unlock
	pthread_mutex_unlock(&stk->mut);
	return data;
}

// get ptr from queue
void *bf_stack_get(bf_stack *stk) {
	// lock & wait for signal
	pthread_mutex_lock(&stk->mut);
	while (bf_stack_empty(stk) && bf_stack_alive(stk)) {
		pthread_cond_wait(&stk->cond, &stk->mut);
	}
	if (!bf_stack_alive(stk)) { return NULL; } // die on dead

	// get data
	void *data = bf_link_bottom(stk->lnk);
	bf_link *l = bf_link_pop_bottom(stk->lnk);
	if (l == NULL) { stk->lnk = NULL; }

	// unlock
	pthread_mutex_unlock(&stk->mut);
	return data;
}

// has the stack been poisoned?
bool bf_stack_alive(bf_stack *stk) {
	return stk->good;
}

// poison the stack
void bf_stack_kill(bf_stack *stk) {
	stk->good = false;
	pthread_cond_broadcast(&stk->cond); // wake all threads
}
