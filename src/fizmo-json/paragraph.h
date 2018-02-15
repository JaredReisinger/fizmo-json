// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_LINE_H
#define FIZMO_JSON_LINE_H

#include <iostream>
#include <list>

#include "span.h"
#include "format.h"
#include "blockbuf.h"


// A `Paragraph` represents contiguous `Span`s, up to a complete line.
class Paragraph {
  public:
    // Construct a Paragraph from a fizmo block buffer.  This will create zero or more
    // Spans, one for each change of format.
    Paragraph(const BlockBuf &buf, int line, int start, int end);
    Paragraph(const struct blockbuf_char& bbch);
    Paragraph(const std::string &str, const Format &format);

    // Appends a character from a BlockBuf as long as it doesn't start a new
    // line.
    bool Append(const BlockBuf &buf, int line, int col);
    bool Append(const struct blockbuf_char& bbch);
    void Append(const std::string &str, const Format &format);

    bool IsEmpty() const;

    json_t* ToJson() const;

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const Paragraph& run);

  private:
    std::list<Span> spans_;
};

#endif // FIZMO_JSON_LINE_H
