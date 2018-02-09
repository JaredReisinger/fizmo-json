// buffer.h
//
// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_BUFFER_H
#define FIZMO_JSON_BUFFER_H

// jansson...
#include <jansson.h>

#include "format.h"

typedef struct {} LINEBUF;

extern LINEBUF *create_line_buffer();
extern void free_line_buffer(LINEBUF **buf);

extern void line_buffer_append(LINEBUF *buf, const char *src, int start, int end, format_info format, bool advance);

extern void line_buffer_prepend_and_free(LINEBUF* buf, LINEBUF** prepend);

extern json_t* line_buffer_generate_json(LINEBUF* buf);


#endif // FIZMO_JSON_BUFFER_H
