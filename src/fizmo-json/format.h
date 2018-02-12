// format.h
//
// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_FORMAT_H
#define FIZMO_JSON_FORMAT_H

#include <interpreter/blockbuf.h>
#include <tools/z_ucs.h>
#include <jansson.h>


typedef struct {
    // color and font are ignored for now...
    z_style style;
} format_info;

// const format_info DEFAULT_FORMAT = (format_info){ .style = Z_STYLE_ROMAN };
#define DEFAULT_FORMAT (format_info){ .style = Z_STYLE_ROMAN }

typedef struct {
    format_info format;
    int ch;
} formatted_char;

typedef struct formatted_text {
    format_info format;
    char *str;
    struct formatted_text *next;
} formatted_text;

// const formatted_char empty_char = (formatted_char){ };

extern struct blockbuf_char *blockbuf_char_at_yx(BLOCKBUF *bb, int ypos, int xpos);

extern bool format_equal(format_info a, format_info b);

extern bool blockbuf_format_equal(const struct blockbuf_char *a, const struct blockbuf_char *b);

extern formatted_text *new_text(const char *src, int start, int end, format_info format);

extern formatted_text *formatted_text_from_blockbuf(BLOCKBUF *buf, int line, int start, int end);

extern void free_formatted_text(formatted_text **text);

extern json_t *formatted_text_to_json(formatted_text *text);


#endif // FIZMO_JSON_FORMAT_H
