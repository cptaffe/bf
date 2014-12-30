
#ifndef BF_LEX_THREADABLE_H_
#define BF_LEX_THREADABLE_H_

// cleanup attribute compatible
static inline void lex_free_c(lex **l) { lex_free(*l); }

// fits p_thread function signature,
// takes lex *.
void *lex_state_threadable(void *lx);

#endif // BF_LEX_THREADABLE_H_
