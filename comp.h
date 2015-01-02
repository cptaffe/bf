#ifndef BF_COMP_H_
#define BF_COMP_H_

#include <stdio.h>

#include "lex.h"
#include "jit.h"

// compiler struct
typedef struct {
	lex *l;
	bf_jit *j;
} bf_comp;

bf_comp *bf_comp_init(FILE *file);
int bf_comp_free(bf_comp *c);
int bf_comp_run(bf_comp *c);

#endif // BF_COMP_H_
