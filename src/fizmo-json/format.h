// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_FORMAT_H
#define FIZMO_JSON_FORMAT_H

extern "C" {
    #include <jansson.h>
}

#include "blockbuf.h"


class Format {
  public:
    Format();
    Format(const Format &format);
    Format(const struct blockbuf_char& bbch);

    bool operator==(const Format &rhs) const;
    bool operator!=(const Format &rhs) const;

    void Reset();
    void SetFont(z_font font);
    void SetStyle(z_style style);
    void SetForeground(z_colour color);
    void SetBackground(z_colour color);

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const Format& format);

    void AddJsonProps(json_t *obj) const;

    // utility helpers...
    static const char * FontName(z_font font, bool brief = false);
    static const char * StyleName(z_style style, bool brief = false);
    static const char * ColorName(z_colour color, bool brief = false);


  private:
    z_font      font_;
    z_style     style_;
    z_colour    foreground_colour_;
    z_colour    background_colour_;
};


#endif // FIZMO_JSON_FORMAT_H
