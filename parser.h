#pragma once

#include "linkedlist.h"

typedef struct AstNode {
	int op;
	struct AstNode *left;
	struct AstNode *right;
	union {
		int int_value;
		float float_value;
		int id;
	};
} AstNode;

struct symbol_table {
	char *name;
};

AstNode *parse(LinkedList *tokens);
