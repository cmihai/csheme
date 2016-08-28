#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>

typedef struct {
	FILE *f;
} Tokenizer;

typedef enum {
	TOK_UNDEF = 0,
	TOK_END,
	TOK_OPEN_PAREN,
	TOK_CLOSE_PAREN,
	TOK_NUMBER,
	TOK_BOOLEAN,
	TOK_SYMBOL
} TokenType;

typedef struct {
	TokenType type;
	char *value;
} Token;

Tokenizer* tok_init();

void tok_free(Token *tok);

Token* tok_next(Tokenizer *state);

#endif
