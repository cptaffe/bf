
#ifndef BF_BF_H_
#define BF_BF_H_

#include "stack.h" // define stack

// awesome macros which print the file and line number which these
// calls originated from.
#define err(...) ({char *__str; int __asret = asprintf(&__str, __VA_ARGS__); if (!(__asret < 0)) {bf_err("%s:%d::%s: %s", __FILE__, __LINE__, __func__, __str); free(__str);}})

// log error.
void bf_err(char *msg, ...);

#endif // BF_BF_H_
