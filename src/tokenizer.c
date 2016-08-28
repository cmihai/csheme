#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "tokenizer.h"

Tokenizer* tok_init(FILE *f) {
	Tokenizer *state = malloc(sizeof(Tokenizer));
	state->f = f ? f : stdin;
	return state;
}

void tok_deinit(Tokenizer *t) {
	free(t);
}

Token* tok_alloc(TokenType type, const char *val) {
	Token *result = malloc(sizeof(Token));
	result->type = type;
	result->value = malloc(strlen(val) + 1);
	strcpy(result->value, val);
	return result;
}

void tok_free(Token *tok) {
	if (tok == NULL) {
		return;
	}
	if (tok->value != NULL) {
		free(tok->value);
	}
	free(tok);
}

Token* tok_next(Tokenizer *state) {
	char current_token[100];
	int i = 0;
	TokenType type = TOK_UNDEF;

	while (true) {
		char c = fgetc(state->f);
		if (feof(state->f)) {
			type = TOK_END;
			break;
		}
		if (isspace(c)) {
			if (type == TOK_UNDEF) {
				continue;
			}
			else {
				break;
			}
		}

		current_token[i++] = c;

		switch (type) {
		case TOK_UNDEF:
			if (c == '(') {
				type = TOK_OPEN_PAREN;
				goto out;
			}
			else if (c == ')') {
				type = TOK_CLOSE_PAREN;
				goto out;
			}
			else if (isdigit(c)) {
				type = TOK_NUMBER;
			}
			else if (c == '#') {
				type = TOK_BOOLEAN;
			}
			else {
				type = TOK_SYMBOL;
			}
			break;
		case TOK_SYMBOL:
		case TOK_NUMBER:
			if (c == '(' || c == ')') {
				ungetc(c, state->f);
				i--;
				goto out;
			}
			break;
		case TOK_BOOLEAN:
			if (c == 't' || c == 'f') {
				goto out;
			}
			if (c == '(' || c == ')') {
				ungetc(c, state->f);
				i--;
				goto out;
			}
			break;
		}
	}

out:
	current_token[i] = '\0';

	Token *t = tok_alloc(type, current_token);
	return t;
}