#ifndef ASDFOPENGL_LOG_H
#define ASDFOPENGL_LOG_H

#include <stdio.h>

typedef enum LogLevelE {
  LOG_ALL = 0, // Display all logs
  LOG_DEBUG = 1, // Debugging
  LOG_INFO = 2, // Program execution info
  LOG_WARNING = 3, // Recoverable errors
  LOG_ERROR = 4, // Unrecoverable errors
  LOG_NONE = 5, // Disable logging
} LogLevelE;

extern LogLevelE logLevel;

void logMsg(LogLevelE msgType, const char *fmt, ...);

#endif // ASDFOPENGL_LOG_H
