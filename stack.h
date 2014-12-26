
#ifndef BF_STACK_H_
#define BF_STACK_H_

#include <pthread.h>
#include <stdbool.h>

// linked list
struct bf_link {
	void *data;
	struct bf_link *next;
};

typedef struct bf_link bf_link;

typedef struct {
	// multithread syncronization
	pthread_cond_t cond;
	pthread_mutex_t mut;
	bool good; // is the stack alive.
	bf_link *lnk;
} bf_stack;

bf_stack *bf_stack_init();
int bf_stack_free(bf_stack *stk);
void bf_stack_push(bf_stack *st, void *data);
void *bf_stack_pop(bf_stack *st);
void *bf_stack_get(bf_stack *stk);
bool bf_stack_empty(bf_stack *st);
bool bf_stack_alive(bf_stack *stk);
void bf_stack_kill(bf_stack *stk);

#endif // BF_STACK_H_
