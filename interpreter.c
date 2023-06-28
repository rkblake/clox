#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "linkedlist.h"
#include "scanner.h"

bool had_error = false;

void report(size_t line, char *where, char *message) {
    fprintf(stderr, "ERROR [path:%ld] %s\n", line, where);
    fprintf(stderr, "%s\n\n", message);
    had_error = true;
}

void error(size_t line, char *message) { report(line, "", message); }

void run(char *text, size_t size) {
    size_t num_tokens = 0;
    LinkedList *tokens = create_list();
    scan_tokens(text, size, &num_tokens, tokens);
    // printf("%ld\n", num_tokens);

    Node *node = tokens->head;
    while (node->next != NULL) {
        // printf("%ld\n", ((Token *)(node->data))->type);
        node = node->next;
    }
}

int run_file(char *path) {
    // open file and read contents
    FILE *fp;
    fp = fopen(path, "r");

    size_t size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *text;
    text = (char *)malloc(size * sizeof(char));
    // fgets(text, size, fp);
    int i = 0;
    for (; i < size; i++) { text[i] = fgetc(fp); }

    if (text) { run(text, size); }

    free(text);
    fclose(fp);

    if (had_error) { return 65; }

    return 0;
}

void run_prompt() {
    char buffer[255];
    while (1) {
        printf("> ");
        fgets(buffer, 255, stdin);
        run(buffer, 255);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        printf("Usage: Clox [script]\n");
        return 64;
    } else if (argc == 2) {
        return run_file(argv[1]);
    } else {
        run_prompt();
    }

    return 0;
}
