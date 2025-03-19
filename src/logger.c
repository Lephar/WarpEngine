#include "logger.h"

void debug(const char *fmt, ...) {
    if(DEBUG) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);
    }
}
