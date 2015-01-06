
#ifndef BF_BF_H_
#define BF_BF_H_

#include "stdarg.h"

#include "stack.h" // define stack

// awesome macros which print the file and line number which these
// calls originated from.
#define err(...) bf_err(__VA_ARGS__)
#define bf_err(...) bf_err_stat(__FILE__, __func__, __LINE__, __VA_ARGS__)

// log error.
void bf_err_stat(const char *file, const char *func, const int line, char *fmt, ...);

#endif // BF_BF_H_
