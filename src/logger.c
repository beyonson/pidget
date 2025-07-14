#include <stdio.h>
#include <time.h>
#include "logger.h"

static LogLevel current_log_level = INFO;

void set_log_level (LogLevel level)
{
  current_log_level = level;
}

void log_message (LogLevel level, const char* text)
{
  if (level < current_log_level) {
    return;
  }

  const char* level_strings[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
  printf("[%s] %s\n", level_strings[level], text);
}
