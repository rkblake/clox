#pragma once

#include "linkedlist.h"

enum { ADD, SUBTRACT, MULTIPLY, DIVIDE, INT_LIT };

typedef struct AstNode {
	int op;
	struct AstNode *left;
	struct AstNode *right;
	int int_value;
} AstNode;

void parse(LinkedList *tokens);
