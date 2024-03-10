#ifndef LAX_LOG_H
#define LAX_LOG_H

#include <stdio.h>

/*
 * LogLevel
 *
 * Determines what is prepended to the format string.
*/
typedef enum {
    NONE,
    INFO,
    WARNING,
    ERROR
} LogLevel;

/*
 * Logging Function that takes a format string and a
 * variable number of arguments. Outputs the
 * appropriate message based on the LogLevel.
 *
 * NONE prepends nothing (hence the name). Useful
 * for when you don't actually want something like
 * '[INFO]' prepended to your log message.
 *
 * Automatically appends a newline to the output,
 * so you do not need to do so yourself like with
 * the printf's.
*/
void
laxlog(LogLevel level, const char *fmt, ...);

#endif // LAX_LOG_H
