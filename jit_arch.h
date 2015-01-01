
#ifndef BF_JIT_ARCH_H_
#define BF_JIT_ARCH_H_

// define instructions for each architecture.

// x86 mod-r/m value tables:

// x86_64 architecture
#ifdef __x86_64__

#define PTRSIZE 8

// return instruction.
#define RET 0xc3

// mov <8 bytes>, %rsi
#define MOV_64 0x48, 0xbe

// mov <8 bytes>, %rax
#define MOV_64_RAX 0x48, 0xb8

// mov <8 bytes>, %rdi
#define MOV_64_RDI 0x48, 0xbf

// mov <8 bytes>, %rdx
#define MOV_64_RDX 0x48, 0xba

// mov %rsi, (%rax)
#define MOV_SAVE_RSI 0x48, 0x89, 0x30

// syscall
#define SYSCALL 0x0f, 0x05

// add <1 byte>, (%rsi) // optional lock element.
#define ADD_LIT8 0x48, 0x83, 0x06
#define ADD_LIT8_LOCK 0xf0, ADD_LIT8

// add <1 byte>, %rsi
#define ADD_LIT8_RSI 0x48, 0x83, 0xc6
#define ADD_LIT8_RSI_LOCK 0xf0, ADD_LIT8

// sub <1 byte>, %rsi
#define SUB_LIT8_RSI 0x48, 0x83, 0xee
#define SUB_LIT8_RSI_LOCK 0xf0, ADD_LIT8

// mov (%rsi), %rcx
#define MOV_RSI_RCX 0x48, 0x8b, 0x0e

// loop <1 byte>
#define LOOP 0xe2

// sub <1 byte>, (%rsi), optional lock element.
#define SUB_LIT8 0x83, 0x2e
#define SUB_LIT8_LOCK 0xf0, SUB_LIT8

#else
// x86 architecture
#ifdef __i386__

#else
#error cannot emit code for this architecture.
#endif // __i386__
#endif // __x86_64__

#endif // BF_JIT_ARCH_H_
