// format.c
//
// This file is part of fizmo-bot.  Please see LICENSE.md for the license.

#include <stdlib.h>
#include <interpreter/fizmo.h>

// const formatted_char empty_char = (formatted_char){ };

#include "format.h"

bool format_equal(format_info a, format_info b) {
    return a.style == b.style;
}

bool blockbuf_format_equal(const struct blockbuf_char *a, const struct blockbuf_char *b) {
    // we only care about style for now...
    return a->style == b->style;
}


formatted_text *new_text(char *src, int start, int end, format_info format) {
    formatted_text *text = fizmo_malloc(sizeof(formatted_text));
    memset(text, 0, sizeof(formatted_text));
    text->format = format;
    text->str = strndup(&src[start], end-start);
    return text;
}

void free_formatted_text(formatted_text *text) {
    if (text == NULL) {
        return;
    }

    free_formatted_text(text->next);

    // From the man page for free(3): "If ptr is a NULL pointer, no
    // operation is performed".
    free(text->str);
    free(text);
}
