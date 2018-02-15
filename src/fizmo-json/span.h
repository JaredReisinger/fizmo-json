// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_SPAN_H
#define FIZMO_JSON_SPAN_H

#include <list>
#include <ostream>
#include <string>

extern "C" {
    #include <jansson.h>
}

#include "blockbuf.h"
#include "format.h"

// A `Span` is a contiguous flow of text that has a single format.  This is the
// smallest "interesting" piece of text we will ever care about.
class Span {
  public:
    Span(const Span &span);

    // Construct a Span from a fizmo block buffer.  Note that this will *not*
    // validate that the format stays constant; it uses the format at `start`,
    // and assumes it holds until `end`.
    Span(const BlockBuf &buf, int line, int start, int end);
    Span(const struct blockbuf_char& bbch);
    Span(const std::string &str, const Format &format);

    // Appends a character from a BlockBuf as long as the format matches.
    bool Append(const BlockBuf &buf, int line, int col);
    bool Append(const struct blockbuf_char& bbch);
    bool Append(const std::string &str, const Format &format);

    json_t* ToJson() const;

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const Span& span);

  private:
    Format      format_;
    std::string str_;   // UTF-8-encoded string
};

#endif // FIZMO_JSON_SPAN_H
