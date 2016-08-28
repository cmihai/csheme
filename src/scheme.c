#include <assert.h>

#include "scheme.h"
#include "syntax.h"
#include "util.h"

Scope* scope_init(Scope* parent) {
	Scope* result = (Scope*) calloc(1, sizeof(Scope));
	result->parent = parent;

	return result;
}

void scope_free(Scope* scope) {
	free(scope);
}

Obj * scope_add(Scope* scope, const char* varname, Obj *val) {
	VarList* var = (VarList*) calloc(1, sizeof(VarList));
	var->name = str_clone(varname);
	var->value = val;
	var->next = scope->vars;
	scope->vars = var;
	return val;
}

Obj* scope_find(Scope* scope, const char* varname) {
	while (scope) {
		VarList* var = scope->vars;
		while (var != NULL) {
			if (!strcmp(var->name, varname)) {
				return var->value;
			}
			var = var->next;
		}

		scope = scope->parent;
	}
	return NULL;
}

Obj* eval(Scope* scope, Obj *node) {
	Obj *result;

	switch (node->type) {
	case SYN_NUMBER:
	case SYN_BOOLEAN:
		break;
	case SYN_SYMBOL: {
		Obj *obj = scope_find(scope, node->value.str);
		if (obj != NULL) {
			INCREF(obj);
			return obj;
		}
		break;
	}
	case SYN_PAIR:
		if (CAR(node)->type == SYN_SYMBOL) {
			// "if" is special
			if (!strcmp(CAR(node)->value.str, "if")) {
				Obj *obj = eval(scope, ARG1(node));

				if (obj->value.num) {
					DECREF(obj);
					result = eval(scope, ARG2(node));
				}
				else {
					DECREF(obj);
					result = eval(scope, ARG3(node));
				}
				return result;
			}
			else if (!strcmp(CAR(node)->value.str, "quote")) {
				result = ARG1(node);
				INCREF(result);
				return result;
			}
			else if (!strcmp(CAR(node)->value.str, "define")) {
				const char *varname = ARG1(node)->value.str;
				Obj *val = ARG2(node);
				node = scope_add(scope, varname, eval(scope, val));
				break;
			}
			else if (!strcmp(CAR(node)->value.str, "lambda")) {
				break;
			}
		}

		Obj *head = NULL, *last = NULL, *left = NULL, *current = node;
		do {
			Obj *pair = CONS(eval(scope, CAR(current)), syn_alloc());
			current = CDR(current);

			if (head == NULL) {
				head = last = pair;
			}
			else {
				last->value.right = pair;
				last = pair;
			}
		} while (current->type != SYN_NIL);

		result = apply(scope, head);
		return result;
	}

	INCREF(node);
	return node;
}

Obj* apply(Scope* scope, Obj *node) {
	Obj* result = NULL;
	const char *sym = CAR(node)->value.str;

	if (!strcmp(sym, "+")) {
		int sum = 0;
		node = CDR(node);

		while (!NILP(node)) {
			sum += CAR(node)->value.num;
			node = CDR(node);
		}

		result = syn_alloc();
		result->type = SYN_NUMBER;
		result->value.num = sum;
	}
	else if (!strcmp(sym, "-")) {
		int diff = ARG1(node)->value.num;
		node = CDR(CDR(node));

		while (!NILP(node)) {
			diff -= CAR(node)->value.num;
			node = CDR(node);
		}

		result = syn_alloc();
		result->type = SYN_NUMBER;
		result->value.num = diff;
	}
	else if (!strcmp(sym, "=")) {
		int n1 = ARG1(node)->value.num;
		int n2 = ARG2(node)->value.num;

		result = syn_alloc();
		result->type = SYN_BOOLEAN;
		result->value.num = (n1 == n2);
	}
	else if (!strcmp(CAR(node)->value.str, "null?")) {
		result = syn_alloc();
		result->type = SYN_BOOLEAN;
		result->value.num = NILP(ARG1(node));
	}
	else if (!strcmp(CAR(node)->value.str, "car")) {
		result = CAR(ARG1(node));
		INCREF(result);
		return result;
	}
	else if (!strcmp(CAR(node)->value.str, "cdr")) {
		result = CDR(ARG1(node));
		INCREF(result);
		return result;
	}
	else if (!strcmp(CAR(node)->value.str, "cons")) {
		return CONS(ARG1(node), ARG2(node));
	}
	else if (CAR(node)->type == SYN_PAIR && !strcmp(CAR(CAR(node))->value.str, "lambda")) {
		Obj *lambda = CAR(node);

		// Create new scope
		Scope *s = scope_init(scope);

		Obj *vals;
		Obj *vars;

		if (SYMBOLP(ARG1(lambda))) {
			vars = CONS(ARG1(lambda), syn_alloc());
			vals = CONS(CDR(node), syn_alloc());
		}
		else {
			vars = ARG1(lambda);
			vals = CDR(node);
		}

		while (!NILP(vars) && !NILP(vals)) {
			INCREF(CAR(vals));
			scope_add(s, CAR(vars)->value.str, CAR(vals));
			vars = CDR(vars);
			vals = CDR(vals);
		}

		// Execute lambda
		result = eval(s, ARG2(lambda));
		scope_free(s);
	}

	assert(result != NULL);

	return result;
}
