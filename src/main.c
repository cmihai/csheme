#include <stdbool.h>
#include <string.h>

#include "syntax.h"
#include "scheme.h"

const char *PROMPT = "> ";

int main() {
	Tokenizer *state = tok_init();
	Scope* scope = scope_init(NULL);

	while (true) {
		printf(PROMPT);
		fflush(stdout);

		Obj *t = syn_next(state);
		if (t == NULL) {
			break;
		}

		Obj *result = eval(scope, t);
		DECREF(t);
		syn_print(result);
		printf("\n");
		DECREF(result);
	}

	return 0;
}