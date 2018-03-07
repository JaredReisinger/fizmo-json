// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "screen.h"

#include <map>
#include <string>

extern "C" {
    #include <stdio.h>
    #include <ctype.h>
    #include <string.h>
    #include <sys/time.h>

    // fizmo includes...
    #include <interpreter/fizmo.h>
    #include <interpreter/streams.h>
    #include <interpreter/text.h>
    #include <interpreter/zscii.h>
    #include <tools/z_ucs.h>

    // jansson...
    #include <jansson.h>
}

#include "config.h"
#include "util.h"
#include "buffer.h"
#include "columns.h"
#include "format.h"

Format currentFormat;
Buffer screenBuffer;

// The BLOCKBUF tracked by fizmo *never* shrinks (for performance and other
// reasons), but we need to know the intended size when rendering output.
int upperWindowHeight = 0;

const int STORY_WINDOW = 0;
const int STATUS_WINDOW = 1;

int currentWindow = STORY_WINDOW;

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

static bool use_simple_console_input = false;

void screen_use_simple_console_input() {
    trace(1, "");
    use_simple_console_input = true;
}

// Generate a JSON object for the output...
void generate_output() {
    trace(1, "");

    // Collect status
    BlockBuf upperWindow(upper_window_buffer, upperWindowHeight);
    Columns columns(upperWindow);
    // std::cerr << columns << "\n";
    auto upperBuffer = upperWindow.ToBuffer();

    json_t* status = json_object();
    json_object_set_new(status, "columns", columns.ToJson());
    json_object_set_new(status, "lines", upperBuffer->ToJson());
    delete upperBuffer;
    upperBuffer = NULL;

    // Collect story
    json_t* story = screenBuffer.ToJson(true, true);

    // Put it all together!
    json_t* output = json_object();
    json_object_set_new(output, "status", status);
    json_object_set_new(output, "story", story);

    char *str = json_dumps(output, JSON_INDENT(2));
    json_decref(output);

    fprintf(stdout, "%s\n", str);
    free(str);
    fflush(stdout);
}



// Simple interface helpers
char *screen_get_name()     { return (char*)PACKAGE_NAME; }

bool screen_return_false()  { return false; }
bool screen_return_true()   { return true; }
uint8_t screen_return_0()   { return 0; }
uint8_t screen_return_1()   { return 1; }

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
}

// Called at @restart time.
void screen_reset() {
    trace(1, "");
    currentFormat.Reset();
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
    const char *mode = "UNKNOWN!!!";
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


void screen_output_z_ucs(z_ucs *z_ucs_output) {
    if (currentWindow != STORY_WINDOW) {
        return;
    }

    screenBuffer.Append(ToUtf8(z_ucs_output), currentFormat);
}

const std::map<std::u32string, const zscii> single_map = {
    { U"delete",  ZSCII_DELETE },
    { U"tab",     ZSCII_TAB },
    { U"space",   ' ' },
    { U"newline", ZSCII_NEWLINE },
    { U"enter",   ZSCII_NEWLINE },
    { U"return",  ZSCII_NEWLINE },
    { U"escape",  ZSCII_ESCAPE },
    { U"up",      ZSCII_CURSOR_UP },
    { U"down",    ZSCII_CURSOR_DOWN },
    { U"left",    ZSCII_CURSOR_LEFT },
    { U"right",   ZSCII_CURSOR_RIGHT },
    { U"f1",      ZSCII_F1 },
    { U"f2",      ZSCII_F2 },
    { U"f3",      ZSCII_F3 },
    { U"f4",      ZSCII_F4 },
    { U"f5",      ZSCII_F5 },
    { U"f6",      ZSCII_F6 },
    { U"f7",      ZSCII_F7 },
    { U"f8",      ZSCII_F8 },
    { U"f9",      ZSCII_F9 },
    { U"f10",     ZSCII_F10 },
    { U"f11",     ZSCII_F11 },
    { U"f12",     ZSCII_F12 },
};

// #define ZSCII_KEYPAD_0 145
// #define ZSCII_KEYPAD_1 146
// #define ZSCII_KEYPAD_2 147
// #define ZSCII_KEYPAD_3 148
// #define ZSCII_KEYPAD_4 149
// #define ZSCII_KEYPAD_5 150
// #define ZSCII_KEYPAD_6 151
// #define ZSCII_KEYPAD_7 152
// #define ZSCII_KEYPAD_8 153
// #define ZSCII_KEYPAD_9 154

// The JSON I/O may need to go elsewhere, this is a temporary stub
int wait_for_input(bool single, zscii *dest, int max, int *elapsedTenths) {
    trace(2, "%s, (*dest), %d, (*elapsedTenths)", single ? "true" : "false", max);

    // std::cerr << screenBuffer << "\n";
    generate_output();
    screenBuffer.Empty();

    struct timeval start;
    struct timeval end;
    int n = gettimeofday(&start, NULL); // do we care about failed calls?

    // fprintf(stderr, "\n\e[38;5;13mwaiting to read%s...\e[0m\n", single ? " (single character only!)": "");

    // Extract input string...
    std::u32string u32input;

    if (use_simple_console_input) {
        char buf[1000];
        const char * value = fgets(buf, sizeof(buf), stdin);
        if (!value) {
            tracex(1, "error reading line");
            return -1;
        }
        u32input = FromUtf8(value);
    } else {
        json_error_t error;
        json_t *input = json_loadf(stdin, JSON_DISABLE_EOF_CHECK, &error);
        if (!input) {
            fprintf(stderr, "ERROR with input, line %d, column %d (position %d): %s (%s)\n", error.line, error.column, error.position, error.text, error.source);
            return -1;
        }

        // What did we get?
        if (!json_is_object(input)) {
            fprintf(stderr, "ERROR: expected object!");
            json_decref(input);
            return -1;
        }

        const char *value = json_string_value(json_object_get(input, "input"));
        u32input = FromUtf8(value);
        json_decref(input);
        // strlcpy(buf, value, sizeof(buf));
    }

    n = gettimeofday(&end, NULL); // do we care about failed calls?

    // try to fill the elapsed time...
    if (elapsedTenths) {
        tracex(1, "elapsed time: %ld to %ld", start.tv_sec, end.tv_sec);
        *elapsedTenths = (end.tv_sec - start.tv_sec) * 10;
    } else {
        tracex(2, "no elapsed time requested");
    }

    // terminate as of the first newline... *except* for the single-char case
    // when the newline is the first character.
    auto iNewline = u32input.find('\n');
    if (iNewline != std::u32string::npos && !(single && iNewline == 0)) {
        u32input.resize(iNewline);
    }

    // Special-case for single... recognize some special terms and translate
    // them to specific characters.
    if (single) {
        tracex(1, "looking up \"%ls\" in %d-entry single_map...", u32input.c_str(), single_map.size());
        auto found = single_map.find(u32input);
        if (found != single_map.end()) {
            tracex(1, "found \"%ls\" -> '%c' in single_map...", u32input.c_str(), found->second);
            dest[0] = found->second;
            if (max > 1) {
                dest[1] =  '\0';
            }
            return 1;
        }
    }

    // Finally, copy everything to the destination buffer...

    int o = 0;
    for (int i = 0; i < u32input.length() && o < max-1; ++i, ++o) {
        zscii ch = unicode_char_to_zscii_input_char(u32input[i]);
        if (ch == 0xff) {
            // silently drop the character?
            tracex(1, "Droping unrecognized unicode character '%1$c' (%1$d)!", u32input[i]);
            --o;
            continue;
        }
        tracex(1, "Converted '%1$c' (%1$d) to '%2$c' (%2$d)...\n", u32input[i], ch);
        dest[o] = ch;
    }
    dest[o] = '\0';

    return o;
}



int16_t screen_read_line(zscii *dest, uint16_t maximum_length,
    uint16_t tenth_seconds, uint32_t verification_routine,
    uint8_t preloaded_input, int *tenth_seconds_elapsed,
    bool disable_command_history, bool return_on_escape) {
    trace(1, "%d, %d, %d, %d, (*elapsed), %s, %s",
        maximum_length, tenth_seconds, verification_routine, preloaded_input,
        disable_command_history ? "true" : "false",
        return_on_escape? "true" : "false");

    return wait_for_input(false, dest, maximum_length, tenth_seconds_elapsed);
}

int screen_read_char(uint16_t tenth_seconds,
    uint32_t verification_routine, int *tenth_seconds_elapsed) {
    trace(1, "%d, %d, (*elapsed)", tenth_seconds, verification_routine);

    zscii buf[2];
    int n = wait_for_input(true, buf, 2, tenth_seconds_elapsed);
    if (n > 0) {
        tracex(1, "returning %1$d ('%1$c')", buf[0]);
        return buf[0];
    }

    tracex(1, "failure waiting for input");
    return 0;
}

void screen_show_status(z_ucs *room_description,
    int status_line_mode, int16_t parameter1, int16_t parameter2) {
    trace(1, "%s, %d, %d, %d", (char*)room_description, status_line_mode, parameter1, parameter2);
    // REVIEW: ???
}

void screen_set_text_style(z_style text_style) {
    trace(1, "%d (%s)", text_style, Format::StyleName(text_style));
    currentFormat.SetStyle(text_style);
}


void screen_set_colour(z_colour foreground, z_colour background, int16_t window) {
    trace(1, "%d (%s), %d (%s), %d", foreground, Format::ColorName(foreground), background, Format::ColorName(background), window);
    if (window != STORY_WINDOW) {
        return;
    }
    currentFormat.SetForeground(foreground);
    currentFormat.SetBackground(background);
}


void screen_set_font(z_font font_type) {
    trace(1, "%d (%s)", font_type, Format::FontName(font_type));
    currentFormat.SetFont(font_type);
}

void screen_split_window(int16_t nof_lines) {
    trace(1, "%d lines", nof_lines);

    // If we're shrinking it, force-dump the current values.  This appears to be
    // needed for Graham Nelson's "Curses", for example, for the intro quote.
    if (nof_lines < upperWindowHeight) {
        tracex(1, "prepending upper window!");
        BlockBuf upperWindow(upper_window_buffer, upperWindowHeight);
        auto buf = upperWindow.ToBuffer();
        // std::cerr << buf << "\n";
        screenBuffer.Prepend(*buf);
        delete buf;
    }

    upperWindowHeight = nof_lines;
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

    if (window_number != STORY_WINDOW) {
        return;
    }

    // erase_buffered_window();
    // free_line_buffer(&screen_linebuf);
}

void screen_set_cursor(int16_t line, int16_t column, int16_t window) {
    trace(3, "%d, %d, %s", line, column, WINDOW_NAMES[window]);
    // if (window_number != STORY_WINDOW) {
    //     return;
    // }
}

uint16_t screen_get_cursor_row()    {
    trace(1, "[window: %s]", WINDOW_NAMES[currentWindow]);
    if (currentWindow == STATUS_WINDOW) {
        // return unbufferedParagraph + 1;
        return upper_window_buffer->xpos + 1;
    }

    return 0;
}

uint16_t screen_get_cursor_column() {
    trace(1, "[window: %s]", WINDOW_NAMES[currentWindow]);
    if (currentWindow == STATUS_WINDOW) {
        // return unbufferedCol + 1;
        return upper_window_buffer->ypos + 1;
    }

    return 0;
}

void screen_erase_line_value(uint16_t start_position) {
    trace(1, "%d, [window: %s]", start_position, WINDOW_NAMES[currentWindow]);
}

void screen_erase_line_pixels(uint16_t start_position) {
    trace(1, "%d, [window: %s]", start_position, WINDOW_NAMES[currentWindow]);
}

void screen_output_info() {
    trace(1, "");
    (void)streams_latin1_output((char*)PACKAGE_NAME);
    (void)streams_latin1_output((char*)" interface version ");
    (void)streams_latin1_output((char*)PACKAGE_VERSION);
    (void)streams_latin1_output((char*)"\n");
}

void screen_game_was_restored_and_history_modified() {
    trace(1, "");
}

int screen_prompt_for_filename(char *filename_suggestion,
    z_file **result, char *directory, int filetype, int fileaccess) {
    trace(1, "%s, (result), %s, %d, %d", filename_suggestion, directory, filetype, fileaccess);
    // -3 means "not supported", but the interpreter specifically "handles"
    // this for transcripts... perhaps we should notice this and return
    // -2 in that case?
    return -3;
}

// int screen_do_autosave() {
//     trace(0, "");
//     return 0;
// }
//
// int screen_restore_autosave(z_file *savefile) {
//     trace(0, "%p", savefile);
//     return 0;
// }


struct z_screen_interface bot_screen = {
    &screen_get_name,           // get_interface_name

    &screen_return_true,        // is_status_line_available
    &screen_return_false, //??  // is_split_screen_available
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
    &screen_prompt_for_filename,// prompt_for_filename
    // &screen_do_autosave,     // do_autosave
    // &screen_restore_autosave // restore_autosave
    NULL,                       // do_autosave
    NULL                        // restore_autosave
};
