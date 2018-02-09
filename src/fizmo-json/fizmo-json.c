/* fizmo-json.c
 *
 * This file is part of fizmo-json.  Please see LICENSE.md for the license.
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
#include "util.h"

int main(int argc, char **argv) {
    trace(1, "%d (%s)", argc, argv[0]);

    tracex(1, "%s (%s) is barely implmented! (using libfizmo %s)\n", PACKAGE_NAME, PACKAGE_VERSION, LIBFIZMO_VERSION);


    // TODO: implement command-line processing...
    char *game = argv[1];


    fizmo_register_filesys_interface(&bot_filesys);
    tracex(1, "registered filesys\n");

    // fprintf(stderr, "bot_screen is: %p\n", &bot_screen);
    int r = fizmo_register_screen_interface(&bot_screen);
    tracex(1, "register screen result: %d\n", r);

    // open a test story file...
    z_file *story = (&bot_filesys)->openfile(game, FILETYPE_SAVEGAME, FILEACCESS_READ);

    fizmo_start(story, NULL, NULL);

    tracex(1, "%s exiting!\n", PACKAGE_NAME);
    return 0;
}
