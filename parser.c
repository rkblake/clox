#include "parser.h"

#include "stack.h"

#include <stdbool.h>

Node *parser_current;
Program *program;
LinkedList *tokens;
Stack stack;

Token *previous() {
    Node *node = tokens->head;
    while (node->next != NULL) {
        if (node->next == parser_current) return node->data;
    }
    return NULL;
}
Token *parser_peek() { return parser_current->data; }
bool parser_is_at_end() { return parser_peek()->type == ENDOFFILE; }

bool check(enum TOKEN_TYPE type) {
    if (parser_is_at_end()) return false;
    return parser_peek()->type == type;
}

Token *parser_advance() {
    if (!parser_is_at_end()) parser_current = parser_current->next;
    return previous();
}

bool match(enum TOKEN_TYPE *types, int num_types) {
    for (int i = 0; i < num_types; i++) {
        if (check(types[i])) {
            parser_advance();
            return true;
        }
    }
    return false;
}

Expr *equality() {
    // Expr expr = comparison();
    enum TOKEN_TYPE tok[] = {BANG_EQUAL, EQUAL_EQUAL};
    while (match(tok, 2)) {
        //
        Token operator;
    }
}

Program *parse(LinkedList *_tokens) {
    tokens = _tokens;
    program = (Program *)malloc(sizeof(Program));
    program->type = PROGRAM;

    parser_current = tokens->head;
    while (parser_current->next != NULL) {
        //
        parser_current = parser_current->next;
    }

    return program;
}
