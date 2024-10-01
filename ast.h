#pragma once

// clang-format off
typedef enum {
    AST_ADD=1, AST_SUBTRACT, AST_MULTIPLY, AST_DIVIDE,
    AST_EQ, AST_NE, AST_LT, AST_GT, AST_LTE, AST_GTE,
    AST_INTLIT,
    AST_IDENT,
    AST_LVIDENT,
    AST_ASSIGNMENT,
    AST_GLUE,
    AST_IF,
} AST_TYPE;
// clang-format off
