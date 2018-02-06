/* screen.c
 *
 * This file is part of fizmo-bot.  Please see LICENSE.md for the license.
 */

// #include <string.h>
#include <stdio.h>
#include "config.h"
#include "util.h"

// fizmo includes...
#include <interpreter/fizmo.h>


// Simple interface helpers
char *screen_get_name()     { return PACKAGE_NAME; }

bool screen_return_false()  { return false; }
bool screen_return_true()   { return true; }
uint8_t screen_return_0()   { return 0; }
uint8_t screen_return_1()   { return 1; }

uint16_t screen_get_screen_height() { return 200; }
uint16_t screen_get_screen_width()  { return 100; }

z_colour screen_get_default_foreground_colour() { return Z_COLOUR_BLACK; }
z_colour screen_get_default_background_colour() { return Z_COLOUR_WHITE; }

int screen_parse_config_parameter(char *key, char *value) { return 1; }

char *screen_get_config_value(char *key)    { return NULL; }
char **screen_get_config_option_names()     { return NULL; }

void screen_link_to_story(struct z_story *story) {
    dbg(__func__);
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
    dbg(__func__);
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
    dbg(__func__);
    if (error_message){
        // glkint_fatal_error_handler(NULL, error_message, NULL, 0, 0);
    }

    return 0;
}

void screen_set_buffer_mode(uint8_t new_buffer_mode) {
    dbg(__func__);
}

void screen_output_z_ucs(z_ucs *z_ucs_output) {
    dbg(__func__);
    // Conveniently, z_ucs is the same as glui32.
    // glk_put_string_uni(z_ucs_output);
}

int16_t screen_read_line(zscii *dest, uint16_t maximum_length,
    uint16_t tenth_seconds, uint32_t verification_routine,
    uint8_t preloaded_input, int *tenth_seconds_elapsed,
    bool disable_command_history, bool return_on_escape) {
    dbg(__func__);
    return 0;
}

int screen_read_char(uint16_t tenth_seconds,
    uint32_t verification_routine, int *tenth_seconds_elapsed) {
    dbg(__func__);
    return 0;
}

void screen_show_status(z_ucs *room_description,
    int status_line_mode, int16_t parameter1, int16_t parameter2) {
    dbg(__func__);
}


void screen_set_text_style(z_style text_style) {
    dbg(__func__);
}


void screen_set_colour(z_colour foreground,
    z_colour background, int16_t window) {
    dbg(__func__);
}

void screen_set_font(z_font font_type) {
    dbg(__func__);
}

void screen_split_window(int16_t nof_lines) {
    dbg(__func__);
}


void screen_set_window(int16_t window_number) {
    dbg(__func__);
}

void screen_erase_window(int16_t window_number) {
    dbg(__func__);
}

void screen_set_cursor(int16_t line, int16_t column, int16_t window) {
    dbg(__func__);
}

uint16_t screen_get_cursor_row()    { return 0; }
uint16_t screen_get_cursor_column() { return 0; }

void screen_erase_line_value(uint16_t start_position) {
    dbg(__func__);
}

void screen_erase_line_pixels(uint16_t start_position) {
    dbg(__func__);
}

void screen_output_info() {
    dbg(__func__);
}

void screen_game_was_restored_and_history_modified() {
    dbg(__func__);
}

int screen_prompt_for_filename(char *filename_suggestion,
    z_file **result, char *directory, int filetype, int fileaccess) {
    dbg(__func__);
    return 0;
}



struct z_screen_interface bot_screen = {
    &screen_get_name,           // get_interface_name

    &screen_return_true,        // is_status_line_available
    &screen_return_true,        // is_split_screen_available
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
