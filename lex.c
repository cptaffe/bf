// lexer

#include <stdio.h>
#include <stdlib.h>
#include <errno.h> // errorno
#include <string.h> // memset
#include <stdint.h> // integer types
#include <unistd.h> // read/write

extern int errno;

#include "bf.h"
#include "lex.h"

// allocate and initialize the lexer struct
// returns a pointer to an allocated lex if successful,
// else returns NULL.
lex *lex_init(size_t sz, FILE *file) {
	lex *l;

	// allocate lex
	if ((l = malloc(sizeof (lex))) == NULL) {
		return NULL;
	} else {
		// set size of buffer
		l->size = sz;

		// set file to read from
		l->file = file;

		// zero lens
		l->len = 0;
		l->llen = 0;

		// allocate sz bytes for buf
		if ((l->lexed = malloc(sz)) == NULL) {
			free(l); // clean up.
			return NULL;
		} else {
			return l;
		}
	}
}

// frees the lex struct
void lex_free(lex *l) {
	free(l->lexed);
	free(l);
}

// dump currently lexed characters while
// preserving back'd characters.
void lex_dump(lex *l) {
	// if there are back'd characters that need preserving.
	if ((l->llen -= l->len) > 0) {
		// copy back preserved (back'd) characters,
		// using memmove because it allows for overlapping buffers.
		if ((l->lexed = memmove(l->lexed, &l->lexed[l->len], l->llen)) == NULL) {
			// unrecoverable error
			fail("movement of lex buffer failed: %s.", strerror(errno));
		}
	}

	// reset length
	l->len = 0;
}

// returns an allocated string with a copy of the
// currently lexed characters, also dumps.
char *lex_emit(lex *l) {
	char *copy;

	// allocate an extra byte for null termination
	if ((copy = malloc(l->len + 1)) == NULL) {
		return NULL; // error
	} else {
		// using memcpy because memory does not overlap.
		if ((copy = memcpy(copy, l->lexed, l->len)) == NULL) {
			return NULL; // error
		} else {
			copy[l->len] = '\0'; // null terminate
			lex_dump(l);
			return copy;
		}
	}
}

// gets the next character, either a back'd
// character or a character from the file.
char lex_next(lex *l) {

	// reallocate 'lexed' buffer when it runs out of space
	// simply doubles the size.
	if (l->len >= l->size) {

		// double buffer size
		l->size *= 2;
		if ((l->lexed = realloc(l->lexed, l->size)) == NULL) {
			// reallocation of lex buffer failed
			fail("reallocation of lex buffer failed: %s.", strerror(errno));
		}
	}

	// if there are back'd characters
	if (l->llen > l->len) {
		return l->lexed[l->len++];
	} else {
		// read character from file
		char c = getc(l->file);

		// don't save EOF, not a real character.
		if (c != EOF) {
			l->lexed[l->len++] = c;
			l->llen++; // increase lexed length
		}

		return c;
	}
}

// backup a character, leaving llen at its current position.
void lex_back(lex *l) {
	if (l->len > 0) {
		l->len--;
	}
}

// get the next character (either back'd or from file),
// and backup so it can be next'd again.
char lex_peek(lex *l) {
	char c = lex_next(l);
	lex_back(l);
	return c;
}

// function signature for state functions
void *lex_all(lex *l);
void *lex_op(lex *l);

// lexes operator characters, but does not handle loops.
void *lex_op(lex *l) {

	// check if the current character is an op.
	char gc; // gotten char
	if ((gc = lex_peek(l)) == EOF && !(gc == '>' || gc == '<' || gc == '+' || gc == '-')) {
		// unrecoverable error, stop lexing.
		return NULL;
	} else {
		// lex the op type as a string of consecutive ops of the same type.
		char c;
		while ((c = lex_next(l)) != EOF && c == gc) {}
		lex_back(l);

		#ifdef DEBUG
		// get the current lex'd string from emit.
		// scoped to prevent side effects.
		{
			char *msg = lex_emit(l);
			printf("lexed: %s\n", msg);
			free(msg);
		}
		#endif

		// return to the default state.
		return lex_all;
	}
}

void *lex_all(lex *l) {
	char c;
	while ((c = lex_peek(l)) != EOF) {
		// looks for a lexable character
		if (c == '>' || c == '<' || c == '+' || c == '-') {
			return lex_op;
		} else {
			// ignores unknown characters
			lex_next(l); lex_dump(l); // eat unknown
		}
	}
	return NULL;
}

// finite state machine!
void lex_state(lex *l) {

	// start with lex_all, the default.
	void *func = lex_all;

	// loop until a state function returns NULL.
	while (func != NULL) {
		// type conversion for simplicity.
		func = ((void *(*)(lex *)) func)(l);
	}
}
