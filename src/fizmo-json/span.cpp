// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "span.h"
#include "util.h"

Span::Span(const Span &span)
: format_(span.format_), str_(span.str_) {
    trace(2, "[%p] copy from %p", this, &span);
}


Span::Span(const BlockBuf &buf, int line, int start, int end) {
    trace(2, "[%p] %p, %d, %d, %d", this, &buf, line, start, end);

    format_ = buf.At(line, start);

    for (int i = start; i < end; i++) {
        str_ += buf.At(line, i).character;
    }
}

Span::Span(const struct blockbuf_char& bbch) {
    trace(2, "[%p] %c", this, bbch.character);
    Append(bbch);
}

Span::Span(const std::string &str, const Format &format)
: format_(format), str_(str) {
    trace(2, "[%p] \"%s\", %p", this, str.c_str(), &format);
}


bool Span::Append(const BlockBuf &buf, int line, int col) {
    trace(2, "[%p] %p, %d, %d, %d", this, &buf, line, col);
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
        str_ += ToUtf8(bbch.character);
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


std::ostream & operator<<(std::ostream &os, const Span& span) {
    return os << "<span:(" << span.format_ << ")[" << span.str_ << "]>";
}
