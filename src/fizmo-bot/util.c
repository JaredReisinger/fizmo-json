/* util.c
 *
 * This file is part of fizmo-bot.  Please see LICENSE.md for the license.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "config.h"

// void dbg(const char *msg) {
//     fprintf(stderr, "%s\n", msg);
// }


void trace_impl(const char *funcname, const char* filename, const int line, const char *fmt, ...) {
    char *formatted = NULL;
    va_list args;
    va_start(args, fmt);
    vasprintf(&formatted, fmt, args);
    va_end(args);

    fprintf(stderr, "TRACE %s(%s) [%s:%d]\n", funcname, formatted, filename, line);

    free(formatted);
}
