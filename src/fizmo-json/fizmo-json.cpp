// This file is part of fizmo-json.  Please see LICENSE.md for the license.

extern "C" {
    // #include <stdio.h>
    #include <stdlib.h>
    #include <getopt.h>
    #include "config.h"

    // fizmo includes...
    #include <interpreter/fizmo.h>
    #include <interpreter/config.h>
    #include <tools/filesys.h>  // need this to register filesys... should be better?
}

#include "screen.h"
#include "filesys.h"
#include "util.h"

const char *usageFmt = R"(
OVERVIEW: %1$s, the bot-focused JSON frontend to fizmo.

USAGE: %1$s [options] <storyfile>

OPTIONS:
  -h, --help                  this list
  -V, --version               version of %1$s
  -c, --console               use simple input, not JSON
  -t, --trace-level <level>   trace level for stderr
  -s, --save-file <filename>  name for auto-save/restore file

and <storyfile> is the path to a fizmo-runnable story.

If you are running this directly from the command-line, be aware that it
expects JSON-formatted input, like:

  { "input": "look" }

unless the `--console` option has been provided; %1$s will wait until
it reads a complete JSON object before proceeding.

)";


void usage(int exit_code) {
    fprintf(stderr, usageFmt, PACKAGE_NAME);
    exit(exit_code);
}

std::string saveFile;
void set_save_file(const char *file);
bool set_fizmo_config(const char *key, const char *value);

int main(int argc, char **argv) {
    // trace(1, "%d (%s)", argc, argv[0]);
    // tracex(1, "%s (%s) is barely implmented! (using libfizmo %s)\n", PACKAGE_NAME, PACKAGE_VERSION, LIBFIZMO_VERSION);

    // parse command-line options...
    static struct option long_options[] = {
        { "version",     no_argument,       NULL, 'V' },
        { "help",        no_argument,       NULL, 'h' },
        { "console",     no_argument,       NULL, 'c' },
        { "trace-level", required_argument, NULL, 't' },
        { "save-file",   required_argument, NULL, 's' },
        // { "", required_argument, NULL, '' },
        // { "", required_argument, NULL, '' },
        { NULL,          0,                 NULL, 0 }
    };

    int ch;
    while ((ch = getopt_long(argc, argv, "Vhct:s:", long_options, NULL)) != -1) {
        switch (ch) {

            case 'V':
                fprintf(stderr, "%s %s (using libfizmo %s)\n", PACKAGE_NAME, PACKAGE_VERSION, LIBFIZMO_VERSION);
                exit(0);
                break;

            case 'h':
                usage(0);
                break;

            case 'c':
                screen_use_simple_console_input();
                break;

            case 't':
                set_trace_level(atoi(optarg));
                break;

            case 's':
                set_save_file(optarg);
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
        fprintf(stderr, "No storyfile provided!\n");
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

    // Set config values...
    set_fizmo_config("disable-external-streams", config_true_value);
    set_fizmo_config("disable-restore", config_true_value);
    set_fizmo_config("disable-save", config_true_value);
    set_fizmo_config("disable-sound", config_true_value);

    set_fizmo_config("autosave-filename", saveFile.c_str());
    // set_configuration_value("savegame-path", ".");

    // open a test story file...
    z_file *story = (&bot_filesys)->openfile(storyfile, FILETYPE_DATA, FILEACCESS_READ);

    z_file *restore = NULL;
    if (!saveFile.empty()) {
        restore = (&bot_filesys)->openfile((char*)saveFile.c_str(), FILETYPE_SAVEGAME, FILEACCESS_READ);
    }

    fizmo_start(story, NULL, restore);

    tracex(1, "%s exiting!\n", PACKAGE_NAME);
    return 0;
}

// cast from const char *, because fizmo uses old-school C...
bool set_fizmo_config(const char *key, const char *value) {
    return 0 == set_configuration_value(const_cast<char*>(key), const_cast<char*>(value));
}

void set_save_file(const char *file) {
    trace(0, "\"%s\"", file);
    saveFile = file;
}
