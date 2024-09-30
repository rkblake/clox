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
    case PLUS:
        return AST_ADD;
    case MINUS:
        return AST_SUBTRACT;
    case STAR:
        return AST_MULTIPLY;
    case SLASH:
        return AST_DIVIDE;
    case ENDOFFILE:
        return AST_EOF;
    default:
        fprintf(stderr, "unkown token in arithOp %d\n", tok);
    }
    return 0;
}
#endif

Token *current_token;
Node *current_node;

extern void print_token(int);

void next_token() {
    current_node = current_node->next;
    current_token = current_node->data;
}

void match_tok(TOKEN_TYPE t) {
    if (current_token->type != t) {
        fprintf(stderr, "unexpected token: ");
        print_token(current_token->type);
        fprintf(stderr, " expected: ");
        print_token(t);
        fprintf(stderr, "\n");
        exit(-1);
    }
    next_token();
}

static const int op_prec[] = {0, 10, 10, 20, 20, 0};

int op_precedence(int token_type) {
    int prec = op_prec[token_type];
    if (prec == 0) {
        fprintf(stderr, "syntax error in op_precedence\n");
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
        next_token();
        return n;
    case IDENTIFIER:
        id = find_glob(current_token->lexeme);
        if (id == -1) {
            fprintf(stderr, "unknown variable %s\n", current_token->lexeme);
            exit(1);
        }
    default:
        fprintf(stderr, "syntax error in primary expr\n");
        exit(1);
    }
}

AstNode *binexpr(int prev_tok_prec) {
    AstNode *left, *right;
    int token_type;

    left = primary();

    token_type = current_token->type;
    if (token_type == SEMICOLON)
        return left;
    int ast_type = arithOp(token_type);

    while (op_precedence(ast_type) > prev_tok_prec) {
        next_token();

        right = binexpr(op_prec[ast_type]);

        left = makeAstNode(arithOp(token_type), left, right, 0);

        token_type = current_token->type;
        if (token_type == SEMICOLON)
            return left;
        ast_type = arithOp(token_type);
    }

    return left;
}

extern int gen_ast(AstNode *);
extern void cg_printint(int);
extern void free_all_registers();

AstNode *statements() {
    struct AstNode *n;
    int reg;

    while (true) {
        // print_token(current_token); printf("\n");
        match_tok(PRINT);

        n = binexpr(0);
        reg = gen_ast(n);
        cg_printint(reg);
        free_all_registers();

        match_tok(SEMICOLON);
        if (current_token->type == ENDOFFILE)
            return NULL;
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

    if (node->left)
        leftval = interpret_ast(node->left);
    if (node->right)
        rightval = interpret_ast(node->right);

    switch (node->op) {
    case AST_ADD:
        return leftval + rightval;
    case AST_SUBTRACT:
        return leftval - rightval;
    case AST_MULTIPLY:
        return leftval * rightval;
    case AST_DIVIDE:
        return leftval / rightval;
    case AST_INTLIT:
        return node->int_value;
    default:
        fprintf(stderr, "unknown op %d\n", node->op);
        exit(1);
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
