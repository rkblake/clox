#ifndef SCANNER_H
#define SCANNER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include "linkedlist.h"

enum TOKEN_TYPE {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    IDENTIFIER,
    STRING,
    NUMBER,
    AND,
    OR,
    CLASS,
    IF,
    ELSE,
    TRUE,
    FALSE,
    FUN,
    FOR,
    WHILE,
    NIL,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    VAR,
    _EOF
};

typedef struct Token {
    size_t type;
    char *lexeme;
    void *literal;
    int line;
} Token;

void scan_tokens(char *text, size_t length, size_t *num_tokens,
                 LinkedList *tokens);

#endif
