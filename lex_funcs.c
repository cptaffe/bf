
#include <stdio.h>
#include <stdlib.h>

#include "bf.h" // err
#include "tok.h"
#include "lex.h"
#include "lex_funcs.h"

lex_data *lex_data_init() {
	lex_data *l = malloc(sizeof (lex_data));
	if (l == NULL) {
		return NULL;
	} else {

		// init loop count
		l->loop_count = 0;

		return l;
	}
}

void lex_data_free(lex_data *l) {
	free(l);
}

// send tokens
void lex_tok_push(lex *l, tok *t) {

	// note pushing of token
	printf("lex'd & push'd: '%s'.\n", t->msg);

	// deallocate
	free(t->msg);
	free(t);
}

// lexes operator characters, but does not handle loops.
void *lex_op(lex *l) {

	int gc; // gotten char

	// check if the current character is not an op or is an EOF.
	// in either case, the program should not be here, error.
	if ((gc = lex_peek(l)) < 0 || !(gc == '>' || gc == '<' || gc == '+' || gc == '-' || gc == '.' || gc == ',')) {

		if (gc < -1) {
			// unrecoverable error, stop lexing.
			return NULL;
		} else {
			// note the error
			#ifdef DEBUG
			err("lex_op: expected operator, saw '%c'.", gc);
			#endif

			// unrecoverable error, stop lexing.
			return NULL;
		}
	} else {

		// lex the op type as a string of consecutive ops of the same type.
		char c;
		while ((c = lex_next(l)) >= 0 && c == gc) {}

		if (c < -1) {

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
					// get token type from gotten char.
					int type;
					switch (gc) {
						case '-':
							type = TOK_MINUS;
							break;
						case '+':
							type = TOK_PLUS;
							break;
						case '>':
							type = TOK_GT;
							break;
						case '<':
							type = TOK_LT;
							break;
						case '.':
							type = TOK_DOT;
							break;
						case ',':
							type = TOK_COMMA;
							break;
						default:
							fail("unknown token type, should never reach.");
					}

					// allocate & send token
					tok *t = tok_init(type, msg);
					lex_tok_push(l, t);
				}
				#endif

				// return to the default state.
				return lex_all;
			}
		}
	}
}

// lexes loop character
void *lex_loop(lex *l) {
	char c;
	if ((c = lex_next(l)) >= 0) {
		lex_data *ld;
		if ((ld = (lex_data *) l->data) == NULL) {

			// note error
			#ifdef DEBUG
			err("lex_loop: undefined lex->data");
			#endif

			// unrecoverable error, stop lexing
			return NULL;
		} else {
			if (c == '[') {

				// acknowledge loop begin
				ld->loop_count++;

				// note lexed content
				#ifdef DEBUG
				char *msg;
				if ((msg = lex_emit(l)) == NULL) {
					return NULL; // error
				} else {
					// allocate & send token
					tok *t = tok_init(TOK_RB, msg);
					lex_tok_push(l, t);
				}
				#endif

				return lex_all;
			} else if (c == ']') {

				// acknowledge loop end
				ld->loop_count--;

				// note lexed content
				#ifdef DEBUG
				char *msg;
				if ((msg = lex_emit(l)) == NULL) {
					return NULL; // error
				} else {
					// allocate & send token
					tok *t = tok_init(TOK_LB, msg);
					lex_tok_push(l, t);
				}
				#endif

				return lex_all;
			} else {

				// note error
				#ifdef DEBUG
				err("lex_loop: expected loop, saw '%c'", c);
				#endif

				// unrecoverable error, stop lexing.
				return NULL;
			}
		}
	} else {

		// catch error
		if (c < -1) {

			// unrecoverable error, stop lexing.
			return NULL;
		} else {

			// called with EOF
			#ifdef DEBUG
			err("lex_loop: unexpected character EOF");
			#endif

			// unrecoverable error, stop lexing.
			return NULL;
		}
	}
}

// default state function,
// lexes the initial state and returns subsequent states.
void *lex_all(lex *l) {
	char c;
	while ((c = lex_peek(l)) >= 0) {
		// looks for a lexable character
		if (c == '>' || c == '<' || c == '+' || c == '-' || c == '.' || c == ',') {
			return lex_op;
		} else if (c == '[' || c == ']') {
			return lex_loop;
		} else {
			// ignores unknown characters
			if (lex_next(l) < 0) { return NULL; } // error
			if (lex_dump(l)) { return NULL; } // error
		}
	}
	return NULL;
}
