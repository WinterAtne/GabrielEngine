
void _error_call(const char* message, const char* file, const int line, const char* func);
#define ERROR(message) _error_call(message, __FILE__, __LINE__, __FUNCTION__);

