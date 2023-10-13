#include "scanner.h"

#include <ctype.h>
#include <string.h>

char *text;
size_t current = 0;
size_t line = 0;
size_t length;

char advance() { return text[current++]; }
char peek_next() { return text[current + 1]; }
bool is_at_end() { return (current >= length); }
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

void string(Token *token) {
    size_t start = current + 1;
    while (text[current + 1] != '"' && !(current >= length)) {
        if (text[current + 1] == '\n') line++;
        advance();
    }

    if (is_at_end()) { fprintf(stderr, "%ld: Unexpected end of string", line); }

    advance();

    char *string = (char *)malloc(current - start);
    // Token *token = (Token *)malloc(sizeof(Token));
    token->line = line;
    token->type = STRING;
    token->literal = string;
    memcpy(string, text + current, current - start);
    // return token;
}

void number(Token *token) {
    size_t start = current;
    while (is_digit(peek())) advance();
    if (peek() == '.' && is_digit(peek_next())) {
        advance();
        while (is_digit(peek())) advance();
    }
    char *string = (char *)malloc(current - start);
    token->line = line;
    token->type = NUMBER;
    strncpy(string, &text[start], current - start);
    double *num = (double *)malloc(sizeof(double));
    *num = atof(string);
    token->literal = num;
    free(string);
}

void identifier(Token *token) {
    size_t start = current;
    token->type = IDENTIFIER;

    while (is_alphanumeric(peek())) advance();
    char *string = (char *)malloc(current - start);
    strncpy(string, &text[start], current - start);
    token->lexeme = string;
}

void scan_tokens(char *_text, size_t _length, size_t *num_tokens,
                 LinkedList *tokens) {

    text = _text;
    length = _length;

    while (!is_at_end()) {
        Token *token = (Token *)malloc(sizeof(Token));
        token->type = -1;
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
                    while (text[current] != '\n' && !(current >= length))
                        current++;
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
                free(token);
                goto skip_add;
            case '\0':
                token->type = ENDOFFILE;
                break;
            case '"':
                string(token);
                break;
            default:
                if (is_digit(peek())) {
                    number(token);
                    break;
                } /*else if (is_alpha(peek())) {
                    identifier(token);
                    break;
                }*/
                else if (strcicmp(&text[current], "and") == 0)
                    token->type = AND;
                else if (strcicmp(&text[current], "class") == 0)
                    token->type = CLASS;
                else if (strcicmp(&text[current], "else") == 0)
                    token->type = ELSE;
                else if (strcicmp(&text[current], "false") == 0)
                    token->type = FALSE;
                else if (strcicmp(&text[current], "for") == 0)
                    token->type = FOR;
                else if (strcicmp(&text[current], "if") == 0)
                    token->type = IF;
                else if (strcicmp(&text[current], "nil") == 0)
                    token->type = NIL;
                else if (strcicmp(&text[current], "or") == 0)
                    token->type = OR;
                else if (strcicmp(&text[current], "print") == 0)
                    token->type = PRINT;
                else if (strcicmp(&text[current], "return") == 0)
                    token->type = RETURN;
                else if (strcicmp(&text[current], "super") == 0)
                    token->type = SUPER;
                else if (strcicmp(&text[current], "this") == 0)
                    token->type = THIS;
                else if (strcicmp(&text[current], "true") == 0)
                    token->type = TRUE;
                else if (strcicmp(&text[current], "var") == 0)
                    token->type = VAR;
                else if (strcicmp(&text[current], "while") == 0)
                    token->type = WHILE;
                else if (strcicmp(&text[current], "fun") == 0)
                    token->type = FUN;
                else if (is_alpha(peek())) {
                    identifier(token);
                    break;
                }

                if (token->type != -1) { break; }

                fprintf(stderr, "Unexpected character: %c on line: %ld\n", c,
                        line);
                break;
        }
        print_token(token);
        list_add(tokens, token);
        (*num_tokens)++;
    skip_add:
        current++;
    }
}

void print_token(Token *token) {
    printf("<");
    switch (token->type) {
        case LEFT_PAREN:
        case RIGHT_PAREN:
            printf("PAREN");
            break;
        case LEFT_BRACE:
        case RIGHT_BRACE:
            printf("BRACE");
            break;
        case COMMA:
        case DOT:
            printf("PUNCTUATION");
            break;
        case STAR:
        case SLASH:
        case MINUS:
        case PLUS:
            printf("MATH");
            break;
        case SEMICOLON:
            printf("SEMICOLON");
            break;
        case BANG:
        case BANG_EQUAL:
        case EQUAL:
        case EQUAL_EQUAL:
            printf("EQUALITY");
            break;
        case GREATER:
        case GREATER_EQUAL:
        case LESS:
        case LESS_EQUAL:
            printf("MATH_EQUALITY");
            break;
        case AND:
        case OR:
            printf("BOOLEAN_OPERATOR");
            break;
        case CLASS:
            printf("CLASS");
            break;
        case IF:
        case ELSE:
            printf("CONDITIONAL");
            break;
        case TRUE:
        case FALSE:
            printf("BOOLEAN");
            break;
        case FUN:
            printf("FUNCTION");
            break;
        case FOR:
        case WHILE:
            printf("LOOP");
            break;
        case NIL:
            printf("NIL");
            break;
        case IDENTIFIER:
            printf("IDENTIFIER: %s", token->lexeme);
            break;
        case NUMBER:
            printf("NUMBER: %.1f", *(double *)token->literal);
            break;
        default:
            printf("Other: %ld", token->type);
            break;
    }
    printf(">, ");
}
