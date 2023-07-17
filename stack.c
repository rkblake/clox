#include "stack.h"

#include <string.h>

void stack_push(Stack *stack, void *val) {
    if (stack->size == 0) { stack->stack = (void *)malloc(sizeof(void *) * 2); }
    if (stack->top == stack->size) {
        void *tmp = (void *)malloc(sizeof(void *) * stack->size * 2);
        memcpy(tmp, stack->stack, stack->size);
        stack->size *= 2;
        free(stack->stack);
        stack->stack = tmp;
    }
    stack->stack[stack->top++] = val;
}

void *stack_pop(Stack *stack) {
    if (stack->size == 0) return NULL;
    void *ret = stack->stack[stack->top--];
    if (stack->top < stack->size / 4) {
        void *tmp = (void *)malloc(sizeof(void *) * stack->size / 2);
        memcpy(tmp, stack->stack, stack->top);
        stack->size /= 2;
        free(stack->stack);
        stack->stack = tmp;
    }

    return ret;
}
