#pragma once

#include "syntax.h"
#include "scanner.h"
#include "linkedlist.h"

Program* parse(LinkedList* tokens);
