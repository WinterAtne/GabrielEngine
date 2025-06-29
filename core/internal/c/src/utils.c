#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const char* file_read(const char* name) {
	FILE* source = fopen(name, "r");
	if (!source) {
		return NULL;
	}

	fseek(source, 0L, SEEK_END);
	long size = ftell(source);
	rewind(source);

	if (!size) {
		return NULL;
	}

	char* text = calloc(size+1, sizeof(char));
	if (!text) {
		return NULL;
	}

	long err = fread((void*)text, 1, size, source);
	if (err != size) {
		free(text);
		return NULL;
	}

	fclose(source);
	return text;
}

void _error_call_do_not_call(const char* message, const char* file, const int line, const char* func) {
	fprintf(stderr, "\nError: %s\n %s:%i | func:%s\n", message, file, line, func);
}
