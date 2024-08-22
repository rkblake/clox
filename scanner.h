#ifndef SCANNER_H
#define SCANNER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include "linkedlist.h"

typedef struct Token {
	size_t type;
	char *lexeme;
	void *literal;
	int line;
} Token;

void scan_tokens(char *text, size_t length, size_t *num_tokens,
				 LinkedList *tokens);

void print_token(Token *);

#endif
