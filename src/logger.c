#include "logger.h"
#include <stdio.h>
#include <time.h>

void logger_event(const char *message) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char stamp[32] = "unknown-time";
    if (local) strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", local);
    printf("[LOG %s] %s\n", stamp, message ? message : "(no message)");
}
