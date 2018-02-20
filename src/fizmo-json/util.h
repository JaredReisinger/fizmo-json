// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_UTIL_H
#define FIZMO_JSON_UTIL_H

#include <string>

extern "C" {
    #include <tools/types.h>
}

#define trace(level, args...) trace_impl(level, true, __func__, __FILE__, __LINE__, args)
#define tracex(level, args...) trace_impl(level, false, __func__, __FILE__, __LINE__, args)

extern void trace_impl(int level, bool funcentry, const char *funcname, const char *filename, int line, const char *fmt, ...);

extern void set_trace_level(int trace_level);

// Note that the fizmo character conversion functions almost, but not quite,
// convert into UTF-8.  In particular, zucs_string_to_utf8_string() converts
// into the 1993 variation of UTF-8 which allows for length 5 and 6 encodings.
// This was removed from UTF-8 in 2003 (see RFC 3629).  Further, the single-
// character helper, zucs_char_to_latin1_char() simply lops off the high bits,
// and returns '?' for any multi-octet character.  In order to more-fully
// support modern output, we use system UTF-8 encodings.
std::string ToUtf8(const z_ucs *str);
std::string ToUtf8(const z_ucs ch);

std::u32string FromUtf8(const char *);

#endif // FIZMO_JSON_UTIL_H
