#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include "tokens.h"
#include <malloc.h>
#include <stdlib.h>

#if 1
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
		case PLUS: return AST_ADD;
		case MINUS: return AST_SUBTRACT;
		case STAR: return AST_MULTIPLY;
		case SLASH: return AST_DIVIDE;
		case ENDOFFILE: return AST_EOF;
		default: fprintf(stderr, "unkown token in arithOp %d\n", tok);
	}
	return 0;
}
#endif

Token *current_token;
Node *current_node;

void next_token() {
	current_node = current_node->next;
	current_token = current_node->data;
}

static const int op_prec[] = {0, 10, 10, 20, 20, 0};

int op_precedence(int token_type) {
	int prec = op_prec[token_type];
	if (prec == 0) {
		fprintf(stderr, "syntax error\n");
		exit(1);
	}
	return prec;
}

AstNode *primary() {
	AstNode *n;
	switch (current_token->type) {
		case NUMBER:
			n = makeAstLeaf(AST_INTLIT, *(int *)(current_token->literal));
			next_token();
			return n;
		default: fprintf(stderr, "syntax error\n"); exit(1);
	}
}

AstNode *binexpr(int prev_tok_prec) {
	AstNode *left, *right;
	int token_type;

	left = primary();

	token_type = current_token->type;
	int ast_type = arithOp(token_type);
	if (token_type == ENDOFFILE) return left;

	while (op_precedence(ast_type) > prev_tok_prec) {
		next_token();

		right = binexpr(op_prec[ast_type]);

		left = makeAstNode(arithOp(token_type), left, right, 0);

		token_type = current_token->type;
		ast_type = arithOp(token_type);
		if (ast_type == AST_EOF) return left;
	}

	return left;
}

void print_tree(AstNode *node, int depth) {
	for (int i = 0; i < depth; i++) {
		printf("  ");
	}
	printf("%d\n", node->op);

	if (node->left) {
		print_tree(node->left, depth + 1);
	}
	if (node->right) {
		print_tree(node->right, depth + 1);
	}
}

int interpret_ast(AstNode *node) {
	int leftval, rightval;

	if (node->left) leftval = interpret_ast(node->left);
	if (node->right) rightval = interpret_ast(node->right);

	switch (node->op) {
		case AST_ADD: return leftval + rightval;
		case AST_SUBTRACT: return leftval - rightval;
		case AST_MULTIPLY: return leftval * rightval;
		case AST_DIVIDE: return leftval / rightval;
		case AST_INTLIT: return node->int_value;
		default: fprintf(stderr, "unknown op %d\n", node->op); exit(1);
	}
}

AstNode *parse(LinkedList *tokens) {
	current_node = tokens->head;
	current_token = current_node->data;

	AstNode *root = binexpr(0);
	printf("Print AST:\n");
	print_tree(root, 0);
	printf("eval: %d\n", interpret_ast(root));

	return root;
}
