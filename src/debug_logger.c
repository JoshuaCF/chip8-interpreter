#include <stdarg.h>
#include <stdio.h>

#include "debug_logger.h"

#ifdef DEBUG
static FILE* debug_file;

void _initDebugLogging() {
	debug_file = fopen("LOG", "w");
}
void _debugWriteToLogFile(char* filename, unsigned int lineno, ...) {
	if (debug_file == NULL) return;
	va_list va_args;
	va_start(va_args, lineno);
	char* usr_fmt = va_arg(va_args, char*);
	char fmt[snprintf(NULL, 0, "[%s:%u] %s\n", filename, lineno, usr_fmt) + 1];
	sprintf(fmt, "[%s:%u] %s\n", filename, lineno, usr_fmt);
	vfprintf(debug_file, fmt, va_args);
	va_end(va_args);
}
void _endDebugLogging() {
	if (debug_file == NULL) return;
	fclose(debug_file);
}
#else
void _initDebugLogging() {}
void _debugWriteToLogFile(char* filename, unsigned int lineno, ...) {}
void _endDebugLogging() {}
#endif
