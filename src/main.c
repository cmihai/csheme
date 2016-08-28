#include <stdbool.h>
#include <string.h>

#include "syntax.h"
#include "scheme.h"

const char *PROMPT = "> ";

int main() {
	Scope* scope = scope_init(NULL);

	// Load runtime
	FILE* runtime = fopen("runtime.scm", "rt");
	if (runtime) {
		Tokenizer *rtk = tok_init(runtime);
		while (true) {
			Obj *t = syn_next(rtk);
			if (t == NULL) {
				break;
			}
			Obj *result = eval(scope, t);
			DECREF(t);
			DECREF(result);
		}
		tok_deinit(rtk);
		fclose(runtime);
	}

	Tokenizer *tok = tok_init(stdin);
	while (true) {
		printf(PROMPT);
		fflush(stdout);

		Obj *t = syn_next(tok);
		if (t == NULL) {
			break;
		}

		Obj *result = eval(scope, t);
		DECREF(t);
		syn_print(result);
		printf("\n");
		DECREF(result);
	}
	tok_deinit(tok);

	return 0;
}