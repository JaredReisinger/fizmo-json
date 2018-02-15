// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_BUFFER_H
#define FIZMO_JSON_BUFFER_H

#include <iostream>
#include <list>

extern "C" {
    #include <jansson.h>
}

#include "paragraph.h"
#include "format.h"


class Buffer {
  public:
    Buffer();

    void Empty();

    void Append(const struct blockbuf_char& bbch);
    void Append(const std::string &str, const Format &format);
    void EndParagraph();

    void Prepend(const Buffer &buffer);

    json_t* ToJson(bool skipLeadingBlanks = false, bool omitPrompt = false) const;

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const Buffer& buffer);


  private:
    void AppendSafe(const std::string &str, const Format &format, bool leaveParagraphOpen);

    std::list<Paragraph>    paragraphs_;
    bool                    lastParagraphOpen_;
};


#endif // FIZMO_JSON_BUFFER_H
