
#include <stdlib.h>
#include <stdbool.h>

#include "bf.h" // err
#include "tok.h"
#include "lex.h"
#include "lex_funcs.h"

#define IS_OP(c) (c == '>' || c == '<' || c == '+' || c == '-' || c == '.' || c == ',')
#define IS_LOOP(c) (c == '[' || c == ']')
#define IS_NEWLINE(c) (c == '\n')

bf_lex_data *bf_lex_data_init() {
	bf_lex_data *l = malloc(sizeof (bf_lex_data));
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

void bf_lex_data_free(bf_lex_data *l) {
	bf_stack_free(l->st);
	free(l);
}

// send tokens
#define bf_lex_tok_push(l, t) bf_stack_push(((bf_lex_data *) l->data)->st, (void *) t)

// lexes operator characters, but does not handle loops.
void *bf_lex_op(lex *l) {

	int gc; // gotten char

	// check if the current character is not an op or is an EOF.
	// in either case, the program should not be here, error.
	if ((gc = lex_peek(l)) < 0 || !IS_OP(gc)) {

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
				char *msg;
				if ((msg = lex_emit(l)) == NULL) {
					return NULL; // error
				}

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
						return NULL; // should never reach
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

				// return to the default state.
				return bf_lex_all;
			}
		}
	}
}

// lexes loop character
void *bf_lex_loop(lex *l) {
	char c;
	if ((c = lex_next(l)) && IS_LOOP(c)) {
		bf_lex_data *ld;
		if ((ld = (bf_lex_data *) l->data) == NULL) {

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
				char *msg;
				if ((msg = lex_emit(l)) == NULL) {
					return NULL; // error
				}

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

				return bf_lex_all;
			} else if (c == ']') {

				// acknowledge loop end
				ld->loop_count--;

				// note lexed content
				char *msg;
				if ((msg = lex_emit(l)) == NULL) {
					return NULL; // error
				}

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

				return bf_lex_all;
			} else {
				return NULL; // should never reach
			}
		}
	} else {

		// catch error
		if (c < -1) {

			// unrecoverable error, stop lexing.
			return NULL;
		} else {

			// note error
			#ifdef DEBUG
			err("lex_loop: expected loop, saw '%c'", c);
			#endif

			// unrecoverable error, stop lexing.
			return NULL;
		}
	}
}

// lexes newline character
void *bf_lex_newline(lex *l) {
	char c;
	if ((c = lex_next(l)) == '\n') {

		// note lexed content
		char *msg;
		if ((msg = lex_emit(l)) == NULL) {
			return NULL; // error
		}

		// allocate & send token
		bf_tok *t = bf_tok_init(BF_TOK_STE, msg);
		if (t == NULL) {

			// note error
			#ifdef DEBUG
			err("lex_newline: expected newline, saw '%c'", c);
			#endif

			// unrecoverable error, stop lexing
			return NULL;
		}
		bf_lex_tok_push(l, t);

		return bf_lex_all;
	} else {

		// note error
		#ifdef DEBUG
		err("lex_newline: expected newline, saw '%c'", c);
		#endif

		// unrecoverable error, stop lexing.
		return NULL;
	}
}

// default state function,
// lexes the initial state and returns subsequent states.
void *bf_lex_all(lex *l) {
	char c;
	while ((c = lex_peek(l)) >= 0) {
		// looks for a lexable character
		if (IS_OP(c)) {
			return bf_lex_op;
		} else if (IS_LOOP(c)) {
			return bf_lex_loop;
		} else if (IS_NEWLINE(c)) {
			return bf_lex_newline;
		} else {
			// ignores unknown characters
			if (lex_next(l) < 0) { return NULL; } // error
			if (lex_dump(l)) { return NULL; } // error
		}
	}
	return NULL;
}
