#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include "tokens.h"
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
	// switch (tok) {
	// 	case PLUS: return AST_ADD;
	// 	case MINUS: return AST_SUBTRACT;
	// 	case STAR: return AST_MULTIPLY;
	// 	case SLASH: return AST_DIVIDE;
	// 	case ENDOFFILE: return AST_EOF;
	// 	default: fprintf(stderr, "unkown token in arithOp %d\n", tok);
	// }
	if (tok > ENDOFFILE && tok < NUMBER) return tok;
	fprintf(stderr, "syntax error, token ");
	print_token(tok);
	return 0;
}

Token *current_token;
Node *current_node;

extern void print_token(int);

void next_token() {
	current_node = current_node->next;
	current_token = current_node->data;
}

bool peek_tok(TOKEN_TYPE t) {
	if (current_token->type != t) {
		fprintf(stderr, "Line %d: unexpected token: ", current_token->line);
		print_token(current_token->type);
		fprintf(stderr, " expected: ");
		print_token(t);
		return false;
	}
	return true;
}

void match_tok(TOKEN_TYPE t) {
	bool b = peek_tok(t);
	if (!b) exit(-1);
	next_token();
}

static const int op_prec[] = {0, 10, 10, 20, 20, 30, 30, 40, 40, 40, 40};

int op_precedence(int token_type) {
	int prec = op_prec[token_type];
	if (prec == 0) {
		fprintf(stderr, "Line %d: syntax error in op_precedence\n",
				current_token->line);
		exit(1);
	}
	return prec;
}

extern int find_glob(char *);

AstNode *primary() {
	AstNode *n;
	int id;
	switch (current_token->type) {
		case NUMBER:
			n = makeAstLeaf(AST_INTLIT, *(int *)(current_token->literal));
			break;
		case IDENTIFIER:
			id = find_glob(current_token->lexeme);
			if (id == -1) {
				fprintf(stderr, "unknown variable %s\n", current_token->lexeme);
				exit(1);
			}
			n = makeAstLeaf(AST_IDENT, id);
			break;
		default: fprintf(stderr, "syntax error in primary expr\n"); exit(1);
	}

	next_token();
	return n;
}

AstNode *binexpr(int prev_tok_prec) {
	AstNode *left, *right;
	int token_type;

	left = primary();

	token_type = current_token->type;
	if (token_type == SEMICOLON) return left;
	int ast_type = arithOp(token_type);

	while (op_precedence(ast_type) > prev_tok_prec) {
		next_token();

		right = binexpr(op_prec[ast_type]);

		left = makeAstNode(arithOp(token_type), left, right, 0);

		token_type = current_token->type;
		if (token_type == SEMICOLON) return left;
		ast_type = arithOp(token_type);
	}

	return left;
}

extern int gen_ast(AstNode *, int);
extern void cg_printint(int);
extern void free_all_registers();

void print_statement() {
	AstNode *n;
	int reg;
	match_tok(PRINT);
	n = binexpr(0);
	reg = gen_ast(n, -1);
	cg_printint(reg);
	free_all_registers();

	match_tok(SEMICOLON);
}

extern int add_glob(char *);
extern void cg_globsym(char *);

void var_declaration() {
	match_tok(INT);
	peek_tok(IDENTIFIER);
	add_glob(current_token->lexeme);
	cg_globsym(current_token->lexeme);
	next_token();

	match_tok(SEMICOLON);
}

void assignment_statement() {
	AstNode *left, *right, *tree;
	int id;

	peek_tok(IDENTIFIER);

	if ((id = find_glob(current_token->lexeme)) == -1) {
		fprintf(stderr, "Line %d: undeclared variable %s\n",
				current_token->line, current_token->lexeme);
		exit(-1);
	}
	right = makeAstLeaf(AST_LVIDENT, id);

	next_token();
	match_tok(EQUAL);

	left = binexpr(0);

	tree = makeAstNode(AST_ASSIGNMENT, left, right, 0);

	gen_ast(tree, -1);
	free_all_registers();

	match_tok(SEMICOLON);
}

AstNode *statements() {
	// struct AstNode *n;
	// int reg;

	while (true) {
		switch (current_token->type) {
			case PRINT: print_statement(); break;
			case INT: var_declaration(); break;
			case IDENTIFIER: assignment_statement(); break;
			case ENDOFFILE: return NULL;
			default:
				fprintf(stderr, "Line %d: syntax error\n", current_token->line);
				exit(-1);
		}

		// if (current_token->type == ENDOFFILE)
		//     return NULL;
	}
	// return n;
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

	// AstNode *root = binexpr(0);
	AstNode *root = statements();
	// printf("Print AST:\n");
	// print_tree(root, 0);
	// printf("eval: %d\n", interpret_ast(root));

	return root;
}
