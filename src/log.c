#include "log.h"

#include <stdarg.h>
#include <stdio.h>

LogLevelE logLevel = LOG_INFO;

void logMsg(LogLevelE msgType, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  if (msgType < logLevel) {
    return;
  }

  switch (msgType) {
  case LOG_DEBUG:
    printf("[DEBUG]: ");
    break;
  case LOG_INFO:
    printf("[INFO]: ");
    break;
  case LOG_WARNING:
    printf("[WARN]: ");
    break;
  case LOG_ERROR:
    printf("[ERROR]: ");
    break;
  default:
    break;
  }

  vprintf(fmt, args);
  printf("\n");

  va_end(args);
}
