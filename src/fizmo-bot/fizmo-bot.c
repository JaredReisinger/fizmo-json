/* fizmo-bot.c
 *
 * This file is part of fizmo-bot.  Please see LICENSE.md for the license.
 */

// #include <string.h>
#include <stdio.h>
#include "config.h"

// fizmo includes...
#include <interpreter/fizmo.h>
// #include <interpreter/config.h>
#include <tools/filesys.h>  // need this to register filesys... should be better?

#include "screen.h"
#include "filesys.h"

int main(int argc, char **argv) {
    fprintf(stderr, "%s (%s) is barely implmented! (using libfizmo %s)\n", PACKAGE_NAME, PACKAGE_VERSION, LIBFIZMO_VERSION);

    fizmo_register_filesys_interface(&bot_filesys);
    fprintf(stderr, "registered filesys\n");

    // fprintf(stderr, "bot_screen is: %p\n", &bot_screen);
    int r = fizmo_register_screen_interface(&bot_screen);
    fprintf(stderr, "register screen result: %d\n", r);

    // open a test story file...
    z_file *story = (&bot_filesys)->openfile("curses.z5", FILETYPE_SAVEGAME, FILEACCESS_READ);

    fizmo_start(story, NULL, NULL);

    fprintf(stderr, "%s exiting!\n", PACKAGE_NAME);
    return 0;
}
