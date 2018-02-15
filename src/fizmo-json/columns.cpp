// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "columns.h"
#include "util.h"


Columns::Columns() {
    trace(2, "[%p]", this);
}

Columns::Columns(const BlockBuf &buf) {
    trace(2, "[%p] %p", this, &buf);
    Infer(buf);
}

json_t *Columns::ToJson() const {
    json_t *obj = json_array();
    for (auto i : infos_) {
        json_array_append_new(obj, i.ToJson());
    }
    return obj;
}

void Columns::Infer(const BlockBuf &buf) {
    trace(2, "[%p] %p", this, &buf);

    // Run through the lines, looking for space-separated text.
    const int lines = buf.Height();
    const int cols = buf.Width();

    for (int l = 0; l < lines; ++l) {
        tracex(2, "looking at line %d", l);
        int lastTextStart = -1;
        int spacesSeen = 0;

        for (int c = 0; c < cols; ++c) {
            auto bbch = buf.At(l, c);
            if (bbch.character == Z_UCS_SPACE) {
                if (++spacesSeen >= 3 && lastTextStart >= 0) {
                    const int end = c - spacesSeen + 1;
                    tracex(2, "line %d, text start at %d ended at %d", l, lastTextStart, end);
                    // add column...
                    AddColumnText(buf, l, lastTextStart, end);
                    lastTextStart = -1;
                }
            } else {
                if (lastTextStart < 0) {
                    tracex(2, "line %d, text start at: %d", l, c);
                    lastTextStart = c;
                }
                spacesSeen = 0;
            }
        }
    }
}

ColumnInfo *Columns::EnsureColumn(int column) {
    trace(2, "[%p] %d", this, column);
    auto cur = infos_.begin();
    for (; cur != infos_.end() && cur->Column() <= column; ++cur) {
        if (cur->Column() == column) {
            return &(*cur);
        }
    }

    return &(*infos_.emplace(cur, column));
}

void Columns::AddColumnText(const BlockBuf &buf, int line, int start, int end) {
    trace(2, "[%p] %p, %d, %d, %d", this, &buf, line, start, end);
    auto col = EnsureColumn(start);
    col->AddLine(buf, line, start, end);
}

std::ostream & operator<<(std::ostream &os, const Columns& columns) {
    os << "<columns:\n";
    for (const auto &col : columns.infos_) {
        os << col;
    }
    os << ">\n";
    return os;
}


ColumnInfo::ColumnInfo(int column) {
    trace(2, "[%p] %d", this, column);
    column_ = column;
}

ColumnInfo::ColumnInfo(const ColumnInfo &info) {
    trace(2, "[%p] copy from %p", this, &info);
    column_ = info.column_;
    infos_ = info.infos_;
}

ColumnInfo::~ColumnInfo() {
    trace(2, "[%p] BOOM", this);
}


int ColumnInfo::Column() const {
    return column_;
}

void ColumnInfo::AddLine(const BlockBuf &buf, int line, int start, int end) {
    trace(2, "[%p] %p, %d, %d, %d", this, &buf, line, start, end);
    infos_.emplace_back(buf, line, start, end);
    // std::cerr << *this << "\n";
}

json_t *ColumnInfo::ToJson() const {
    json_t *obj = json_object();
    json_object_set_new(obj, "column", json_integer(column_));

    json_t *array = json_array();
    for (auto i : infos_) {
        json_array_append_new(array, i.ToJson());
    }
    json_object_set_new(obj, "lines", array);
    return obj;
}

std::ostream & operator<<(std::ostream &os, const ColumnInfo& info) {
    os << "  <col:" << info.column_ << "\n";
    for (const auto &line : info.infos_) {
        os << line;
    }
    os << "  >\n";
    return os;
}

LineInfo::LineInfo(const BlockBuf &buf, int line, int start, int end)
: text_(buf, line, start, end) {
    trace(2, "[%p] %p, %d, %d, %d", this, &buf, line, start, end);
    line_ = line;
}

LineInfo::LineInfo(const LineInfo &info) {
    trace(2, "[%p] copy from %p", this, &info);
    line_ = info.line_;
    text_ = info.text_;
}

LineInfo::~LineInfo() {
    trace(2, "[%p] BOOM", this);
}


json_t *LineInfo::ToJson() const {
    json_t *obj = json_object();
    json_object_set_new(obj, "line", json_integer(line_));
    json_object_set_new(obj, "text", text_.ToJson());
    return obj;
}

std::ostream & operator<<(std::ostream &os, const LineInfo& info) {
    os << "    <line:" << info.line_ << info.text_ << ">\n";
    return os;
}
