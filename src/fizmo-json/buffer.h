// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_BUFFER_H
#define FIZMO_JSON_BUFFER_H

#include <iostream>
#include <list>

extern "C" {
    // jansson...
    #include <jansson.h>
}

#include "paragraph.h"
#include "format.h"

// typedef struct {} LINEBUF;
//
// extern LINEBUF *create_line_buffer();
// extern void free_line_buffer(LINEBUF **buf);
//
// extern void line_buffer_append(LINEBUF *buf, const char *src, int start, int end, format_info format, bool advance);
// extern void line_buffer_append_text(LINEBUF *buf, formatted_text **text, bool advance);
//
// extern void line_buffer_prepend_and_free(LINEBUF* buf, LINEBUF** prepend);
//
// extern json_t* line_buffer_to_json(LINEBUF* buf, bool avoidPrompt);



class Buffer {
  public:
    Buffer();

    void Empty();

    void Append(const struct blockbuf_char& bbch);
    void Append(const std::string &str, const Format &format);
    void EndParagraph();

    void Prepend(const Buffer &buffer);

    // void Add(const Paragraph &paragraph);

    json_t* ToJson(bool skipLeadingBlanks, bool omitPrompt) const;

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const Buffer& buffer);


  private:
    void AppendSafe(const std::string &str, const Format &format, bool leaveParagraphOpen);

    std::list<Paragraph>    paragraphs_;
    bool                    lastParagraphOpen_;
};


#endif // FIZMO_JSON_BUFFER_H
