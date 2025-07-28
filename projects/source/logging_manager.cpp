#include "logging_manager.h"

void SetLogFileEX(const char* filename) {
	static bool initialized = false;
	if (initialized) return;

	int level = LOG_INFO_EX;

	log_set_level(level);
	log_set_quiet(true);  // Disable logging to stdout


	FILE* file = fopen(filename, "w");
	if (file) {
		log_add_fp(file, level);
		initialized = true;
	}
	else {
		// fallback to stderr if file open failed
		log_set_quiet(false);
		log_error("Failed to open log file: %s", filename);
	}
}

void LogEX(const char* format, ...) {
	char buffer[2048];  // adjust as needed

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	// Dispatch to log.c
	log_info("%s", buffer);
}


void LogEX(int level, const char* format, ...) {
	char buffer[2048];  // adjust as needed

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	// Dispatch to log.c
	switch (level) {
	case LOG_TRACE_EX: log_trace("%s", buffer); break;
	case LOG_DEBUG_EX: log_debug("%s", buffer); break;
	case LOG_INFO_EX:  log_info("%s", buffer); break;
	case LOG_WARN_EX:  log_warn("%s", buffer); break;
	case LOG_ERROR_EX: log_error("%s", buffer); break;
	case LOG_FATAL_EX: log_fatal("%s", buffer); break;
	default:        log_info("%s", buffer); break;
	}
}