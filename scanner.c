#include "scanner.h"

#include <ctype.h>
#include <string.h>

#include "tokens.h"

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
	for (; *a && *b; a++, b++) {
		int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
		if (d != 0 || !*a || !*b) return d;
	}
	return 0;
}

bool match_next(char match) {
	if (text[current + 1] == '\0') return false;
	if (text[current + 1] != match) return false;
	current++;
	return true;
}

void string(Token *token, char delim) {
	size_t start = current + 1;
	while (peek_next() != delim && !is_at_end()) {
		// if (peek_next() == '\n') line++;
		advance();
	}

	if (is_at_end()) {
		fprintf(stderr, "%ld: Unexpected end of string", line);
	}

	advance();

	char *string = (char *)malloc(current - start);
	token->type = STRING;
	token->literal = string;
	memcpy(string, &text[start], current - start);
}

void number(Token *token) {
	size_t start = current;
	while (is_digit(peek_next()))
		advance();
	if (peek() == '.' && is_digit(peek_next())) {
		advance();
		while (is_digit(peek_next()))
			advance();
	}
	char *string = (char *)malloc(current - start + 1);
	token->type = NUMBER;
	strncpy(string, &text[start], current - start + 1);
	int *num = (int *)malloc(sizeof(int));
	*num = atoi(string);
	token->literal = num;
	free(string);
}

void identifier(Token *token) {
	size_t start = current;
	token->type = IDENTIFIER;

	while (is_alphanumeric(peek_next()))
		advance();
	char *string = (char *)malloc(current - start + 1);
	memset(string, 0, current - start + 1);
	strncpy(string, &text[start], current - start + 1);
	token->lexeme = string;
	// current--;
}

void scan_tokens(char *_text, size_t _length, size_t *num_tokens,
				 LinkedList *tokens) {

	text = _text;
	length = _length;
	current = 0;
	line = 1;

	while (!is_at_end()) {
		Token *token = (Token *)malloc(sizeof(Token));
		token->type = -1;
		token->line = line;
		char c = text[current];
		switch (c) {
			case '(': token->type = LEFT_PAREN; break;
			case ')': token->type = RIGHT_PAREN; break;
			case '{': token->type = LEFT_BRACE; break;
			case '}': token->type = RIGHT_BRACE; break;
			case ',': token->type = COMMA; break;
			case '.': token->type = DOT; break;
			case '-': token->type = MINUS; break;
			case '+': token->type = PLUS; break;
			case ';': token->type = SEMICOLON; break;
			case ':':
				token->type = (match_next('=') ? COLON_EQUAL : COLON);
				break;
			case '*': token->type = STAR; break;
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
			case '\t': free(token); goto skip_add;
			case '\n':
				line++;
				free(token);
				goto skip_add;
			case EOF:
			case '\0': token->type = ENDOFFILE; break;
			case '"': string(token, '"'); break;
			case '\'': string(token, '\''); break;
			default:
				if (is_digit(peek())) {
					number(token);
					break;
				} else if (strcicmp(&text[current], "and") == 0) {
					token->type = AND;
					current += 3;
				} else if (strcicmp(&text[current], "class") == 0) {
					token->type = CLASS;
					current += 4;
				} else if (strcicmp(&text[current], "else") == 0) {
					token->type = ELSE;
				} else if (strcicmp(&text[current], "false") == 0) {
					token->type = FALSE;
				} else if (strcicmp(&text[current], "for") == 0) {
					token->type = FOR;
				} else if (strcicmp(&text[current], "if") == 0) {
					token->type = IF;
					current += 2;
				} else if (strcicmp(&text[current], "nil") == 0) {
					token->type = NIL;
				} else if (strcicmp(&text[current], "or") == 0) {
					token->type = OR;
				} else if (strcicmp(&text[current], "print") == 0) {
					token->type = PRINT;
					current += 5;
				} else if (strcicmp(&text[current], "return") == 0) {
					token->type = RETURN;
				} else if (strcicmp(&text[current], "super") == 0) {
					token->type = SUPER;
				} else if (strcicmp(&text[current], "this") == 0) {
					token->type = THIS;
				} else if (strcicmp(&text[current], "true") == 0) {
					token->type = TRUE;
				} else if (strcicmp(&text[current], "var") == 0) {
					token->type = VAR;
					current += 3;
				} else if (strcicmp(&text[current], "while") == 0) {
					token->type = WHILE;
				} else if (strcicmp(&text[current], "fn") == 0) {
					token->type = FUNC;
					current += 2;
				} else if (strcicmp(&text[current], "int") == 0) {
					token->type = INT;
					current += 3;
				} else if (is_alpha(peek())) {
					identifier(token);
					break;
				}

				if (token->type != -1) {
					break;
				}

				fprintf(stderr, "Unexpected character: %c on line: %ld\n", c,
						line);
				break;
		}
		// print_token(token->type);
		list_add(tokens, token);
		(*num_tokens) += 1;
		if (token->type == ENDOFFILE) break;
	skip_add:
		current++;
	}
	Token *token = (Token *)malloc(sizeof(Token));
	token->type = ENDOFFILE;
	list_add(tokens, token);
	(*num_tokens) += 1;
}

void print_token(int type) {
	printf("<");
	switch (type) {
		case LEFT_PAREN:
		case RIGHT_PAREN: printf("PAREN"); break;
		case LEFT_BRACE: printf("LBRACE"); break;
		case RIGHT_BRACE: printf("RBRACE"); break;
		case COMMA:
		case DOT: printf("PUNCTUATION"); break;
		case STAR: printf("STAR"); break;
		case SLASH: printf("SLASH"); break;
		case MINUS: printf("MINUS"); break;
		case PLUS: printf("PLUS"); break;
		case SEMICOLON: printf("SEMICOLON"); break;
		case BANG_EQUAL:
		case EQUAL_EQUAL: printf("EQUALITY"); break;
		case EQUAL: printf("ASSIGNMENT"); break;
		case GREATER:
		case GREATER_EQUAL:
		case LESS:
		case LESS_EQUAL: printf("MATH_EQUALITY"); break;
		case AND:
		case OR:
		case BANG: printf("BOOLEAN_OPERATOR"); break;
		case CLASS: printf("CLASS"); break;
		case IF: printf("IF"); break;
		case ELSE: printf("ELSE"); break;
		case TRUE:
		case FALSE: printf("BOOLEAN"); break;
		case FUNC: printf("FUNCTION"); break;
		case FOR:
		case WHILE: printf("LOOP"); break;
		case NIL: printf("NIL"); break;
		case IDENTIFIER: printf("IDENTIFIER"); break;
		case NUMBER: printf("NUMBER" /*, *(int *)token->literal*/); break;
		case STRING: printf("STRING" /*, (char *)token->literal*/); break;
		case VAR: printf("VAR"); break;
		case PRINT: printf("PRINT"); break;
		case INT: printf("INT"); break;
		case ENDOFFILE: printf("ENDOFFILE"); break;
		default: printf("Other" /*, token->type*/); break;
	}
	printf(">\n");
}
