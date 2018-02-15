// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "buffer.h"

extern "C" {
    #include <interpreter/fizmo.h>
    // #include <jansson.h>
}

#include "screen.h"
#include "format.h"
#include "util.h"


Buffer::Buffer() {
    trace(2, "%p", this);
    lastParagraphOpen_ = false;
}

void Buffer::Empty() {
    trace(2, "%p", this);
    paragraphs_.clear();
    lastParagraphOpen_ = false;
}

void Buffer::Append(const struct blockbuf_char& bbch) {
    trace(2, "%c", bbch.character);

    Paragraph *last = paragraphs_.empty() ? NULL : &paragraphs_.back();

     if (last == NULL || !lastParagraphOpen_) {
        paragraphs_.emplace_back(bbch);
        lastParagraphOpen_ = true;
    } else if (!last->Append(bbch)) {
        lastParagraphOpen_ = false;
    }
}

void Buffer::Append(const std::string &str, const Format &format) {
    trace(2, "\"%s\"", str.c_str());

    if (str.empty()) {
        return;
    }

    // split into lines/paragraphs before appending...
    int start = 0;
    int end;
    while (start < str.length() && (end = str.find('\n', start)) != -1) {
        tracex(3, "found paragraph: %d-%d", start, end);
        AppendSafe(str.substr(start, end-start), format, false);
        start = end+1;
    }

    end = str.length();
    if (start < end) {
        tracex(3, "remainder from %d-%d", start, end);
        AppendSafe(str.substr(start, end-start), format, true);
    }
}

// This is the internal version of Append... we *don't* check for newlines, and
// just use the entire string...  (Should we lift the newline handling out to
// the caller?)
void Buffer::AppendSafe(const std::string &str, const Format &format, bool leaveParagraphOpen) {
    trace(2, "\"%s\"", str.c_str());

    if (lastParagraphOpen_ && !paragraphs_.empty()) {
        paragraphs_.back().Append(str, format);
    } else {
        paragraphs_.emplace_back(str, format);
    }

    tracex(3, "********");
    lastParagraphOpen_ = leaveParagraphOpen;
}

void Buffer::EndParagraph() {
    lastParagraphOpen_ = false;
}


void Buffer::Prepend(const Buffer &buffer) {
    trace(2, "%p, %p", this, &buffer);
    for (auto it = buffer.paragraphs_.crbegin(); it != buffer.paragraphs_.crend(); it++) {
        paragraphs_.push_front(*it);
    }
}

json_t* Buffer::ToJson(bool skipLeadingBlanks, bool omitPrompt) const {
    json_t *obj = json_array();

    if (paragraphs_.empty()) {
        return obj;
    }

    auto start = paragraphs_.cbegin();
    auto end = paragraphs_.cend();

    if (skipLeadingBlanks) {
        while (start->IsEmpty() && start != end) {
            ++start;
        }
    }

    if (end != start && omitPrompt && lastParagraphOpen_) {
        --end;      // pointing at last, open paragraph
        if (end != start) {
            --end;  // pointing to the one before that
        }
    }

    while (end != start && end->IsEmpty()) {
        --end;
    }

    if (end != paragraphs_.cend()) {
        ++end;
    }

    for (auto p = start; p != end; ++p) {
        json_array_append_new(obj, p->ToJson());
    }

    return obj;
}

std::ostream & operator<<(std::ostream &os, const Buffer& buffer) {
    os << "<buffer:\n";
    for (const auto &paragraph : buffer.paragraphs_) {
        os << "  "
            << paragraph
            << "\n";
    }
    os << "last paragraph "
        << (buffer.lastParagraphOpen_ ? "OPEN" : "CLOSED")
        << ">\n";
    return os;
}
