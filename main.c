
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "bf.h"
#include "comp.h"

int main(int argc, char **argv) {
	FILE *file = stdin;

	// argument checking
	if (argc > 2) {
		// print usage instructions on improper usage
		fprintf(stderr, "usage: bf <file>\n");
	} else if (argc == 2) {
		file = fopen(argv[1], "r");
		if (file == NULL) {
			char *err = strerror(errno);
			fprintf(stderr, "file open failed: %s.", err);
			free(err);
			return 1;
		}
	}

	bf_comp *c = bf_comp_init(file);
	if (!c) { return 1; }

	if (bf_comp_run(c)) {
		err("bf_comp_run failed miserably");
	}
	if (bf_comp_free(c)) { return 1; }

	return 0;
}
