#pragma once

#include "linkedlist.h"
#include "scanner.h"
#include "syntax.h"

Program *parse(LinkedList *tokens);
