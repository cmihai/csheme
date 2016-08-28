#ifndef SEMANTICS_H
#define SEMANTICS_H

#include <stdbool.h>

#include "syntax.h"

typedef struct VarList_ {
	const char *name;
	Obj *value;
	struct VarList_ *next;
} VarList;

typedef struct Scope_ {
	VarList *vars;
	struct Scope_ *parent;
} Scope;

Scope* scope_init(Scope* parent);

Obj* eval(Scope* scope, Obj *node);

Obj* apply(Scope* scope, Obj *node);


static inline Obj* CAR(Obj *pair) {
	return pair->value.left;
}
static inline Obj* CDR(Obj *pair) {
	return pair->value.right;
}
static inline Obj* ARG1(Obj *pair) {
	return CAR(CDR(pair));
}
static inline Obj* ARG2(Obj *pair) {
	return CAR(CDR(CDR(pair)));
}
static inline Obj* ARG3(Obj *pair) {
	return CAR(CDR(CDR(CDR(pair))));
}
static inline Obj* ARG4(Obj *pair) {
	return CAR(CDR(CDR(CDR(CDR(pair)))));
}
static inline bool NILP(Obj *obj) {
	return (obj != NULL && obj->type == SYN_NIL);
}
static inline bool CONSP(Obj *obj) {
	return (obj != NULL && obj->type == SYN_PAIR);
}
static inline bool SYMBOLP(Obj *obj) {
	return (obj != NULL && obj->type == SYN_SYMBOL);
}
static inline Obj* CONS(Obj *left, Obj *right) {
	Obj *pair = syn_alloc();
	pair->type = SYN_PAIR;
	pair->value.left = left;
	pair->value.right = right;
	INCREF(left);
	INCREF(right);
	return pair;
}

static inline bool CALLABLE(Obj* node) {
	return node->type == SYN_PAIR && !(CAR(node)->type == SYN_SYMBOL && !strcmp(CAR(node)->value.str, "lambda"));
}

#endif // !SEMANTICS_H
