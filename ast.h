#pragma once

typedef enum {
	AST_EOF,
	AST_ADD,
	AST_SUBTRACT,
	AST_MULTIPLY,
	AST_DIVIDE,
	AST_INTLIT,
	// AST_NUMBER,
	// AST_IDENTIFIER,
	// AST_OPERATOR,
	// AST_ASSIGNMENT,
} AST_TYPE;
