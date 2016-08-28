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
			if (!strcmp(CAR(node)->value.str, "cons")) {
				result = CONS(ARG1(node), ARG2(node));
				return result;
			}
			if (!strcmp(CAR(node)->value.str, "define")) {
				const char *varname = ARG1(node)->value.str;
				Obj *val = ARG2(node);
				node = scope_add(scope, varname, eval(scope, val));
				break;
			}
			if (!strcmp(CAR(node)->value.str, "lambda")) {
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

		return apply(scope, head);
	}

	INCREF(node);
	return node;
}

Obj* apply(Scope* scope, Obj *node) {
	const char *sym = CAR(node)->value.str;

	if (!strcmp(sym, "+")) {
		int sum = 0;
		node = CDR(node);

		while (!NILP(node)) {
			sum += CAR(node)->value.num;
			node = CDR(node);
		}

		node = syn_alloc();
		node->type = SYN_NUMBER;
		node->value.num = sum;
	}
	else if (!strcmp(sym, "-")) {
		int diff = ARG1(node)->value.num;
		node = CDR(CDR(node));

		while (!NILP(node)) {
			diff -= CAR(node)->value.num;
			node = CDR(node);
		}

		node = syn_alloc();
		node->type = SYN_NUMBER;
		node->value.num = diff;
	}
	else if (!strcmp(sym, "=")) {
		int n1 = ARG1(node)->value.num;
		int n2 = ARG2(node)->value.num;

		node = syn_alloc();
		node->type = SYN_NUMBER;
		node->value.num = (n1 == n2);
	}
	else if (CAR(node)->type == SYN_PAIR && !strcmp(CAR(CAR(node))->value.str, "lambda")) {
		Obj *lambda = CAR(node);

		// Create nvew scope
		Scope *s = scope_init(scope);

		Obj *vars = ARG1(lambda);
		Obj *vals = CDR(node);

		while (!NILP(vars) && !NILP(vals)) {
			INCREF(CAR(vals));
			scope_add(s, CAR(vars)->value.str, CAR(vals));
			vars = CDR(vars);
			vals = CDR(vals);
		}
		 
		// Execute lambda
		Obj *result = eval(s, ARG2(lambda));
		scope_free(s);
		return result;
	}

	return node;
}
