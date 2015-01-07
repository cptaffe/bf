
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "bf.h"
#include "comp.h"

int main(int argc, char **argv) {
	FILE *file = stdin;

	// argument checking, if one is provided, it is the file name.
	if (argc > 2) {
		// print usage instructions on improper usage
		fprintf(stderr, "usage: bf <file>\n");
	} else if (argc == 2) {
		// argument provided, open file.
		file = fopen(argv[1], "r");
		if (file == NULL) {
			// open file failed.
			char *err = strerror(errno);
			fprintf(stderr, "file open failed: %s.", err);
			free(err);
			return 1;
		}
	}

	// init compiler
	bf_comp *c = bf_comp_init(file);
	if (!c) { return 1; }

	// run compiler
	int err;
	if ((err = bf_comp_run(c))) {
		err("bf_comp_run failed (%d)", err);
	}

	// cleanup compiler
	if (bf_comp_free(c)) { return 1; }

	return 0;
}
