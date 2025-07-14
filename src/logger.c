#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

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
  const char *level_strings[] = { "DEBUG", "INFO", "WARNING", "ERROR" };

  va_start (args, fmt);
  vprintf (fmt, args);
  va_end (args);
}
