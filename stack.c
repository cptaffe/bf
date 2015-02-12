
#include <stdlib.h>
#include <errno.h>

#include "stack.h"

static inline int mut_cond_get(bf_stack *stk) {
	int mls = pthread_mutex_lock(&stk->mut);
	if (mls) { return 1; }
	while (bf_stack_empty(stk) && bf_stack_alive(stk)) {
		int cws = pthread_cond_wait(&stk->cond, &stk->mut);
		if (cws) { return 2; }
	}
	if (!bf_stack_alive(stk)) { return 3; } // die on dead
	return 0;
}

// initialize stack
bf_stack *bf_stack_init() {
	// alloc
	bf_stack *stk = (bf_stack *) malloc(sizeof(bf_stack));
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
		return 1;
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
	int mls = pthread_mutex_lock(&stk->mut);
	if (mls) { return; }

	// do work
	stk->lnk = bf_link_push(stk->lnk, data);

	// signal & unlock
	pthread_mutex_unlock(&stk->mut);
	pthread_cond_signal(&stk->cond);
}

// pop ptr from stack
void *bf_stack_pop(bf_stack *stk) {
	// lock & wait for signal
	int s = mut_cond_get(stk);
	if (s) { return NULL; }

	// get data
	void *data = bf_link_top(stk->lnk);
	stk->lnk = bf_link_pop(stk->lnk);

	// unlock
	pthread_mutex_unlock(&stk->mut);
	return data;
}

// get ptr from stack, without removing.
void *bf_stack_top(bf_stack *stk) {
	// lock & wait for signal
	int s = mut_cond_get(stk);
	if (s) { return NULL; }

	// get data
	void *data = bf_link_top(stk->lnk);

	// unlock
	pthread_mutex_unlock(&stk->mut);
	return data;
}

// get ptr from queue, without removing.
void *bf_stack_bottom(bf_stack *stk) {
	// lock & wait for signal
	int s = mut_cond_get(stk);
	if (s) { return NULL; }

	// get data
	void *data = bf_link_bottom(stk->lnk);

	// unlock
	pthread_mutex_unlock(&stk->mut);
	return data;
}

// get ptr from queue
void *bf_stack_get(bf_stack *stk) {
	// lock & wait for signal
	int s = mut_cond_get(stk);
	if (s) { return NULL; }

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
