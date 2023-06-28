#include "scanner.h"

#include <ctype.h>
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
bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
bool is_alphanumeric(char c) { return is_digit(c) || is_alpha(c); }

int strcicmp(char const *a, char const *b) {
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a || !*b) return d;
    }
}

bool match_next(char match) {
    if (text[current + 1] == '\0') return false;
    if (text[current + 1] != match) return false;
    current++;
    return true;
}

Token *string() {
    size_t start = current + 1;
    while (text[current + 1] != '"' && !(current >= length)) {
        if (text[current + 1] == '\n') line++;
        advance();
    }

    if (is_at_end()) { fprintf(stderr, "%ld: Unexpected end of string", line); }

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
    size_t start = current;
    while (is_digit(peek())) advance();
    if (peek() == '.' && is_digit(peek_next())) {
        advance();
        while (is_digit(peek())) advance();
    }
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

Token *identifier() {
    // size_t start;
    Token *token = (Token *)malloc(sizeof(Token));

    token->type = IDENTIFIER;

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
        case '>':
            token->type = (match_next('=') ? GREATER_EQUAL : GREATER);
            break;
        case '/':
            if (match_next('/')) {
                while (text[current] != '\n' && !(current >= length)) current++;
                free(token);
                goto skip_add;
            } else {
                token->type = SLASH;
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            free(token);
            goto skip_add;
        case '\n':
            line++;
            goto skip_add;
        case '\0':
            token->type = _EOF;
            break;
        case '"':
            token = string();
            break;
        // case 'o':
        // if (match_next('r'))
        // token->type = OR;
        // break;
        default:
            if (is_digit(text[current])) {
                number();
            } else if (is_alpha(peek())) {
                identifier();
            }
            if (strcicmp(&text[current], "and")) token->type = AND;
            if (strcicmp(&text[current], "class")) token->type = CLASS;
            if (strcicmp(&text[current], "else")) token->type = ELSE;
            if (strcicmp(&text[current], "false")) token->type = FALSE;
            if (strcicmp(&text[current], "for")) token->type = FOR;
            if (strcicmp(&text[current], "if")) token->type = IF;
            if (strcicmp(&text[current], "nil")) token->type = NIL;
            if (strcicmp(&text[current], "or")) token->type = OR;
            if (strcicmp(&text[current], "print")) token->type = PRINT;
            if (strcicmp(&text[current], "return")) token->type = RETURN;
            if (strcicmp(&text[current], "super")) token->type = SUPER;
            if (strcicmp(&text[current], "this")) token->type = THIS;
            if (strcicmp(&text[current], "true")) token->type = TRUE;
            if (strcicmp(&text[current], "var")) token->type = VAR;
            if (strcicmp(&text[current], "while")) token->type = WHILE;
            fprintf(stderr, "Unexpected character: %c\n", c);
            break;
        }
        printf("%ld\n", token->type);
        list_add(tokens, token);
        (*num_tokens)++;
    skip_add:
        current++;
    }
}
