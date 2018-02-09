// buffer.c
//
// This file is part of fizmo-json.  Please see LICENSE.md for the license.

// jansson...
#include <jansson.h>

#include "screen.h"
#include "format.h"
#include "util.h"



// For now, we keep a very cheap array of lines, with a hard-coded upper bound.
formatted_text* bufferedWindow[SCREEN_HEIGHT] = { NULL };
int bufferedLines = 0;


void erase_buffered_window() {
    for (int l = 0; l < SCREEN_HEIGHT; l++) {
        free_formatted_text(bufferedWindow[l]);
    }
    memset(bufferedWindow, 0, sizeof(formatted_text*) * SCREEN_HEIGHT);
    bufferedLines = 0;
}

void append_buffered(char *src, int start, int end, format_info format, bool advance) {
    trace(3, "(src), %d, %d, %s", start, end, advance ? "true" : "false");

    if (end-start > 0) {
        if (bufferedWindow[bufferedLines] == NULL) {
            tracex(3, "starting new line");
            bufferedWindow[bufferedLines] = new_text(src, start, end, format);
        } else {
            formatted_text *cur = bufferedWindow[bufferedLines];
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
        bufferedLines++;
    }
}

void set_optional_bool(json_t *obj, char *name, bool value) {
    if (value) {
        json_object_set_new(obj, name, json_true());
    }
}

json_t* generate_buffered_json() {
    trace(1, "");

    // Collect story lines (buffered window)...
    json_t* lines = json_array();

    for (int l = 0; l < bufferedLines+1; l++) {
        json_t* line;

        formatted_text *text = bufferedWindow[l];

        if (!text) {
            line = json_null();
        } else {
            line = json_array();

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
        }

        json_array_append_new(lines, line);
    }

    return lines;
}


void dump_formatted_text(formatted_text *text) {
    if (text == NULL) {
        return;
    }
    fprintf(stderr, "\e[38;5;70m%s%s%s%s\e[0m",
        text->format.style & Z_STYLE_REVERSE_VIDEO ? "\e[7m" : "",
        text->format.style & Z_STYLE_BOLD ? "\e[1m" : "",
        text->format.style & Z_STYLE_ITALIC ? "\e[4m" : "",
        text->str);
    dump_formatted_text(text->next);
}


void dump_buffered() {
    fprintf(stderr, "\e[38;5;12mbuffered window (%d-ish lines):\e[0m\n", bufferedLines);
    // +1 for any trailing line (NULL is ignored)...
    for (int l = 0; l < bufferedLines+1; l++) {
        dump_formatted_text(bufferedWindow[l]);
        fprintf(stderr, "\n");
    }
}
