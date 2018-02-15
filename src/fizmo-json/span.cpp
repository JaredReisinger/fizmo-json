// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "span.h"

#include "util.h"

// Note that the fizmo character conversion functions almost, but not quite,
// convert into UTF-8.  In particular, zucs_string_to_utf8_string() converts
// into the 1993 variation of UTF-8 which allows for length 5 and 6 encodings.
// This was removed from UTF-8 in 2003 (see RFC 3629).  Further, the single-
// character helper, zucs_char_to_latin1_char() simply lops off the high bits,
// and returns '?' for any multi-octet character.  In order to more-fully
// support modern output, we use system UTF-8 encodings.
std::wstring_convert<std::codecvt_utf8<wchar_t>> Span::utf8conv_;

Span::Span(const BlockBuf &buf, int line, int start, int end) {
    trace(2, "%p, %p, %d, %d, %d", this, &buf, line, start, end);

    format_ = buf.At(line, start);

    for (int i = start; i < end; i++) {
        str_ += buf.At(line, i).character;
    }
}

Span::Span(const struct blockbuf_char& bbch) {
    trace(2, "%p, %c", this, bbch.character);
    Append(bbch);
}

Span::Span(const std::string &str, const Format &format)
: format_(format), str_(str) {
    trace(2, "%p, \"%s\", %p", this, str.c_str(), &format);
    // format_ = format;
    // str_ = str;
}


bool Span::Append(const BlockBuf &buf, int line, int col) {
    trace(2, "%p, %d, %d, %d", &buf, line, col);
    return Append(buf.At(line, col));
}

bool Span::Append(const struct blockbuf_char& bbch) {
    trace(2, "%c", bbch.character);
    if (str_.empty()) {
        format_ = bbch;
    }

    if (format_ == bbch) {
        // str_ += bbch.character;
        // std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        str_ += utf8conv_.to_bytes((wchar_t)bbch.character);
        return true;
    }

    return false;
}

bool Span::Append(const std::string &str, const Format &format) {
    trace(2, "\"%s\"", str.c_str());

    if (format != format_) {
        return false;
    }

    str_ += str;
    return true;
}


json_t* Span::ToJson() const {
    json_t *obj = json_object();
    format_.AddJsonProps(obj);
    // json_string() requires a UTF-8 string...
    json_object_set_new(obj, "text", json_string(str_.c_str()));
    return obj;
}

// static
std::string Span::ConvertZucsString(const z_ucs *str) {
    return utf8conv_.to_bytes((const wchar_t *)str);
}

std::ostream & operator<<(std::ostream &os, const Span& span) {
    return os << "<span:(" << span.format_ << ")[" << span.str_ << "]>";
}
