
#ifndef LOGGING_MANANGER_H
#define LOGGING_MANANGER_H 1

#include "external/log.h"

#include <memory>
#include <stdarg.h>
#include <stdio.h>

void SetLogFileEX(const char* filename);
void LogEX(const char* format, ...);
void LogEX(int level, const char* format, ...);

#endif