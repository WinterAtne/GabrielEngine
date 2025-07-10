#pragma once
// Macro allows us to grab the file name, line number, and function of the caller
void _error_call_do_not_call(const char* message, const char* file, const int line, const char* func);
#define error(message) _error_call_do_not_call(message, __FILE__, __LINE__, __FUNCTION__);
