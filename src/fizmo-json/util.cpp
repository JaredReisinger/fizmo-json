// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "util.h"
#include <codecvt>
#include <locale>

extern "C" {
    #include <stdarg.h>
}

static int current_trace_level = 0;


void trace_impl(int level, bool funcentry, const char *funcname, const char *filename, int line, const char *fmt, ...) {
    if (level > current_trace_level) {
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

void set_trace_level(int trace_level) {
    current_trace_level = trace_level;
    // tracex(1, "trace level set to %d", trace_level);
}


static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;


std::string ToUtf8(const z_ucs *str) {
    return utf8conv.to_bytes((const wchar_t *)str);
}

std::string ToUtf8(const z_ucs ch) {
    return utf8conv.to_bytes((const wchar_t)ch);
}
