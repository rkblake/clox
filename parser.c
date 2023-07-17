#include "parser.h"

Program* parse(LinkedList* tokens) {
    Program* program = (Program*)malloc(sizeof(Program));
    program->type = PROGRAM;

    return program;
}
