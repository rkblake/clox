#include "parser.h"
#include "scanner.h"
#include <malloc.h>
#include <stdlib.h>

AstNode *makeAstNode(int op, AstNode *left, AstNode *right, int int_value) {
	AstNode *n = (AstNode *)malloc(sizeof(AstNode));
	if (n == NULL) {
		fprintf(stderr, "Unable to malloc in make_ast_node\n");
		exit(-1);
	}
	n->op = op;
	n->left = left;
	n->right = right;
	n->int_value = int_value;

	return n;
}

AstNode *makeAstLeaf(int op, int int_value) {
	return makeAstNode(op, NULL, NULL, int_value);
}

AstNode *makeAstUnary(int op, AstNode *left, int int_value) {
	return makeAstNode(op, left, NULL, int_value);
}

int arithOp(int tok) {
	switch (tok) {
	case PLUS:
		return ADD;
		break;
	case MINUS:
		return SUBTRACT;
		break;
	case STAR:
		return MULTIPLY;
		break;
	case SLASH:
		return DIVIDE;
		break;
	default:
		fprintf(stderr, "unkown token in arithOp\n");
	}
}

void parse(LinkedList *tokens) {}
