#include "scanner.h"

#include <string.h>

char *text;
size_t current = 0;
size_t line = 0;
size_t length;

char advance() { return text[current++]; }
char peek_next() { return text[current + 1]; }
bool is_at_end() { return !(current >= length); }
bool is_digit(char c) { return c >= '0' && c <= '9'; }
char peek() { return text[current]; }

bool match_next(char match) {
    if (text[current + 1] == '\0')
        return false;
    if (text[current + 1] != match)
        return false;
    current++;
    return true;
}

Token *string() {
    size_t start = current + 1;
    while (text[current + 1] != '"' && !(current >= length)) {
        if (text[current + 1] == '\n')
            line++;
        advance();
    }

    if (is_at_end()) {
        fprintf(stderr, "%ld: Enexpected end of string", line);
    }

    advance();

    char *string = (char *)malloc(current - start);
    Token *token = (Token *)malloc(sizeof(Token));
    token->line = line;
    token->type = STRING;
    token->literal = string;
    memcpy(string, text + current, current - start);
    return token;
}

Token *number() {
    size_t start;
    while (is_digit(peek()))
        advance();
    if (peek() == '.' && is_digit(peek_next())) {
        advance();
        while (is_digit(peek()))
            advance();
    }
    // atof
    char *string = (char *)malloc(current - start);
    Token *token = (Token *)malloc(sizeof(Token));
    token->line = line;
    token->type = NUMBER;
    memcpy(string, text + current, current - start);
    double *num = (double *)malloc(sizeof(double));
    *num = atof(string);
    token->literal = num;
    free(string);
    return token;
}

void scan_tokens(char *_text, size_t _length, size_t *num_tokens,
                 LinkedList *tokens) {

    text = _text;
    length = _length;

    while (!(current >= length)) {
        Token *token = (Token *)malloc(sizeof(Token));
        char c = text[current];
        switch (c) {
        case '(':
            token->type = LEFT_PAREN;
            break;
        case ')':
            token->type = RIGHT_PAREN;
            break;
        case '{':
            token->type = LEFT_BRACE;
            break;
        case '}':
            token->type = RIGHT_BRACE;
            break;
        case ',':
            token->type = COMMA;
            break;
        case '.':
            token->type = DOT;
            break;
        case '-':
            token->type = MINUS;
            break;
        case '+':
            token->type = PLUS;
            break;
        case ';':
            token->type = SEMICOLON;
            break;
        case '*':
            token->type = STAR;
            break;
        case '!':
            token->type = (match_next('=') ? BANG_EQUAL : BANG);
            break;
        case '=':
            token->type = (match_next('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            token->type = (match_next('=') ? LESS_EQUAL : LESS);
            break;
        case '/':
            if (match_next('/')) {
                while (text[current] != '\n' && !(current >= length))
                    current++;
            } else {
                token->type = SLASH;
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;
        case '\0':
            token->type = _EOF;
            break;
        case '"':
            token = string();
            break;
        default:
            if (is_digit(text[current])) {
                number();
            }
            fprintf(stderr, "Unexpected character: %c\n", c);
            break;
        }
        list_add(tokens, token);
        (*num_tokens)++;
        current++;
    }
}
