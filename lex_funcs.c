
#include <stdio.h>
#include <stdlib.h>

#include "bf.h" // err
#include "lex.h"
#include "lex_funcs.h"

// lexes operator characters, but does not handle loops.
void *lex_op(lex *l) {

	int gc; // gotten char

	// check if the current character is not an op or is an EOF.
	// in either case, the program should not be here, error.
	if ((gc = lex_peek(l)) < 0 || gc == EOF || !(gc == '>' || gc == '<' || gc == '+' || gc == '-')) {

		if (gc < 0) {
			// unrecoverable error, stop lexing.
			return NULL;
		} else {
			// note the error
			#ifdef DEBUG
			err("unknown character in lex_op: '%c'.", gc);
			#endif

			// unrecoverable error, stop lexing.
			return NULL;
		}
	} else {

		// lex the op type as a string of consecutive ops of the same type.
		int c;
		while ((c = lex_next(l)) >= 0 && c != EOF && c == gc) {}

		if (c < 0) {

			// unrecoverable error, stop lexing.
			return NULL;
		} else {
			if (lex_back(l)){

				// unrecoverable error, stop lexing.
				return NULL;
			} else {

				// note lexed content
				#ifdef DEBUG
				char *msg;
				if ((msg = lex_emit(l)) == NULL) {
					return NULL; // error
				} else {
					printf("lexed: %s\n", msg);
					free(msg);
				}
				#endif

				// return to the default state.
				return lex_all;
			}
		}
	}
}

// default state function,
// lexes the initial state and returns subsequent states.
void *lex_all(lex *l) {
	char c;
	while ((c = lex_peek(l)) >= 0 && c != EOF) {
		// looks for a lexable character
		if (c == '>' || c == '<' || c == '+' || c == '-') {
			return lex_op;
		} else {
			// ignores unknown characters
			if (lex_next(l) < 0) { return NULL; } // error
			if (lex_dump(l)) { return NULL; } // error
		}
	}
	return NULL;
}
