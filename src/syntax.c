#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "syntax.h"

Obj *syn_alloc() {
	Obj *result = calloc(1, sizeof(Obj));
	result->ref = 1;
	return result;
}

void syn_free(Obj* n) {
	if (n == NULL)
		return;
	free(n);
}

void syn_free_all(Obj* n) {
	if (n == NULL)
		return;
	syn_free(n);
}

void INCREF(Obj* o) {
	if (o == NULL)
		return;

	o->ref++;
}

void DECREF(Obj* o) {
	if (o == NULL)
		return;

	if (--o->ref == 0) {
		if (o->type == SYN_PAIR) {
			DECREF(o->value.left);
			DECREF(o->value.right);
		}
		syn_free(o);
	}
}

Obj* syn_read_list(Tokenizer *tok_state);

Obj* syn_next(Tokenizer *tok_state) {
	Token *tok = tok_next(tok_state);
	Obj *node = NULL;

	switch (tok->type) {
	case TOK_END:
		break;
	case TOK_OPEN_PAREN:
		node = syn_read_list(tok_state);
		break;
	case TOK_CLOSE_PAREN:
		break;
	case TOK_BOOLEAN:
		node = syn_alloc();
		node->type = SYN_BOOLEAN;
		if (!strcmp(tok->value, "#t"))
			node->value.num = 1;
		else
			node->value.num = 0;
		break;
	case TOK_NUMBER:
		node = syn_alloc();
		node->type = SYN_NUMBER;
		node->value.num = atoi(tok->value);
		break;
	case TOK_SYMBOL:
		if (!strcmp(tok->value, "nil")) {
			node = syn_alloc();
		}
		else {
			node = syn_alloc();
			node->type = SYN_SYMBOL;
			node->value.str = malloc(strlen(tok->value) + 1);
			strcpy(node->value.str, tok->value);
		}
		break;
	}

	tok_free(tok);

	return node;
}

Obj* syn_read_list(Tokenizer *tok_state) {
	Obj *head = NULL;
	Obj *last = NULL;
	Obj *node;
	Obj *next;
	
	do {
		next = syn_next(tok_state);
		if (next == NULL) {
			node = syn_alloc();
		}
		else {
			node = syn_alloc();
			node->type = SYN_PAIR;
			node->value.left = next;
		}

		if (head == NULL) {
			head = node;
			last = head;
		}
		else {
			last->value.right = node;
			last = node;
		}
	} while (next != NULL);

	assert(head != NULL);

	return head;
}

void syn_print(Obj *node) {
	if (node == NULL) {
		printf("<NULL>");
		return;
	}

	switch (node->type) {
	case SYN_NIL:
		printf("()");
		break;
	case SYN_NUMBER:
		printf("%d", node->value.num);
		break;
	case SYN_SYMBOL:
		printf("'%s", node->value.str);
		break;
	case SYN_BOOLEAN:
		if (node->value.num) {
			printf("#t");
		}
		else {
			printf("#f");
		}
		break;
	case SYN_PAIR:
		printf("(");
		while (node->type != SYN_NIL) {
			syn_print(node->value.left);
			node = node->value.right;
			if (node->type != SYN_NIL)
			{
				printf(" ");
			}
		}
		printf(")");
		break;
	}
}
