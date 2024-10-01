#pragma once

// clang-format off
typedef enum {
    ENDOFFILE,
    PLUS, MINUS,
    STAR, SLASH,
    EQUAL_EQUAL, BANG_EQUAL,
    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    NUMBER, SEMICOLON, EQUAL, IDENTIFIER,
    COLON, COLON_EQUAL, COMMA, DOT, 
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    BANG,
    STRING,
    AND, OR,
    CLASS,
    IF, ELSE,
    TRUE, FALSE,
    FUNC,
    FOR, WHILE,
    NIL,
    PRINT,
    RETURN,
    SUPER, THIS,
    VAR,
    INT, FLOAT, ARRAY,
} TOKEN_TYPE;
// clang-format on
