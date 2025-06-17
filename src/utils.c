#include <stdio.h>

#include "utils.h"

void _error_call_do_not_call(const char* message, const char* file, const int line, const char* func) {
	fprintf(stderr, "\nError: %s\n %s:%i | func:%s\n", message, file, line, func);
}
