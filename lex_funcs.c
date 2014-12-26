
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bf.h" // err
#include "tok.h"
#include "lex.h"
#include "lex_funcs.h"

lex_data *bf_lex_data_init() {
	lex_data *l = malloc(sizeof (lex_data));
	if (l == NULL) {
		return NULL;
	} else {

		// init loop count
		l->loop_count = 0;
		l->st = bf_stack_init();
		if (l->st == NULL) { return NULL; }

		return l;
	}
}

void bf_lex_data_free(lex_data *l) {
	bf_stack_free(l->st);
	free(l);
}

// send tokens
bool bf_lex_tok_push(lex *l, bf_tok *t) {
	bf_stack_push(((lex_data *) l->data)->st, (void *) t);
	return true;
}

// lexes operator characters, but does not handle loops.
void *bf_lex_op(lex *l) {

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
					bf_tok_type_t type;
					switch (gc) {
						case '-':
							type = BF_TOK_MINUS;
							break;
						case '+':
							type = BF_TOK_PLUS;
							break;
						case '>':
							type = BF_TOK_GT;
							break;
						case '<':
							type = BF_TOK_LT;
							break;
						case '.':
							type = BF_TOK_DOT;
							break;
						case ',':
							type = BF_TOK_COMMA;
							break;
						default:
							fail("unknown token type, should never reach.");
					}

					// allocate & send token
					bf_tok *t = bf_tok_init(type, msg);
					if (t == NULL) {

						// note error
						#ifdef DEBUG
						err("lex_loop: expected loop, saw '%c'", c);
						#endif

						// unrecoverable error, stop lexing
						return NULL;
					}
					bf_lex_tok_push(l, t);
				}
				#endif

				// return to the default state.
				return bf_lex_all;
			}
		}
	}
}

// lexes loop character
void *bf_lex_loop(lex *l) {
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
					bf_tok *t = bf_tok_init(BF_TOK_RB, msg);
					if (t == NULL) {

						// note error
						#ifdef DEBUG
						err("lex_loop: expected loop, saw '%c'", c);
						#endif

						// unrecoverable error, stop lexing
						return NULL;
					}
					bf_lex_tok_push(l, t);
				}
				#endif

				return bf_lex_all;
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
					bf_tok *t = bf_tok_init(BF_TOK_LB, msg);
					if (t == NULL) {

						// note error
						#ifdef DEBUG
						err("lex_loop: expected loop, saw '%c'", c);
						#endif

						// unrecoverable error, stop lexing
						return NULL;
					}
					bf_lex_tok_push(l, t);
				}
				#endif

				return bf_lex_all;
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
void *bf_lex_all(lex *l) {
	char c;
	while ((c = lex_peek(l)) >= 0) {
		// looks for a lexable character
		if (c == '>' || c == '<' || c == '+' || c == '-' || c == '.' || c == ',') {
			return bf_lex_op;
		} else if (c == '[' || c == ']') {
			return bf_lex_loop;
		} else {
			// ignores unknown characters
			if (lex_next(l) < 0) { return NULL; } // error
			if (lex_dump(l)) { return NULL; } // error
		}
	}
	return NULL;
}
