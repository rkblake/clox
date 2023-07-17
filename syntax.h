#pragma once

#include "scanner.h"

typedef enum {
    PROGRAM,
    BINARYEXPR,
    UNARYEXPR,
    STATEMENT,
    NUMBERLITERAL,
    STRINGLITERAL,
} SyntaxType;

typedef struct {

} Expr;

typedef union {
    int integer;
    float floating_point;
    char *string;
} Object;

typedef struct {
    Expr left;
    // Token operator;
    Expr right;
} Binary;

typedef struct {
    // Token operator;
    Expr expr;
} Unary;

typedef struct {
    Object value;
} Literal;

typedef struct {
    SyntaxType type;
    union {
        Binary bin;
        Unary una;
        Literal literal;
        Object object;
    };
} Program;
