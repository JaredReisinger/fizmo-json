// screen.c
//
// This file is part of fizmo-bot.  Please see LICENSE.md for the license.

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>

#include "config.h"
#include "util.h"

// fizmo includes...
#include <interpreter/fizmo.h>
#include <tools/z_ucs.h>

// jansson...
#include <jansson.h>


// The standard Z-machine, Glk, and fizmo models of the screen all assume a
// fixed-size, rectangular buffer... but while that's useful for the status
// window, it actually complicates the regular, buffered story window.  To that
// end, we track the buffered and unbuffered variations completely separately.
//
// The buffered text is treated much like HTML, with block (line), and span
// flow.  Its content is *never* placed into the unbuffered grid, under the
// assumption that no game would explicitly set the cursor into the midst of a
// buffered run in order to update the text.
//
// A full-screen-sized unbuffered window is tracked purely for the purposes of
// capturing the status line(s), and/or any explicitly layed-out text (like the
// quote in the beginning of Graham Nelson's "Curses").  We use a screen size
// of 100x255: 100 characters wide allows us to easily infer left/center/right
// alignment, and 255 lines is, as per the Z-machine spec section 8.4.1,
// "infinte height".
const int SCREEN_WIDTH  = 100;
const int SCREEN_HEIGHT = 255;

typedef struct {
    // color and font are ignored for now...
    z_style style;
} format_info;

typedef struct {
    format_info format;
    int ch;
} formatted_char;

typedef struct formatted_text {
    format_info format;
    char *str;
    struct formatted_text *next;
} formatted_text;

const formatted_char empty_char = (formatted_char){ };

format_info currentFormat = (format_info){
    .style = Z_STYLE_ROMAN,
};

formatted_char unbufferedWindow[SCREEN_HEIGHT][SCREEN_WIDTH];

int unbufferedLines = 0;
int unbufferedLine = 0;
int unbufferedCol = 0;
bool unbufferedTouched = false;

// For now, we keep a very cheap array of lines, with a hard-coded upper bound.
formatted_text* bufferedWindow[SCREEN_HEIGHT] = { NULL };
int bufferedLines = 0;

const int STORY_WINDOW = 0;
const int STATUS_WINDOW = 1;

int currentWindow = STORY_WINDOW;

void partial_erase_unbuffered_window(int as_of_line) {
    // We have conveniently defined formatted_char such that a zero-filled
    // structure is the default "empty" value, so we don't have to loop to erase
    // the window... we just zero it out.
    memset(&unbufferedWindow[as_of_line], 0, sizeof(formatted_char) * (SCREEN_HEIGHT - as_of_line) * SCREEN_WIDTH);
}

void erase_unbuffered_window() {
    // // We have conveniently defined formatted_char such that a zero-filled
    // // structure is the default "empty" value, so we don't have to loop to erase
    // // the window... we just zero it out.
    // memset(unbufferedWindow, 0, sizeof(formatted_char) * SCREEN_HEIGHT * SCREEN_WIDTH);
    partial_erase_unbuffered_window(0);
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

void erase_buffered_window() {
    for (int l = 0; l < SCREEN_HEIGHT; l++) {
        free_formatted_text(bufferedWindow[l]);
    }
    memset(bufferedWindow, 0, sizeof(formatted_text*) * SCREEN_HEIGHT);
    bufferedLines = 0;
}

void dump_unbuffered() {
    fprintf(stderr, "\e[38;5;81munbuffered window (%d lines):\e[0m\n", unbufferedLines);

    int escapedLength = (SCREEN_WIDTH * 100) + 1;
    char line[escapedLength];
    char escaped[SCREEN_WIDTH * 100];

    for (int l = 0; l < unbufferedLines; l++) {
        memset(line, 0, escapedLength);

        for (int c = 0; c < SCREEN_WIDTH; c++) {
            formatted_char *fc = &unbufferedWindow[l][c];
            if (fc->ch == 0) {
                strlcat(line, "\e[38;5;255m-\e[0m", escapedLength);
                continue;
            }

            sprintf(escaped, "%s%s%s%c\e[0m",
                fc->format.style & Z_STYLE_REVERSE_VIDEO ? "\e[7m" : "",
                fc->format.style & Z_STYLE_BOLD ? "\e[1m" : "",
                fc->format.style & Z_STYLE_ITALIC ? "\e[4m" : "",
                fc->ch);
            strlcat(line, escaped, escapedLength);
        }
        fprintf(stderr, "\e[38;5;81m|\e[0m%s\n", line);
    }

    unbufferedTouched = false;
}

// #define Z_STYLE_ROMAN 0
// #define Z_STYLE_REVERSE_VIDEO 1
// #define Z_STYLE_BOLD 2
// #define Z_STYLE_ITALIC 4
// #define Z_STYLE_FIXED_PITCH 8

// char* generate_unbuffered() {
//
// }

// Generate a JSON object for the output...
void generate_buffered_output() {
    trace(1, "");
    json_t* lines = json_array();

    for (int l = 0; l < bufferedLines+1; l++) {
        json_t* line;

        formatted_text *text = bufferedWindow[l];

        if (!text) {
            line = json_null();
        } else {
            line = json_array();

            while (text) {
                json_array_append_new(line, json_string(text->str));
                text = text->next;
            }
        }

        json_array_append_new(lines, line);
    }

    json_t* obj = json_object();
    json_object_set_new(obj, "lines", lines);
    char *str = json_dumps(obj, JSON_INDENT(2));
    json_decref(obj);

    fprintf(stderr, "\e[38;5;6m%s\e[0m\n", str);
    free(str);
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

// Simple interface helpers
char *screen_get_name()     { return PACKAGE_NAME; }

bool screen_return_false()  { return false; }
bool screen_return_true()   { return true; }
uint8_t screen_return_0()   { return 0; }
uint8_t screen_return_1()   { return 1; }

// As per the Z-machine spec, "8.4.1: A screen height of 255 lines means
// 'infinte height'."
uint16_t screen_get_screen_height() { return SCREEN_HEIGHT; }
uint16_t screen_get_screen_width()  { return SCREEN_WIDTH; }

z_colour screen_get_default_foreground_colour() { return Z_COLOUR_BLACK; }
z_colour screen_get_default_background_colour() { return Z_COLOUR_WHITE; }

int screen_parse_config_parameter(char *key, char *value) {
    trace(1, "%s, %s", key, value);
    return 1;
}

char *screen_get_config_value(char *key) {
    trace(1, "%s", key);
    return NULL;
}

char **screen_get_config_option_names() {
    trace(1, "");
    return NULL;
}

void screen_link_to_story(struct z_story *story) {
    trace(1, "%s", story ? story->absolute_file_name : "(NULL)");

    // The libglkif implementation has some version-dependent code (no idea
    // where the 'ver' value comes from!) which opens an initial window.
    // We don't do anything here.  It'd be nice to "tie" the story to the
    // screen interface, but this is old-school, not object-oriented C, so we'd
    // have to stash the story in a global or something.  (Or create a thunk
    // layer that the interface functions could access.)

    // if (ver <= 3)
    // {
    //   if (statusline) {
    //     glk_window_close(statusline, NULL);
    //   }
    //   statusline = glk_window_open(
    //       mainwin, winmethod_Above | winmethod_Fixed, 1, wintype_TextGrid, 3);
    // }
}

// Called at @restart time.
void screen_reset() {
    trace(1, "");
    erase_unbuffered_window();
    unbufferedLines = 0;
    unbufferedLine = 0;
    unbufferedCol = 0;
    erase_buffered_window();
    // if (statuswin) {
    //   glk_window_close(statuswin, NULL);
    //   statuswin = NULL;
    // }
    //
    // instatuswin = false;
    // glk_set_window(mainwin);
    // glk_set_style(style_Normal);
    // glk_window_clear(mainwin);
}

// This is called from two points: abort_interpreter() with an error message,
// and close_streams() with no error message.
//
// For the abort case, we call glkint_fatal_error_handler(), which does a
// glk_exit. This is necessary because abort_interpreter() is going to do
// a libc exit(), and we need to get Glk shut down first.
//
// However, close_streams() is different -- that's the normal interpreter
// exit (a @quit opcode). For that case, it's better to return and do nothing,
// so that close_streams() can flush fizmo's buffers. It will immediately
// exit through the end of fizmo_main(), which is the end of glk_main(),
// so we'll get a normal Glk shut down anyway.
int screen_close(z_ucs *error_message) {
    char *debug = dup_zucs_string_to_utf8_string(error_message);
    trace(1, "\"%s\"", debug);
    free(debug);

    if (error_message){
        char *message = dup_zucs_string_to_utf8_string(error_message);
        fprintf(stderr, "\n\n\e[38;5;1mERROR: %s\e[0m\n\n", message);
        free(message);
    }

    return 0;
}

void screen_set_buffer_mode(uint8_t new_buffer_mode) {
    char *mode = "UNKNOWN!!!";
    switch (new_buffer_mode) {
        case 0:
            mode = "OFF";
            break;
        case 1:
            mode = "ON";
            break;
    }
    trace(1, "%s", mode);
}

void append_buffered(char *src, int start, int end, bool advance) {
    trace(3, "(src), %d, %d, %s", start, end, advance ? "true" : "false");
    formatted_text *text = calloc(sizeof(formatted_text), 1);
    text->format = currentFormat;
    text->str = strndup(&src[start], end-start);

    if (bufferedWindow[bufferedLines] == NULL) {
        tracex(3, "starting new line");
        bufferedWindow[bufferedLines] = text;
    } else {
        formatted_text *cur = bufferedWindow[bufferedLines];
        tracex(3, "appending to existing line (%d)", (int)cur);
        while (cur->next != NULL) {
            cur = cur->next;
            tracex(3, "walking... (%d)", (int)cur);
        }
        cur->next = text;
    }

    if (advance) {
        tracex(3, "advancing line");
        bufferedLines++;
    }
}


void screen_output_z_ucs(z_ucs *z_ucs_output) {
    // // debug output (for now...)
    // char *debug = dup_zucs_string_to_utf8_string(z_ucs_output);
    // trace(1, "\"%s\"", debug);
    // free(debug);

    if (currentWindow == STATUS_WINDOW) {
        unbufferedTouched = true;
        // push characters into window, advance cursor...
        for (z_ucs *cur = z_ucs_output; *cur != 0; cur++) {
            formatted_char *fc = &unbufferedWindow[unbufferedLine][unbufferedCol];
            fc->ch = *cur;
            fc->format = currentFormat;
            unbufferedCol++;
        }

        // skip remaining debug output...
        return;
    }

    if (currentWindow == STORY_WINDOW) {
        // Start outputting where we left off... break the output into lines (TBD)
        char *output = dup_zucs_string_to_utf8_string(z_ucs_output);
        int lineStart = 0;
        int len = strlen(output);
        for (int i = 0; i < len; i++) {
            if (output[i] == '\n') {
                // Take what we have and buffer it...
                append_buffered(output, lineStart, i, true);
                lineStart = i+1;
            }
        }
        if (lineStart < len) {
            append_buffered(output, lineStart, len, false);
        }
        free(output);
    }

}

// The JSON I/O may need to go elsewhere, this is a temporary stub
int wait_for_input(bool single, char *dest, int max, int *elapsedTenths) {
    trace(2, "%s, (*dest), %d, (*elapsedTenths)", single ? "true" : "false", max);

    dump_unbuffered();
    dump_buffered();
    generate_buffered_output();
    erase_buffered_window();    // ???

    struct timeval start;
    struct timeval end;
    int n = gettimeofday(&start, NULL); // do we care about failed calls?

    char buf[1000];
    memset(buf, 0, 1000);   // need sizeof/countof?
    fprintf(stderr, "\n\e[38;5;13mwaiting to read%s...\e[0m\n", single ? " (single character only!)": "");
    char * str = fgets(buf, 1000, stdin);

    if (!str) {
        tracex(1, "error reading line");
        return -1;
    }

    n = gettimeofday(&end, NULL); // do we care about failed calls?

    // try to fill the elapsed time...
    if (elapsedTenths) {
        tracex(1, "elapsed time: %ld to %ld", start.tv_sec, end.tv_sec);
        *elapsedTenths = (end.tv_sec - start.tv_sec) * 10;
    } else {
        tracex(2, "no elapsed time requested");
    }

    for (int i = 0; i < max; i++) {
        if (buf[i] == '\n' || buf[i] == 0) {
            dest[i] = 0;
            tracex(1, "read (%d): \"%s\"", i, dest);
            return i;
        }

        dest[i] = buf[i];
    }

    tracex(1, "read maximum length");
    return max;
}


int16_t screen_read_line(zscii *dest, uint16_t maximum_length,
    uint16_t tenth_seconds, uint32_t verification_routine,
    uint8_t preloaded_input, int *tenth_seconds_elapsed,
    bool disable_command_history, bool return_on_escape) {
    trace(1, "%d, %d, %d, %d, (*elapsed), %s, %s",
        maximum_length, tenth_seconds, verification_routine, preloaded_input,
        disable_command_history ? "true" : "false",
        return_on_escape? "true" : "false");

    return wait_for_input(false, (char *)dest, maximum_length, tenth_seconds_elapsed);
}

int screen_read_char(uint16_t tenth_seconds,
    uint32_t verification_routine, int *tenth_seconds_elapsed) {
    trace(1, "%d, %d, (*elapsed)", tenth_seconds, verification_routine);

    char ch;
    int n = wait_for_input(true, &ch, 1, tenth_seconds_elapsed);
    if (n > 0) {
        tracex(1, "returning %d ('%c')", ch, ch);
        return ch;
    }

    tracex(1, "failure waiting for input");
    return 0;
}

void screen_show_status(z_ucs *room_description,
    int status_line_mode, int16_t parameter1, int16_t parameter2) {
    trace(1, "%s, %d, %d, %d", (char*)room_description, status_line_mode, parameter1, parameter2);
}

// const char* STYLE_NAMES[] = {
//     "ROMAN",
//     "REVERSE",
//     "BOLD",
//     "ITALIC",
//     "FIXED",
// };

void screen_set_text_style(z_style text_style) {
    trace(1, "%d", text_style);

    if (text_style == Z_STYLE_ROMAN) {
        currentFormat.style = Z_STYLE_ROMAN;
    } else {
        currentFormat.style |= text_style;
    }
}


void screen_set_colour(z_colour foreground, z_colour background, int16_t window) {
    trace(1, "%d, %d, %d", foreground, background, window);
}


const char* FONT_NAMES[] = {
    "PREVIOUS",
    "NORMAL",
    "PICTURE",
    "CHARACTER_GRAPHICS",
    "COURIER_FIXED_PITCH",
};

void screen_set_font(z_font font_type) {
    trace(1, "%s", FONT_NAMES[font_type]);
}

void screen_split_window(int16_t nof_lines) {
    trace(1, "%d lines", nof_lines);

    // If the unbuffered window was touched, *and* we're shrinking it,
    // force-dump the current values.  This appears to be needed for
    // Graham Nelson's "Curses", for example, for the intro quote.
    if (nof_lines < unbufferedLines && unbufferedTouched) {
        dump_unbuffered();
    }

    partial_erase_unbuffered_window(nof_lines);

    unbufferedLines = nof_lines;
}

// We only support very simple "windows", where window 0 is the story, and
// window 1 (if any) is the status.
const char* WINDOW_NAMES[] = {
    "STORY",
    "STATUS",
};

void screen_set_window(int16_t window_number) {
    trace(1, "%s", WINDOW_NAMES[window_number]);
    currentWindow = window_number;
}

void screen_erase_window(int16_t window_number) {
    trace(1, "%s", WINDOW_NAMES[window_number]);
    switch (window_number) {
        case STATUS_WINDOW:
            erase_unbuffered_window();
            break;
        case STORY_WINDOW:
            erase_buffered_window();
            break;
    }
}

void screen_set_cursor(int16_t line, int16_t column, int16_t window) {
    trace(3, "%d, %d, %s", line, column, WINDOW_NAMES[window]);
    if (window == STATUS_WINDOW) {
        unbufferedLine = line - 1;
        unbufferedCol = column - 1;
    }
}

uint16_t screen_get_cursor_row()    {
    trace(1, "[window: %s]", WINDOW_NAMES[currentWindow]);
    if (currentWindow != STATUS_WINDOW) {
        return 0;
    }

    return unbufferedLine + 1;
}

uint16_t screen_get_cursor_column() {
    trace(1, "[window: %s]", WINDOW_NAMES[currentWindow]);
    if (currentWindow != STATUS_WINDOW) {
        return 0;
    }

    return unbufferedCol + 1;
}

void screen_erase_line_value(uint16_t start_position) {
    trace(1, "%d, [window: %s]", start_position, WINDOW_NAMES[currentWindow]);
    if (currentWindow != STATUS_WINDOW) {
        return;
    }
}

void screen_erase_line_pixels(uint16_t start_position) {
    trace(1, "%d, [window: %s]", start_position, WINDOW_NAMES[currentWindow]);
    if (currentWindow != STATUS_WINDOW) {
        return;
    }
}

void screen_output_info() {
    trace(1, "");
}

void screen_game_was_restored_and_history_modified() {
    trace(1, "");
}

int screen_prompt_for_filename(char *filename_suggestion,
    z_file **result, char *directory, int filetype, int fileaccess) {
    trace(1, "%s, (result), %s, %d, %d", filename_suggestion, directory, filetype, fileaccess);
    return 0;
}



struct z_screen_interface bot_screen = {
    &screen_get_name,           // get_interface_name

    &screen_return_true,        // is_status_line_available
    &screen_return_false,       // is_split_screen_available
    &screen_return_true,        // is_variable_pitch_font_default
    &screen_return_false,       // is_colour_available
    &screen_return_false,       // is_picture_displaying_available
    &screen_return_true,        // is_bold_face_available
    &screen_return_true,        // is_italic_available
    &screen_return_true,        // is_fixed_space_font_available
    &screen_return_true,        // is_timed_keyboard_input_available
    &screen_return_true,        // is_preloaded_input_available
    &screen_return_false,       // is_character_graphics_font_availiable
    &screen_return_false,       // is_picture_font_availiable

    &screen_get_screen_height,  // get_screen_height_in_lines
    &screen_get_screen_width,   // get_screen_width_in_characters
    &screen_get_screen_width,   // get_screen_width_in_units
    &screen_get_screen_height,  // get_screen_height_in_units
    &screen_return_1,           // get_font_width_in_units
    &screen_return_1,           // get_font_height_in_units
    &screen_get_default_foreground_colour,  // get_default_foreground_colour
    &screen_get_default_background_colour,  // get_default_background_colour
    &screen_return_0,           // get_total_width_in_pixels_of_text_sent_to_output_stream_3

    &screen_parse_config_parameter, // parse_config_parameter
    &screen_get_config_value,   // get_config_value
    &screen_get_config_option_names,    // get_config_option_names

    &screen_link_to_story,      // link_interface_to_story

    &screen_reset,              // reset_interface

    &screen_close,              // close_interface

    &screen_set_buffer_mode,    // set_buffer_mode
    &screen_output_z_ucs,       // z_ucs_output

    &screen_read_line,          // read_line
    &screen_read_char,          // read_char

    &screen_show_status,        // show_status
    &screen_set_text_style,     // set_text_style
    &screen_set_colour,         // set_colour
    &screen_set_font,           // set_font
    &screen_split_window,       // split_window
    &screen_set_window,         // set_window
    &screen_erase_window,       // erase_window

    &screen_set_cursor,         // set_cursor
    &screen_get_cursor_row,     // get_cursor_row
    &screen_get_cursor_column,  // get_cursor_column
    &screen_erase_line_value,   // erase_line_value
    &screen_erase_line_pixels,  // erase_line_pixels
    &screen_output_info,        // output_interface_info
    &screen_return_false,       // input_must_be_repeated_by_story
    &screen_game_was_restored_and_history_modified, // game_was_restored_and_history_modified
    &screen_prompt_for_filename,    // prompt_for_filename
    NULL,                       // do_autosave
    NULL                        // restore_autosave
};
