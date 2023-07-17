#pragma once

#include <stdlib.h>

typedef struct {
    void **stack;
    size_t top;
    size_t size;
} Stack;

void stack_push(Stack *stack, void *val);
void *stack_pop(Stack *stack);
