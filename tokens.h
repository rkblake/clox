#pragma once

// clang-format off
enum TOKEN_TYPE {
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, STAR, SLASH,
    MINUS, PLUS,
    SEMICOLON,
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    IDENTIFIER, STRING, NUMBER,
    AND, OR,
    CLASS,
    IF, ELSE,
    TRUE, FALSE,
    FUN,
    FOR, WHILE,
    NIL,
    PRINT,
    RETURN,
    SUPER, THIS,
    VAR,
    ENDOFFILE
};
// clang-format on
