#pragma once

#include "ast.h"
#include "linkedlist.h"

// enum { ADD, SUBTRACT, MULTIPLY, DIVIDE, INT_LIT };

typedef struct AstNode {
	AST_TYPE type;
	union {
		struct {
			char operator;
			struct AstNode *left;
			struct AstNode *right;
		} op;
		int number;
		char *indentifier;
	} value;
} AstNode;

void parse(LinkedList *tokens);
