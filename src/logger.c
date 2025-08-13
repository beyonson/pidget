#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

static LogLevel current_log_level = INFO;

static const char *log_level_strings[]
    = { "DEBUG", "INFO", "WARNING", "ERROR" };

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

  const char *level_str
      = (level >= 0 && level <= ERROR) ? log_level_strings[level] : "UNKNOWN";

  printf ("[%s] ", level_str);

  va_list args;

  va_start (args, fmt);
  vprintf (fmt, args);
  va_end (args);
}
