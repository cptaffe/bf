
#ifndef BF_JIT_ARCH_H_
#define BF_JIT_ARCH_H_

// define instructions for each architecture.

// x86_64 architecture
#ifdef __x86_64__
// return instruction.
#define RET 0xc3

// add instruction, optional lock element.
#define ADD_MEM8_LIT8 0x80, 0x0
#define ADD_MEM8_LIT8_LOCK 0xf0, 0x80, 0x0

// sub instruction, optional lock element.
#define SUB_MEM8_LIT8 0x80, 0x5
#define SUB_MEM8_LIT8_LOCK 0xf0, 0x80, 0x5

#else
// x86 architecture
#ifdef __i386__

#else
#error cannot emit code for this architecture.
#endif // __i386__
#endif // __x86_64__

#endif // BF_JIT_ARCH_H_
