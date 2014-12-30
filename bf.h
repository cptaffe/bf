
#ifndef BF_BF_H_
#define BF_BF_H_

#include "stack.h" // define stack

// awesome macros which print the file and line number which these
// calls originated from.
#define fail(...) ({char *_____str; asprintf(&_____str, __VA_ARGS__); bf_fail("%s:%d: %s", __FILE__, __LINE__, _____str);})

#define err(...) ({char *_____str; asprintf(&_____str, __VA_ARGS__); bf_err("%s:%d: %s", __FILE__, __LINE__, _____str);})

// log error and gracefully fail.
void bf_fail(char *msg, ...);
void bf_err(char *msg, ...);

#endif // BF_BF_H_
