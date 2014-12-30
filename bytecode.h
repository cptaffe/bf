
#ifndef BF_BYTECODE_H_
#define BF_BYTECODE_H_

#include <stdint.h>

#include "stack.h"
#include "astree.h"

/*
This header describes the mechanism for emitting bf bytecode.
Codes are described in the following fashion:
	<op>:<arg1 type>:...
*/

// maximum instruction length
#define BF_BC_MAXINST 10

enum bf_bc_code {
	BF_BC_EEXEC = 0x0, // end execution
	// op:64 (signed 8 byte value)
	BF_BC_MUT = 0x1, // mutate value at current address
	// op:64 (signed 8 byte value)
	BF_BC_AMUT = 0x2, // mutate current address
	// op:64 (signed 8 byte value)
	BF_BC_JMP = 0x3 // jmp to offset
};

// bytecode
typedef struct {
	int fd;
	int pos;
	uint8_t *bc;
	bf_stack *st;
} bf_bc;

bf_bc *bf_bc_init(bf_stack *st, int fd);
void bf_bc_free(bf_bc *bc);
int bf_bc_gen(bf_bc *bc, bf_astree *t);
void *bf_bc_threadable(void *v);
int bf_bc_emit(bf_bc *b, bf_astree *t);

#endif // BF_BYTECODE_H_
