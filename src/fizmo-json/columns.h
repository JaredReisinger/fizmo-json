// buffer.h
//
// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_COLUMNS_H
#define FIZMO_JSON_COLUMNS_H

// jansson...
#include <jansson.h>

typedef struct {} COLUMNBUF;

extern COLUMNBUF *create_column_buffer();
extern void free_column_buffer(COLUMNBUF **buf);

extern void column_buffer_add_column(COLUMNBUF *buf, int c, BLOCKBUF *bb, int l, int start, int end);

extern json_t* column_buffer_to_json(COLUMNBUF *buf);

#endif // FIZMO_JSON_COLUMNS_H
