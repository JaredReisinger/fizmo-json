// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_SCREEN_H
#define FIZMO_JSON_SCREEN_H

extern "C" {
    #include <screen_interface/screen_interface.h>
}


extern struct z_screen_interface bot_screen;

extern void screen_use_simple_console_input();


#endif // FIZMO_JSON_SCREEN_H
