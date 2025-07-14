#ifndef LOGGER_H
#define LOGGER_H

typedef enum
{
  DEBUG,
  INFO,
  WARNING,
  ERROR
} LogLevel;

void set_log_level (LogLevel level);
void log_message (LogLevel level, const char *fmt, ...);

#endif // LOGGER_H
