#include <assert.h>
#include <stdarg.h>

#include "log.h"

void
laxlog(LogLevel level, const char *fmt, ...)
{
    switch (level) {
        default: assert(0 && "Unreachable");
        case NONE:    break;
        case INFO:    fprintf(stderr, "[INFO] ");    break;
        case WARNING: fprintf(stderr, "[WARNING] "); break;
        case ERROR:   fprintf(stderr, "[ERROR] ");   break;
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}
