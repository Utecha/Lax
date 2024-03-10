#include <assert.h>

#include "log.h"

void
laxlog(LogLevel level, const char *fmt, ...)
{
    switch (level) {
        default: assert(0 && "Unreachable");
        case NONE:      fprintf(stderr, ""); break;
        case INFO:      fprintf(stderr, "[INFO] ");
        case WARNING:   fprintf(stderr, "[WARNING] ");
        case ERROR:     fprintf(stderr, "[ERROR] ");
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}
