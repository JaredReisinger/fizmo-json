// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "format.h"

#include <iostream>

extern "C" {
    #include <stdlib.h>
    #include <interpreter/fizmo.h>
    // #include <jansson.h>
}

// const formatted_char empty_char = (formatted_char){ };
#include "paragraph.h"

#include "util.h"

// bool format_equal(format_info a, format_info b) {
//     return a.style == b.style;
// }
//
// bool blockbuf_format_equal(const struct blockbuf_char *a, const struct blockbuf_char *b) {
//     // we only care about style for now...
//     return a->style == b->style;
// }
//
// struct blockbuf_char *blockbuf_char_at_yx(BLOCKBUF *bb, int ypos, int xpos) {
//     return &bb->content[(ypos * bb->width) + xpos];
// }
//
//
// formatted_text *new_text(const char *src, int start, int end, format_info format) {
//     trace(3, "");
//     formatted_text *text = (formatted_text *)fizmo_malloc(sizeof(formatted_text));
//     memset(text, 0, sizeof(formatted_text));
//     text->format = format;
//     text->str = strndup(&src[start], end-start);
//     return text;
// }
//
// void free_formatted_text(formatted_text **text) {
//     trace(3, "%p", *text);
//
//     if (text == NULL || *text == NULL) {
//         return;
//     }
//
//     free_formatted_text(&((*text)->next));
//
//     // From the man page for free(3): "If ptr is a NULL pointer, no
//     // operation is performed".
//     tracex(3, "freeing string %p", (*text)->str);
//     free((*text)->str);
//     tracex(3, "freeing formatted_text %p", *text);
//     free(*text);
//     *text = NULL;
// }
//
// // create formatted text run from fizmo's BLOCKBUF.
// formatted_text *formatted_text_from_blockbuf(BLOCKBUF *buf, int line, int start, int end) {
//     trace(2, "%p, %d, %d, %d", buf, line, start, end);
//
//
//     // TEST CODE START!!!
//     // tracex(0, "========== testing code start ==============");
//     // FormattedRun *foo = new FormattedRun(buf, line, start, end);
//     // foo->Dump();
//     // delete foo;
//     // tracex(0, "========== testing code end ==============");
//     // tracex(0, "========== testing code start ==============");
//     BlockBuf bbb(buf);
//     // Span *span = new Span(buf, line, start, end);
//     Paragraph run(bbb, line, start, end);
//     // std::cerr << run << "\n";
//     // delete span;
//     // tracex(0, "========== testing code end ==============");
//     // TEST CODE END!!!
//
//
//     formatted_text *result = NULL;
//     formatted_text *prev = NULL;
//     char strBuf[end-start + 1];
//     int len = 0;
//     struct blockbuf_char *bbchFormat = blockbuf_char_at_yx(buf, line, start);
//
//     for (int c = start; c < end; c++) {
//         struct blockbuf_char *bbch = blockbuf_char_at_yx(buf, line, c);
//
//         if (blockbuf_format_equal(bbch, bbchFormat)) {
//             strBuf[len++] = zucs_char_to_latin1_char(bbch->character);
//         } else {
//             // format boundary!
//             strBuf[len] = '\0';
//             // tracex(1, "appending line %d (1) \"%s\"", l, strBuf);
//             formatted_text *text = new_text(strBuf, 0, len, (format_info){ .style = bbchFormat->style});
//
//             if (!result) {
//                 result = text;
//             } else {
//                 prev->next = text;
//             }
//
//             prev = text;
//
//             strBuf[0] = zucs_char_to_latin1_char(bbch->character);
//             len = 1;
//             bbchFormat = bbch;
//         }
//
//     }
//
//     // any trailing text...
//     strBuf[len] = '\0';
//     // tracex(1, "appending line %d (2) \"%s\"", l, buf);
//     formatted_text *text = new_text(strBuf, 0, len, (format_info){ .style = bbchFormat->style});
//     if (!result) {
//         result = text;
//     } else {
//         prev->next = text;
//     }
//     // tracex(1, "finished line: %d", l);
//
//     return result;
// }

void set_optional_bool(json_t *obj, const char *name, bool value) {
    if (value) {
        json_object_set_new(obj, name, json_true());
    }
}


// json_t *formatted_text_to_json(formatted_text *text) {
//     if (!text) {
//         return json_null();
//     }
//
//     json_t *line = json_array();
//
//     for (formatted_text *cur = text; cur != NULL; cur = cur->next) {
//         json_t *span = json_object();
//         z_style style = cur->format.style;
//         // json_object_set_new(span, "style", json_integer(style));
//         set_optional_bool(span, "reverse", style & Z_STYLE_REVERSE_VIDEO);
//         set_optional_bool(span, "bold", style & Z_STYLE_BOLD);
//         set_optional_bool(span, "italic", style & Z_STYLE_ITALIC);
//         set_optional_bool(span, "fixed", style & Z_STYLE_FIXED_PITCH);
//         json_object_set_new(span, "text", json_string(cur->str));
//         json_array_append_new(line, span);
//     }
//
//     return line;
// }


// ================== new C++ code... ====================

const char * FontName(z_font font, bool brief = false) {
    switch (font) {
        case Z_FONT_PREVIOUS_FONT:          return "previous";
        case Z_FONT_NORMAL:                 return brief ? "" : "normal";
        case Z_FONT_PICTURE:                return "picture";
        case Z_FONT_CHARACTER_GRAPHICS:     return "character";
        case Z_FONT_COURIER_FIXED_PITCH:    return "fixed";
    }

    return "(unknown)";
}

const char * StyleName(z_style style, bool brief = false) {
    // styles are a bitmask, but we don't want to be dynamically creating the
    // display string... so we enumerate all of the possible combinations. We do
    // this in numerical order to make sure we don't miss anything.  With only 4
    // bits, there are only 16 variations, which isn't too bad.

    // #define Z_STYLE_ROMAN 0
    // #define Z_STYLE_REVERSE_VIDEO 1
    // #define Z_STYLE_BOLD 2
    // #define Z_STYLE_ITALIC 4
    // #define Z_STYLE_FIXED_PITCH 8

    switch (style) {
        case Z_STYLE_ROMAN:                         return brief ? "" : "roman";
        case Z_STYLE_REVERSE_VIDEO:                 return "reverse";
        case Z_STYLE_BOLD:                          return "bold";
        case Z_STYLE_BOLD|Z_STYLE_REVERSE_VIDEO:    return "bold|reverse";
        case Z_STYLE_ITALIC:                        return "italic";
        case Z_STYLE_ITALIC|Z_STYLE_REVERSE_VIDEO:  return "italic|reverse";
        case Z_STYLE_ITALIC|Z_STYLE_BOLD:           return "italic|bold";
        case Z_STYLE_ITALIC|Z_STYLE_BOLD|Z_STYLE_REVERSE_VIDEO:
            return "italic|bold|reverse";
        case Z_STYLE_FIXED_PITCH:
            return "fixed";
        case Z_STYLE_FIXED_PITCH|Z_STYLE_REVERSE_VIDEO:
            return "fixed|reverse";
        case Z_STYLE_FIXED_PITCH|Z_STYLE_BOLD:
            return "fixed|bold";
        case Z_STYLE_FIXED_PITCH|Z_STYLE_BOLD|Z_STYLE_REVERSE_VIDEO:
            return "fixed|bold|reverse";
        case Z_STYLE_FIXED_PITCH|Z_STYLE_ITALIC:
            return "fixed|italic";
        case Z_STYLE_FIXED_PITCH|Z_STYLE_ITALIC|Z_STYLE_REVERSE_VIDEO:
            return "fixed|italic|reverse";
        case Z_STYLE_FIXED_PITCH|Z_STYLE_ITALIC|Z_STYLE_BOLD:
            return "fixed|italic|bold";
        case Z_STYLE_FIXED_PITCH|Z_STYLE_ITALIC|Z_STYLE_BOLD|Z_STYLE_REVERSE_VIDEO:
            return "fixed|italic|bold|reverse";
    }

    return "(unknown)";
}

const char * ColorName(z_colour color, bool brief = false) {
    switch (color) {
        case Z_COLOUR_UNDEFINED:            return "undefined";
        case Z_COLOUR_UNDER_CURSOR:         return "undercursor";
        case Z_COLOUR_CURRENT:              return "current";
        case Z_COLOUR_DEFAULT:              return brief ? "" : "default";
        case Z_COLOUR_BLACK:                return "black";
        case Z_COLOUR_RED:                  return "red";
        case Z_COLOUR_GREEN:                return "green";
        case Z_COLOUR_YELLOW:               return "yellow";
        case Z_COLOUR_BLUE:                 return "blue";
        case Z_COLOUR_MAGENTA:              return "magenta";
        case Z_COLOUR_CYAN:                 return "cyan";
        case Z_COLOUR_WHITE:                return "white";
        case Z_COLOUR_MSDOS_DARKISH_GREY:   return "grey";
        // case Z_COLOUR_AMIGA_LIGHT_GREY:     return "";
        case Z_COLOUR_MEDIUM_GREY:          return "mediumgrey";
        case Z_COLOUR_DARK_GREY:            return "darkgrey";
        // Z_INVALID_RGB_COLOUR is defined with a trailing ';'...
        // case (z_colour)0xffffffff:          return "invalidrgb";
    }

    return "(unknown)";
}

Format::Format() {
    trace(2, "%p", this);
    Reset();
    // font_               = Z_FONT_NORMAL;    // ?
    // style_              = Z_STYLE_ROMAN;
    // foreground_colour_  = Z_COLOUR_DEFAULT; // ?
    // background_colour_  = Z_COLOUR_DEFAULT; // ?
}

Format::Format(const Format &format) {
    trace(2, "%p", &format);
    font_               = format.font_;
    style_              = format.style_;
    foreground_colour_  = format.foreground_colour_;
    background_colour_  = format.background_colour_;
}

Format::Format(const struct blockbuf_char& bbch) {
    trace(2, "'%c'", bbch.character);
    font_               = bbch.font;
    style_              = bbch.style;
    foreground_colour_  = bbch.foreground_colour;
    background_colour_  = bbch.background_colour;
}

void Format::Reset() {
    trace(2, "");
    font_               = Z_FONT_NORMAL;    // ?
    style_              = Z_STYLE_ROMAN;
    foreground_colour_  = Z_COLOUR_DEFAULT; // ?
    background_colour_  = Z_COLOUR_DEFAULT; // ?
}

void Format::SetFont(z_font font) {
    trace(2, "%d (%s)", font, FontName(font));
    font_ = font;
}

void Format::SetStyle(z_style style) {
    trace(2, "%d (%s)", style, StyleName(style));
    if (style == Z_STYLE_ROMAN) {
        style_ = style;
    } else {
        style_ |= style;
    }
}

void Format::SetForeground(z_colour color) {
    trace(2, "%d (%s)", color, ColorName(color));
    foreground_colour_ = color;
}

void Format::SetBackground(z_colour color) {
    trace(2, "%d (%s)", color, ColorName(color));
    background_colour_ = color;
}



bool Format::operator==(const Format &rhs) const {
    trace(2, "%p, %p", this, &rhs);
    return font_ == rhs.font_ &&
        style_ == rhs.style_ &&
        foreground_colour_ == rhs.foreground_colour_ &&
        background_colour_ == rhs.background_colour_;
}

bool Format::operator!=(const Format &rhs) const {
    return !(*this == rhs);
}

void Format::AddJsonProps(json_t *obj) const {
    trace(2, "&p", obj);
    set_optional_bool(obj, "reverse", style_ & Z_STYLE_REVERSE_VIDEO);
    set_optional_bool(obj, "bold", style_ & Z_STYLE_BOLD);
    set_optional_bool(obj, "italic", style_ & Z_STYLE_ITALIC);
    set_optional_bool(obj, "fixed", style_ & Z_STYLE_FIXED_PITCH);
}



std::ostream & operator<<(std::ostream &os, const Format& format) {
    return os << FontName(format.font_, true) << "," << StyleName(format.style_, true) << "," << ColorName(format.foreground_colour_, true) << "," << ColorName(format.background_colour_, true);
}
