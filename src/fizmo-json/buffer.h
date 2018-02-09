// buffer.h
//
// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_BUFFER_H
#define FIZMO_JSON_BUFFER_H

// jansson...
#include <jansson.h>

#include "format.h"

// Because of the interaction between the story and status windows (buffered
// and unbuffered areas), we manage them *both* in buffer.h/buffer.c.

extern void erase_buffered_window();
extern void append_buffered(char *src, int start, int end, format_info format, bool advance);
extern json_t* generate_buffered_json();

extern void dump_buffered();


// ... also... we don't need to manage the unbuffered part, since fizmo has
// BLOCKBUF which we can leverage...

#endif // FIZMO_JSON_BUFFER_H
