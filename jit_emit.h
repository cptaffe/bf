
#ifndef BF_JIT_EMIT_H_
#define BF_JIT_EMIT_H_

#include <string.h>

#include "jit.h"
#include "jit_arch.h"

static inline void emit_init(bf_jit *j) {
	// mov mem ptr to rsi
	char mov[] = {MOV_64};
	memcpy(&j->exec[j->exec_pos], mov, sizeof(mov));
	j->exec_pos += sizeof(mov);
	memcpy(&j->exec[j->exec_pos], &j->si, sizeof(j->si));
	j->exec_pos += sizeof(j->si);
	j->init_pos = j->exec_pos;
}

static inline void emit_add(bf_jit *j, uint8_t num) {
	char add[] = {ADD_LIT8};
	memcpy(&j->exec[j->exec_pos], add, sizeof(add));
	j->exec_pos += sizeof(add);
	j->exec[j->exec_pos] = num;
	j->exec_pos += sizeof(num);
}

static inline void emit_add_rsi(bf_jit *j, uint8_t num) {
	char add[] = {ADD_LIT8_SI};
	memcpy(&j->exec[j->exec_pos], add, sizeof(add));
	j->exec_pos += sizeof(add);
	j->exec[j->exec_pos] = num;
	j->exec_pos += sizeof(num);
}

static inline void emit_ret(bf_jit *j) {
	char ret[] = {RET};
	memcpy(&j->exec[j->exec_pos], ret, sizeof(ret));
	j->exec_pos += sizeof(ret);
}

static inline void emit_mov_rcx_rsi(bf_jit *j) {
	char add[] = {MOV_CX_SI};
	memcpy(&j->exec[j->exec_pos], add, sizeof(add));
	j->exec_pos += sizeof(add);
}

static inline void emit_loop(bf_jit *j, uint8_t num) {
	char loop[] = {LOOP};
	memcpy(&j->exec[j->exec_pos], loop, sizeof(loop));
	j->exec_pos += sizeof(loop);
	j->exec[j->exec_pos] = num;
	j->exec_pos += sizeof(num);
}

// write syscall
static inline void emit_write(bf_jit *j, uint64_t fd) {
	// rax = 0x2000004
	char mov[] = {MOV_AX_64};
	memcpy(&j->exec[j->exec_pos], mov, sizeof(mov));
	j->exec_pos += sizeof(mov);
	uint64_t s = SYSCALL_WRITE;
	memcpy(&j->exec[j->exec_pos], &s, sizeof(s));
	j->exec_pos += sizeof(s);

	// rdi = 1 (stdout)
	char mov1[] = {MOV_DI_64};
	memcpy(&j->exec[j->exec_pos], mov1, sizeof(mov1));
	j->exec_pos += sizeof(mov1);
	j->exec[j->exec_pos] = fd;
	j->exec_pos += sizeof(fd);

	// rsi is already pointed at the correct memory

	char mov2[] = {MOV_DX_64};
	memcpy(&j->exec[j->exec_pos], mov2, sizeof(mov2));
	j->exec_pos += sizeof(mov2);
	j->exec[j->exec_pos] = (uint8_t) 1;
	j->exec_pos += sizeof(uint8_t);

	char syscall[] = {SYSCALL};
	memcpy(&j->exec[j->exec_pos], syscall, sizeof(syscall));
	j->exec_pos += sizeof(syscall);

	// fix syscall register death.
	char mov3[] = {MOV_64};
	memcpy(&j->exec[j->exec_pos], mov3, sizeof(mov3));
	j->exec_pos += sizeof(mov3);
	j->exec[j->exec_pos] = j->si;
	j->exec_pos += sizeof(j->si);
}

// save current storage index to ptr
static inline void emit_save_ptr(bf_jit *j, uint64_t *ptr) {
	// mov si to ax
	char mov_si_ax[] = {MOV_SI_AX};
	memcpy(&j->exec[j->exec_pos], mov_si_ax, sizeof(mov_si_ax));
	j->exec_pos += sizeof(mov_si_ax);
	// mov ax to ptr
	char mov_si_64[] = {MOV_AX_LIT64};
	memcpy(&j->exec[j->exec_pos], mov_si_64, sizeof(mov_si_64));
	j->exec_pos += sizeof(mov_si_64);
	memcpy(&j->exec[j->exec_pos], &ptr, sizeof(ptr));
	j->exec_pos += sizeof(ptr);
}

#endif
