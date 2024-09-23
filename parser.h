#pragma once

// #include "ast.h"
#include "linkedlist.h"
// #include "tokens.h"
// #include "scanner.h"

// enum { ADD, SUBTRACT, MULTIPLY, DIVIDE, INT_LIT };
#if 0
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
#else

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

#endif
AstNode *parse(LinkedList *tokens);
