// columns.c
//
// This file is part of fizmo-json.  Please see LICENSE.md for the license.

extern "C" {
    #include <interpreter/fizmo.h>
    #include <jansson.h>
}

#include "screen.h"
#include "columns.h"
#include "format.h"
#include "util.h"


// A list of in-order columns, each of which contains per-line info...

typedef struct column_line {
    //int column; // redundant?
    int line;
    formatted_text *text;
    struct column_line *next;
} column_line;

typedef struct {
    int column;
    column_line *first;
} column_info;

typedef struct {
    int size;
    column_info *columns;   // note: *not* '**'!
} column_buffer;


COLUMNBUF *create_column_buffer() {
    trace(2, "");
    column_buffer *cb = (column_buffer *)fizmo_malloc(sizeof(column_buffer));
    // memset(lb, 0, sizeof(line_buffer));
    *cb = (column_buffer){ .size = 0, .columns = NULL };
    tracex(2, "created %p", cb);
    return (COLUMNBUF*)cb;
}

void free_column_line(column_line **cl) {
    trace(2, "%p", *cl);
    if (*cl == NULL) {
        return;
    }

    free_column_line(&((*cl)->next));

    free_formatted_text(&((*cl)->text));
    free(*cl);
    *cl = NULL;
}


void free_column_buffer(COLUMNBUF **buf) {
    trace(2, "%p", *buf);

    if (buf == NULL || *buf == NULL) {
        return;
    }

    column_buffer *cb = (column_buffer *)(*buf);

    for (int c = 0; c < cb->size; c++) {
        free_column_line(&(cb->columns[c].first));
    }

    free(cb->columns);
    free(cb);
    *buf = NULL;
}

// finds the index of the column, if it exists.  If not, returns a negative
// number to indicate what the index *should* be (-index - 1).  To get the
// expected index, take the result, add 1, and negate (or, negate, and then
// subtract 1).  This means -1 => 0, -2 => 1, etc.
int find_column_index(column_buffer *cb, int c) {
    trace(2, "%p, %d", cb, c);
    if (cb->size == 0) {
        return -1;
    }

    for (int i = 0; i < cb->size; i++) {
        if (cb->columns[i].column == c) {
            return i;
        }
        if (cb->columns[i].column > c) {
            return -i - 1;
        }
    }

    return -cb->size - 1;
}

column_info *ensure_column(column_buffer *cb, int c) {
    trace(2, "%p, %d", cb, c);
    int index = find_column_index(cb, c);
    tracex(2, "got index %d for column %d", index, c);

    if (index < 0) {
        // need to add a column (and shift...)
        index = -index - 1;
        tracex(2, "attempting to create index %d for column %d, new size %d", index, c, cb->size+1);
        column_info *columns = (column_info *)fizmo_realloc(cb->columns, sizeof(column_info) * (cb->size+1));
        if (columns == NULL) {
            tracex(1, "out of memory!");
            return NULL;
        }
        tracex(2, "realloc complete...");
        cb->columns = columns;
        cb->size++;
        if (index < cb->size-1) {
            tracex(2, "moving existing columns (index %d to %d)...", index, cb->size-1);
            memmove(&cb->columns[index+1], &cb->columns[index], sizeof(column_info) * (cb->size - index));
        }
        cb->columns[index].column = c;
        cb->columns[index].first = NULL;
    }

    return &(cb->columns[index]);
}

void column_info_add_line(column_info *info, int line, formatted_text *text) {
    trace(1, "%p, %d, %p", info, line, text);
    column_line *cl = (column_line *)fizmo_malloc(sizeof(column_line));
    cl->line = line;
    cl->text = text;
    cl->next = NULL;

    column_line **cur = &(info->first);
    while (*cur != NULL) {
        cur = &((*cur)->next);
    }

    *cur = cl;
}

void column_buffer_add_column(COLUMNBUF *buf, int c, BLOCKBUF *bb, int l, int start, int end) {
    trace(1, "%p, %d, %p, %d, %d, %d", buf, c, bb, l, start, end);
    column_buffer *cb = (column_buffer *)buf;

    // see if we already have the column...
    column_info *info = ensure_column(cb, c);

    // create formatted_text from a series of blockbuf_chars...
    formatted_text *text = formatted_text_from_blockbuf(bb, l, start, end);

    column_info_add_line(info, l, text);
}

json_t* column_buffer_to_json(COLUMNBUF *buf) {
    trace(1, "%p", buf);
    column_buffer *cb = (column_buffer *)buf;

    json_t* list = json_array();

    for (int i = 0; i < cb->size; i++) {
        column_info *info = &cb->columns[i];
        tracex(1, "column[%d]: %d, %p", i, info->column, info->first);

        json_t* col = json_object();
        json_object_set_new(col, "column", json_integer(info->column));

        json_t* lines = json_array();
        for(column_line *cur = info->first; cur != NULL; cur = cur->next) {
            tracex(1, "line: %d", i, cur->line);
            json_t* line = json_object();
            json_object_set_new(line, "line", json_integer(cur->line));
            json_object_set_new(line, "text", formatted_text_to_json(cur->text));
            json_array_append_new(lines, line);
        }

        json_object_set_new(col, "lines", lines);

        json_array_append_new(list, col);
    }

    return list;
}
