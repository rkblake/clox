#include "linkedlist.h"

LinkedList *create_list() {
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    list->head = NULL;
    return list;
}

void list_add(LinkedList *list, void *data) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = data;

    if (list->head == NULL) {
        list->head = node;
    } else {
        Node *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
}
