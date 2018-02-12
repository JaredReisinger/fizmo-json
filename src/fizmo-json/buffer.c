// buffer.c
//
// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include <interpreter/fizmo.h>
#include <jansson.h>

#include "screen.h"
#include "buffer.h"
#include "format.h"
#include "util.h"


typedef struct {
    int capacity;
    int nextLine;
    formatted_text **lines;
} line_buffer;


LINEBUF *create_line_buffer() {
    trace(2, "");
    line_buffer *lb = fizmo_malloc(sizeof(line_buffer));
    // memset(lb, 0, sizeof(line_buffer));
    *lb = (line_buffer){ .capacity = 0, .nextLine = 0 };
    tracex(2, "created %p", lb);
    return (LINEBUF*)lb;
}

void free_line_buffer(LINEBUF **buf) {
    trace(2, "%p", *buf);

    if (buf == NULL || *buf == NULL) {
        return;
    }

    line_buffer *lb = (line_buffer *)(*buf);

    for (int l = 0; l < lb->capacity; l++) {
        free_formatted_text(&(lb->lines[l]));
    }

    free(lb->lines);
    free(lb);
    *buf = NULL;
}

void grow_line_buffer(line_buffer *lb) {
    trace(2, "%p", lb);
    int capacity = lb->capacity * 2;
    if (capacity == 0) {
        capacity = 10;
    }

    formatted_text **lines = fizmo_realloc(lb->lines, sizeof(formatted_text *) * capacity);
    if (lines != NULL) {
        memset(&lines[lb->capacity], 0, sizeof(formatted_text *) * (capacity - lb->capacity));
        lb->lines = lines;
        lb->capacity = capacity;
    }
}

void line_buffer_append(LINEBUF *buf, const char *src, int start, int end, format_info format, bool advance) {
    trace(2, "%p, (src), %d, %d, %s", buf, start, end, advance ? "true" : "false");

    line_buffer *lb = (line_buffer *)buf;

    while (lb->nextLine >= lb->capacity) {
        grow_line_buffer(lb);
    }

    if (end-start > 0) {
        if (lb->lines[lb->nextLine] == NULL) {
            tracex(3, "starting new line");
            lb->lines[lb->nextLine] = new_text(src, start, end, format);
        } else {
            formatted_text *cur = lb->lines[lb->nextLine];
            tracex(3, "appending to existing line (%p)", cur);
            while (cur->next != NULL) {
                cur = cur->next;
                tracex(3, "walking... (%p)", cur);
            }

            // We'll either add a new formatted_text as 'next', or, if the
            // formatting hasn't changed, simply append it to the current one.
            if (format_equal(format, cur->format)) {
                char *str;
                asprintf(&str, "%s%.*s", cur->str, end-start, &src[start]);
                free(cur->str);
                cur->str = str;
            } else {
                cur->next = new_text(src, start, end, format);
            }
        }
    }

    if (advance) {
        tracex(3, "advancing line");
        lb->nextLine++;
    }
}

void line_buffer_append_text(LINEBUF *buf, formatted_text **text, bool advance) {
    trace(2, "%p, %p, %s", buf, *text, advance ? "true" : "false");

    line_buffer *lb = (line_buffer *)buf;

    while (lb->nextLine >= lb->capacity) {
        grow_line_buffer(lb);
    }

    if (*text) {
        if (lb->lines[lb->nextLine] == NULL) {
            tracex(3, "starting new line");
            lb->lines[lb->nextLine] = *text;
            *text = NULL;
        } else {
            formatted_text *cur = lb->lines[lb->nextLine];
            tracex(3, "appending to existing line (%p)", cur);
            while (cur->next != NULL) {
                cur = cur->next;
                tracex(3, "walking... (%p)", cur);
            }

            // We'll either add a new formatted_text as 'next', or, if the
            // formatting hasn't changed, simply append it to the current one.
            if (format_equal((*text)->format, cur->format)) {
                char *str;
                asprintf(&str, "%s%s", cur->str, (*text)->str);
                free(cur->str);
                free_formatted_text(text);
                cur->str = str;
            } else {
                cur->next = *text;
                *text = NULL;
            }
        }
    }

    if (advance) {
        tracex(3, "advancing line");
        lb->nextLine++;
    }
}

void line_buffer_prepend_and_free(LINEBUF* buf, LINEBUF** prepend) {
    trace(1, "%p, %p", buf, *prepend);
    line_buffer *lb = (line_buffer *)buf;
    line_buffer *pre = (line_buffer *)(*prepend);

    tracex(3, "%p (%d / %d) <- %p (%d / %d)", lb, lb->nextLine, lb->capacity, pre, pre->nextLine, pre->capacity);

    int shift = pre->nextLine;
    if (shift < pre->capacity && pre->lines[shift] != NULL) {
        tracex(3, "increasing shift!");
        shift++;
    }

    tracex(2, "prepending %d lines to existing %d...", shift, lb->nextLine);

    size_t existing_size = sizeof(formatted_text *) * lb->capacity;
    size_t shift_size = sizeof(formatted_text *) * shift;

    while (lb->capacity <= shift + lb->nextLine) {
        grow_line_buffer(lb);
    }

    // shift existing lines down
    tracex(3, "shifting existing: %p by %d", lb, shift);
    memmove(&lb->lines[shift], &lb->lines[0], existing_size);
    memset(&lb->lines[0], 0, shift_size);
    lb->nextLine += shift;

    // copy lines from "pre", and free it
    tracex(3, "prepending: %p (length %d) -> %p", pre, shift, lb);
    memcpy(&lb->lines[0], &pre->lines[0], shift_size);
    memset(&pre->lines[0], 0, shift_size);
    free_line_buffer(prepend);
    tracex(3, "prepend buffer is now %p", *prepend);
}


json_t* line_buffer_to_json(LINEBUF* buf, bool avoidPrompt) {
    trace(1, "%p", buf);

    line_buffer *lb = (line_buffer *)buf;
    tracex(2, "linebuf %p capacity: %d, %d(?)", lb, lb->capacity, lb->nextLine);

    json_t* lines = json_array();

    int last = lb->nextLine;
    if (avoidPrompt) {
        if (lb->lines[last] != NULL) {
            last--;
        }

        while (lb->lines[last] == NULL) {
            last--;
        }

        last++;
    } else {
        if (lb->lines[last] != NULL) {
            last++;
        }
    }

    for (int l = 0; l < last; l++) {
        json_array_append_new(lines, formatted_text_to_json(lb->lines[l]));
    }

    return lines;
}
