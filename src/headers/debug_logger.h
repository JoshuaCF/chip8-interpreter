#pragma once

#ifdef DEBUG
#define enable_logging() _initDebugLogging()
#define log_dbg(...) (_debugWriteToLogFile(__FILE__, __LINE__, __VA_ARGS__))
#define disable_logging() _endDebugLogging();

void _initDebugLogging();
void _debugWriteToLogFile(char* filename, unsigned int lineno, ...);
void _endDebugLogging();
#else
#define enable_logging()
#define log_dbg(...)
#define disable_logging()
#endif
