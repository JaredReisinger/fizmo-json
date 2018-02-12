// format.c
//
// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include <stdlib.h>
#include <interpreter/fizmo.h>
#include <jansson.h>

// const formatted_char empty_char = (formatted_char){ };

#include "format.h"
#include "util.h"

bool format_equal(format_info a, format_info b) {
    return a.style == b.style;
}

bool blockbuf_format_equal(const struct blockbuf_char *a, const struct blockbuf_char *b) {
    // we only care about style for now...
    return a->style == b->style;
}

struct blockbuf_char *blockbuf_char_at_yx(BLOCKBUF *bb, int ypos, int xpos) {
    return &bb->content[(ypos * bb->width) + xpos];
}


formatted_text *new_text(const char *src, int start, int end, format_info format) {
    trace(3, "");
    formatted_text *text = fizmo_malloc(sizeof(formatted_text));
    memset(text, 0, sizeof(formatted_text));
    text->format = format;
    text->str = strndup(&src[start], end-start);
    return text;
}

void free_formatted_text(formatted_text **text) {
    trace(3, "%p", *text);

    if (text == NULL || *text == NULL) {
        return;
    }

    free_formatted_text(&((*text)->next));

    // From the man page for free(3): "If ptr is a NULL pointer, no
    // operation is performed".
    tracex(3, "freeing string %p", (*text)->str);
    free((*text)->str);
    tracex(3, "freeing formatted_text %p", *text);
    free(*text);
    *text = NULL;
}

// create formatted text run from fizmo's BLOCKBUF.
formatted_text *formatted_text_from_blockbuf(BLOCKBUF *buf, int line, int start, int end) {
    trace(2, "%p, %d, %d, %d", buf, line, start, end);
    formatted_text *result = NULL;
    formatted_text *prev = NULL;
    char strBuf[end-start + 1];
    int len = 0;
    struct blockbuf_char *bbchFormat = blockbuf_char_at_yx(buf, line, start);

    for (int c = start; c < end; c++) {
        struct blockbuf_char *bbch = blockbuf_char_at_yx(buf, line, c);

        if (blockbuf_format_equal(bbch, bbchFormat)) {
            strBuf[len++] = zucs_char_to_latin1_char(bbch->character);
        } else {
            // format boundary!
            strBuf[len] = '\0';
            // tracex(1, "appending line %d (1) \"%s\"", l, strBuf);
            formatted_text *text = new_text(strBuf, 0, len, (format_info){ .style = bbchFormat->style});

            if (!result) {
                result = text;
            } else {
                prev->next = text;
            }

            prev = text;

            strBuf[0] = zucs_char_to_latin1_char(bbch->character);
            len = 1;
            bbchFormat = bbch;
        }

    }

    // any trailing text...
    strBuf[len] = '\0';
    // tracex(1, "appending line %d (2) \"%s\"", l, buf);
    formatted_text *text = new_text(strBuf, 0, len, (format_info){ .style = bbchFormat->style});
    if (!result) {
        result = text;
    } else {
        prev->next = text;
    }
    // tracex(1, "finished line: %d", l);

    return result;
}

void set_optional_bool(json_t *obj, char *name, bool value) {
    if (value) {
        json_object_set_new(obj, name, json_true());
    }
}


json_t *formatted_text_to_json(formatted_text *text) {
    if (!text) {
        return json_null();
    }

    json_t *line = json_array();

    for (formatted_text *cur = text; cur != NULL; cur = cur->next) {
        json_t *span = json_object();
        z_style style = cur->format.style;
        // json_object_set_new(span, "style", json_integer(style));
        set_optional_bool(span, "reverse", style & Z_STYLE_REVERSE_VIDEO);
        set_optional_bool(span, "bold", style & Z_STYLE_BOLD);
        set_optional_bool(span, "italic", style & Z_STYLE_ITALIC);
        set_optional_bool(span, "fixed", style & Z_STYLE_FIXED_PITCH);
        json_object_set_new(span, "text", json_string(cur->str));
        json_array_append_new(line, span);
    }

    return line;
}
