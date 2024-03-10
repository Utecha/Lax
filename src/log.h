#ifndef LAXLOG_H
#define LAXLOG_H

#include <stdarg.h>

typedef enum {
    NONE,
    INFO,
    WARNING,
    ERROR
} LogLevel;

void
laxlog(LogLevel level, const char *fmt, ...);

#endif // LAXLOG_H
