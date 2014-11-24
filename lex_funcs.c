#include "lex_funcs.h"

// lexes operator characters, but does not handle loops.
void *lex_op(lex *l) {

	char gc; // gotten char

	// check if the current character is not an op or is an EOF.
	// in either case, the program should not be here, error.
	if ((gc = lex_peek(l)) == EOF || !(gc == '>' || gc == '<' || gc == '+' || gc == '-')) {

		// note the error
		#ifdef DEBUG
		err("unknown character in lex_op: '%c'.", gc);
		#endif

		// unrecoverable error, stop lexing.
		return NULL;
	} else {
		// lex the op type as a string of consecutive ops of the same type.
		char c;
		while ((c = lex_next(l)) != EOF && c == gc) {}
		if (!lex_back(l){
			// unrecoverable error, stop lexing.
			return NULL;
		} else {
			#ifdef DEBUG
				// get the current lex'd string from emit.
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

// default state function,
// lexes the initial state and returns subsequent states.
void *lex_all(lex *l) {
	char c;
	while ((c = lex_peek(l)) != EOF) {
		// looks for a lexable character
		if (c == '>' || c == '<' || c == '+' || c == '-') {
			return lex_op;
		} else {
			// ignores unknown characters
			lex_next(l);
			if (lex_dump(l)) { return NULL; } // error
		}
	}
	return NULL;
}
