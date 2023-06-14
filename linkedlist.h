#pragma once

#include <stdlib.h>

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct LinkedList {
    Node *head;
} LinkedList;

LinkedList *create_list();
void list_add(LinkedList *list, void *data);
