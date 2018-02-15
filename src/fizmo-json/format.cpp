// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "format.h"

#include <iostream>

extern "C" {
    #include <stdlib.h>
    #include <interpreter/fizmo.h>
}

#include "paragraph.h"
#include "util.h"


Format::Format() {
    trace(2, "%p", this);
    Reset();
}

Format::Format(const Format &format) {
    trace(2, "[%p] %p (copy)", this, &format);
    font_               = format.font_;
    style_              = format.style_;
    foreground_colour_  = format.foreground_colour_;
    background_colour_  = format.background_colour_;
}

Format::Format(const struct blockbuf_char& bbch) {
    trace(2, "[%p] %p ('%c')", this, &bbch, bbch.character);
    font_               = bbch.font;
    style_              = bbch.style;
    foreground_colour_  = bbch.foreground_colour;
    background_colour_  = bbch.background_colour;
}

void Format::Reset() {
    trace(2, "[%p]", this);
    font_               = Z_FONT_NORMAL;    // ?
    style_              = Z_STYLE_ROMAN;
    foreground_colour_  = Z_COLOUR_DEFAULT; // ?
    background_colour_  = Z_COLOUR_DEFAULT; // ?
}

void Format::SetFont(z_font font) {
    trace(2, "[%p] %d (%s)", this, font, Format::FontName(font));
    font_ = font;
}

void Format::SetStyle(z_style style) {
    trace(2, "[%p] %d (%s)", this, style, Format::StyleName(style));
    if (style == Z_STYLE_ROMAN) {
        style_ = style;
    } else {
        style_ |= style;
    }
}

void Format::SetForeground(z_colour color) {
    trace(2, "[%p] %d (%s)", this, color, Format::ColorName(color));
    foreground_colour_ = color;
}

void Format::SetBackground(z_colour color) {
    trace(2, "[%p] %d (%s)", this, color, Format::ColorName(color));
    background_colour_ = color;
}

bool Format::operator==(const Format &rhs) const {
    trace(2, "[%p] %p", this, &rhs);
    return font_ == rhs.font_ &&
        style_ == rhs.style_ &&
        foreground_colour_ == rhs.foreground_colour_ &&
        background_colour_ == rhs.background_colour_;
}

bool Format::operator!=(const Format &rhs) const {
    return !(*this == rhs);
}

void set_optional_bool(json_t *obj, const char *name, bool value) {
    if (value) {
        json_object_set_new(obj, name, json_true());
    }
}

void Format::AddJsonProps(json_t *obj) const {
    trace(2, "&p", obj);
    set_optional_bool(obj, "reverse", style_ & Z_STYLE_REVERSE_VIDEO);
    set_optional_bool(obj, "bold", style_ & Z_STYLE_BOLD);
    set_optional_bool(obj, "italic", style_ & Z_STYLE_ITALIC);
    set_optional_bool(obj, "fixed", style_ & Z_STYLE_FIXED_PITCH);
}

std::ostream & operator<<(std::ostream &os, const Format& format) {
    return os << Format::FontName(format.font_, true) << "," << Format::StyleName(format.style_, true) << "," << Format::ColorName(format.foreground_colour_, true) << "," << Format::ColorName(format.background_colour_, true);
}

const char * Format::FontName(z_font font, bool brief) {
    switch (font) {
        case Z_FONT_PREVIOUS_FONT:          return "previous";
        case Z_FONT_NORMAL:                 return brief ? "" : "normal";
        case Z_FONT_PICTURE:                return "picture";
        case Z_FONT_CHARACTER_GRAPHICS:     return "character";
        case Z_FONT_COURIER_FIXED_PITCH:    return "fixed";
    }

    return "(unknown)";
}

const char * Format::StyleName(z_style style, bool brief) {
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

const char * Format::ColorName(z_colour color, bool brief) {
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
        // and in any case is equal to Z_COLOUR_UNDEFINED!
        // case (z_colour)0xffffffff:          return "invalidrgb";
    }

    return "(unknown)";
}
