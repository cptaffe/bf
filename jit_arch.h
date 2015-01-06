
#ifndef BF_JIT_ARCH_H_
#define BF_JIT_ARCH_H_

// define instructions for each architecture.

// x86 mod-r/m value tables:

// x86_64 architecture
#ifdef __x86_64__

// mov
#define MOV_64 0x48, 0xbe /* mov <8 bytes>, %rsi */
#define MOV_CX_SI 0x48, 0x8b, 0x0e /* mov %rcx, (%rsi) */
#define MOV_DX_64 0x48, 0xba /* mov <8 bytes>, %rdx */
#define MOV_DI_64 0x48, 0xbf /* mov <8 bytes>, %rdi */
#define MOV_AX_64 0x48, 0xb8 /* mov <8 bytes>, %rdi */

// add
#define ADD_LIT8 0x80, 0x06 /* add <1 byte>, (%rsi) */
#define ADD_LIT8_LOCK 0xf0, ADD_LIT8 /* add <1 byte>, (%rsi) (locking) */
#define ADD_LIT8_SI 0x48, 0x83, 0xc6 /* add <1 byte>, %rsi */
#define ADD_LIT8_SI_LOCK 0xf0, ADD_LIT8 /* add <1 byte>, %rsi (locking) */

// special
#define LOOP 0xe2 /* loop <1 byte offset> */
#define SYSCALL 0x0f, 0x05 /* syscall */
#define RET 0xc3 /* ret */

// saving rsi
#define MOV_SI_AX 0x48, 0x89, 0xf0 /* mov %rsi, %rax */
#define MOV_AX_LIT64 0x48, 0xa3 /* mov %rax, <8 bytes> */

#else
// x86 architecture
#ifdef __i386__

#else
#error cannot emit code for this architecture.
#endif // __i386__
#endif // __x86_64__

#endif // BF_JIT_ARCH_H_
