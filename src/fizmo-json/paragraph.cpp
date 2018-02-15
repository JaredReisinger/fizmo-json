// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "paragraph.h"
#include "util.h"

Paragraph::Paragraph(const BlockBuf &buf, int line, int start, int end) {
    trace(2, "%p, %p, %d, %d, %d", this, &buf, line, start, end);

    z_style style = buf.At(line, start).style;
    tracex(3, "initial style: %d", style);
    int spanStart = start;

    for (int i = start; i < end; i++) {
        if (buf.At(line, i).style != style) {
            // Span span(buf, line, spanStart, i);
            spans_.push_back(Span(buf, line, spanStart, i));
            spanStart = i;
            style = buf.At(line, spanStart).style;
            tracex(3, "new style: %d", style);
        }
        // str_ += buf.At(line, i).character;
    }

    spans_.push_back(Span(buf, line, spanStart, end));
}

Paragraph::Paragraph(const struct blockbuf_char& bbch) {
    trace(2, "%p, %c", this, bbch.character);
    Append(bbch);
}

Paragraph::Paragraph(const std::string &str, const Format &format) {
    trace(2, "%p, \"%s\"", this, str.c_str());
    if (!str.empty()) {
        spans_.push_back(Span(str, format));
    }
}


bool Paragraph::Append(const BlockBuf &buf, int line, int col) {
    trace(2, "%p, %d, %d, %d", &buf, line, col);
    return Append(buf.At(line, col));
}

bool Paragraph::Append(const struct blockbuf_char& bbch) {
    trace(2, "%c", bbch.character);

    if (bbch.character == Z_UCS_NEWLINE) {
        return false;
    }

    Span *last = spans_.empty() ? NULL : &spans_.back();

    if (last == NULL || !last->Append(bbch)) {
        spans_.push_back(Span(bbch));
    }

    return true;
}


void Paragraph::Append(const std::string &str, const Format &format) {
    if (str.empty()) {
        return;
    }

    if (spans_.empty() || !spans_.back().Append(str, format)) {
        spans_.push_back(Span(str, format));
    }
}

bool Paragraph::IsEmpty() const {
    return spans_.empty();
}


json_t* Paragraph::ToJson() const {
    // if (spans_.empty()) {
    //     return json_null();
    // }

    json_t *obj = json_array();

    for (const auto &s : spans_) {
        json_array_append_new(obj, s.ToJson());
    }

    return obj;
}



std::ostream & operator<<(std::ostream &os, const Paragraph& run) {
    os << "<para:";
    for (const auto &s : run.spans_) {
        os << s;
    }
    os << ">";
    return os;
}
