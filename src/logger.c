#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

static LogLevel current_log_level = INFO;

void
set_log_level (LogLevel level)
{
  current_log_level = level;
}

void
log_message (LogLevel level, const char *fmt, ...)
{
  if (level < current_log_level)
    {
      return;
    }

  va_list args;

  va_start (args, fmt);
  vprintf (fmt, args);
  va_end (args);
}
