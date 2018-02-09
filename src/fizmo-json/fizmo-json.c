/* fizmo-json.c
 *
 * This file is part of fizmo-json.  Please see LICENSE.md for the license.
 */

// #include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "config.h"

// fizmo includes...
#include <interpreter/fizmo.h>
// #include <interpreter/config.h>
#include <tools/filesys.h>  // need this to register filesys... should be better?

#include "screen.h"
#include "filesys.h"
#include "util.h"

void usage(int exit_code) {
    fprintf(stderr, "\n"
        "OVERVIEW: %1$s, the bot-focused JSON frontend to fizmo.\n"
        "\n"
        "USAGE: %1$s [options] <storyfile>\n"
        "\n"
        "OPTIONS:\n"
        "  -h, --help                  this list\n"
        "  -V, --version               version of %1$s\n"
        "  -c, --console               use simple input, not JSON\n"
        "  -t, --trace-level <level>   trace level for stderr\n"
        "\n"
        "and <storyfile> is the path to a fizmo-runnable story.\n"
        "\n"
        "If you are running this directly from the command-line, be aware\n"
        "that it expects JSON-formatted input, like:\n"
        "\n"
        "  { \"input\": \"look\" }\n"
        "\n"
        "%1$s will wait until it reads a complete JSON object before\n"
        "proceeding.\n"
        "\n",
        PACKAGE_NAME);
    exit(exit_code);
}

int main(int argc, char **argv) {
    // trace(1, "%d (%s)", argc, argv[0]);
    // tracex(1, "%s (%s) is barely implmented! (using libfizmo %s)\n", PACKAGE_NAME, PACKAGE_VERSION, LIBFIZMO_VERSION);

    // parse command-line options...
    static struct option long_options[] = {
        { "version",     no_argument,       NULL, 'V' },
        { "help",        no_argument,       NULL, 'h' },
        { "console",     no_argument,       NULL, 'c' },
        { "trace-level", required_argument, NULL, 't' },
        // { "", required_argument, NULL, '' },
        // { "", required_argument, NULL, '' },
        { NULL,          0,                 NULL, 0 }
    };

    int ch;
    while ((ch = getopt_long(argc, argv, "Vhct:", long_options, NULL)) != -1) {
        switch (ch) {

            case 'V':
                fprintf(stderr, "%s %s (using libfizmo %s)\n", PACKAGE_NAME, PACKAGE_VERSION, LIBFIZMO_VERSION);
                exit(0);
                break;

            case 'h':
                usage(0);
                break;

            case 'c':
                screen_use_console_input();
                break;

            case 't':
                set_trace_level(atoi(optarg));
                break;

            default:
                usage(-1);
        }
    }

    tracex(2, "done with getopt_long, optind: %d", optind);

    argc -= optind;
    argv += optind;

    for (int i = 0; i < argc; i++) {
        tracex(1, "arg %d: \"%s\"", i, argv[i]);
    }

    if (argc != 1) {
        usage(-2);
    }

    // TODO: implement command-line processing...
    char *storyfile = argv[0];
    tracex(1, "using storyfile: %s", storyfile);


    fizmo_register_filesys_interface(&bot_filesys);
    tracex(1, "registered filesys");

    // fprintf(stderr, "bot_screen is: %p\n", &bot_screen);
    int r = fizmo_register_screen_interface(&bot_screen);
    tracex(1, "register screen result: %d", r);

    // open a test story file...
    z_file *story = (&bot_filesys)->openfile(storyfile, FILETYPE_SAVEGAME, FILEACCESS_READ);

    fizmo_start(story, NULL, NULL);

    tracex(1, "%s exiting!\n", PACKAGE_NAME);
    return 0;
}
