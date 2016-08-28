#include <string.h>
#include <stdlib.h>

static inline char* str_clone(const char* str) {
	size_t len = strlen(str) + 1;
	char* result = (char*) malloc(len);
	strcpy(result, str);
	return result;
}