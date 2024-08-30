#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include "tokens.h"
#include <malloc.h>
#include <stdlib.h>

#if 0
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
	return 0;
}
#endif

// Token *current_token;
Node *current_token;

AstNode *parse_expr();
AstNode *parse_term();
AstNode *parse_factor();

AstNode *parse_expr() {
	AstNode *node = parse_term();

	while (current_token->type == PLUS || current_token->type == MINUS) {
		AstNode *new_node = (AstNode *)malloc(sizeof(AstNode));
		new_node->type = AST_OPERATOR;
		new_node->value.op.operator = current_token->type;
		new_node->value.op.left = node;

		next_token();
		new_node->value.op.right = parse_term();

		node = new_node;
	}

	return node;
}

AstNode* parse_term() {
	return 0;
}

void parse(LinkedList *tokens) {
	current_token = tokens;
}
