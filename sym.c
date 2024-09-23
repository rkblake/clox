#include "parser.h"
#include <string.h>

#define NUM_SYMBOLS 1024
struct symbol_table sym[NUM_SYMBOLS];
static int Globs = 0;

int find_glob(char *s) {
	for (int i = 0; i < Globs; i++) {
		if (sym[i].name == s) return i;
	}
	return -1;
}

static int new_glob() { return Globs++; }

int add_glob(char *name) {
	int y;
	if ((y = find_glob(name)) != -1) return y;

	y = new_glob();

	sym[new_glob()].name = strdup(name);

	return y;
}
