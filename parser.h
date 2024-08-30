#pragma once

#include "ast.h"
#include "linkedlist.h"
#include "tokens.h"
#include "scanner.h"

// enum { ADD, SUBTRACT, MULTIPLY, DIVIDE, INT_LIT };

typedef struct AstNode {
	AST_TYPE type;
	union {
		struct {
			size_t operator;
			struct AstNode *left;
			struct AstNode *right;
		} op;
		int number;
		char *indentifier;
		struct {
			struct AstNode* left;
			struct AstNode* right;
		} assignment;
	} value;
} AstNode;

void parse(LinkedList *tokens);
