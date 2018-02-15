// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_UTIL_H
#define FIZMO_JSON_UTIL_H

// extern void dbg(const char *msg);

extern "C" {
    #include <stdbool.h>
}

#define trace(level, args...) trace_impl(level, true, __func__, __FILE__, __LINE__, args)
#define tracex(level, args...) trace_impl(level, false, __func__, __FILE__, __LINE__, args)

extern void trace_impl(int level, bool funcentry, const char *funcname, const char *filename, int line, const char *fmt, ...);

extern void set_trace_level(int trace_level);

#endif // FIZMO_JSON_UTIL_H
