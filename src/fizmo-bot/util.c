// util.c
//
// This file is part of fizmo-bot.  Please see LICENSE.md for the license.


#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#include "config.h"

// void dbg(const char *msg) {
//     fprintf(stderr, "%s\n", msg);
// }

const int trace_level = 0;


void trace_impl(int level, bool funcentry, const char *funcname, const char *filename, int line, const char *fmt, ...) {
    if (level > trace_level) {
        return;
    }

    char *formatted = NULL;
    va_list args;

    va_start(args, fmt);
    vasprintf(&formatted, fmt, args);
    va_end(args);

    char *message;
    if (funcentry) {
        asprintf(&message, "%s(%s)", funcname, formatted);
    } else {
        asprintf(&message, "%s %s", funcname, formatted);
    }

    fprintf(stderr, "\e[38;5;15mTRACE\e[0m \e[38;5;7m%-70s\e[0m \e[38;5;15m[%s:%d]\e[0m\n", message, filename, line);

    free(message);
    free(formatted);
}
