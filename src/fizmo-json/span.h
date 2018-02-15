// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_SPAN_H
#define FIZMO_JSON_SPAN_H


#include <codecvt>
#include <iostream>
#include <list>
#include <string>

extern "C" {
    #include <interpreter/blockbuf.h>
    #include <jansson.h>
}

#include "blockbuf.h"
#include "format.h"

// A `Span` is a contiguous flow of text that has a single format.  This is the
// smallest "interesting" piece of text we will ever care about.
class Span {
  public:
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

    static std::string ConvertZucsString(const z_ucs *str);

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const Span& span);

  private:
    Format      format_;
    std::string str_;   // UTF-8-encoded string

    static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv_;
};

#endif // FIZMO_JSON_SPAN_H
