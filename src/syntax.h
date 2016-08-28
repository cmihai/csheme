#ifndef SYNTAX_H
#define SYNTAX_H

#include "tokenizer.h"

typedef enum {
	SYN_NIL = 0,
	SYN_NUMBER,
	SYN_SYMBOL,
	SYN_BOOLEAN,
	SYN_PAIR,
} SyntaxType;

typedef struct Obj_ {
	SyntaxType type;
	unsigned int ref;

	union {
		int num;
		char *str;
		struct {
			struct Obj_ *left;
			struct Obj_ *right;
		};
	} value;
} Obj;

void syn_print(Obj *node);

Obj* syn_next(Tokenizer *tok_state);

Obj *syn_alloc();

void INCREF(Obj* o);

void DECREF(Obj* o);

#endif
