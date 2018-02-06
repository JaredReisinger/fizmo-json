/* filesys.c
 *
 * This file is part of fizmo-bot.  Please see LICENSE.md for the license.
 */

#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "util.h"

// fizmo includes...
#include <filesys_interface/filesys_interface.h>


z_file* filesys_openfile(char *filename, int filetype, int fileaccess) {
    trace("%s, %d, %d", filename, filetype, fileaccess);
    return NULL;
}

int filesys_closefile(z_file *file_to_close) {
    trace("%s", file_to_close ? file_to_close->filename : "(NULL)");
    return 0;
}

// Returns -1 on EOF.
int filesys_readchar(z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

// Returns number of bytes read.
size_t filesys_readchars(void *ptr, size_t len, z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_writechar(int ch, z_file *fileref) {
    trace("'%c', %s", ch, fileref ? fileref->filename : "(NULL)");
    return 0;
}

// Returns number of bytes successfully written.
size_t filesys_writechars(void *ptr, size_t len, z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_writestring(char *s, z_file *fileref) {
    trace("\"%s\", %s", s, fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_writeucsstring(z_ucs *s, z_file *fileref) {
    trace("\"%s\", %s", (char*)s, fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_fileprintf(z_file *fileref, char *format, ...) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_vfileprintf(z_file *fileref, char *format, va_list ap) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_filescanf(z_file *fileref, char *format, ...) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_vfilescanf(z_file *fileref, char *format, va_list ap) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

long filesys_getfilepos(z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_setfilepos(z_file *fileref, long seek, int whence) {
    trace("%s, %d, %d", fileref ? fileref->filename : "(NULL)", seek, whence);
    return 0;
}

int filesys_unreadchar(int c, z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_flushfile(z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

time_t filesys_get_last_file_mod_timestamp(z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

int filesys_get_fileno(z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return 0;
}

FILE* filesys_get_stdio_stream(z_file *fileref) {
    trace("%s", fileref ? fileref->filename : "(NULL)");
    return NULL;
}

char* filesys_get_cwd() {
    trace("");
    return NULL;
}

int filesys_ch_dir(char *dirname) {
    trace("%s", dirname);
    return 0;
}

z_dir* filesys_open_dir(char *dirname) {
    trace("%s", dirname);
    return NULL;
}

int filesys_close_dir(z_dir *dirref) {
    trace("[%d]", (int)dirref);
    return 0;
}

int filesys_read_dir(struct z_dir_ent *dir_ent, z_dir *dirref) {
    trace("[%d]", (int)dirref);
    return 0;
}

int filesys_make_dir(char *path) {
    trace("%s", path);
    return 0;
}

bool filesys_is_filename_directory(char *filename) {
    trace("%s", filename);
    return false;
}


struct z_filesys_interface bot_filesys = {
    &filesys_openfile,          // openfile
    &filesys_closefile,         // closefile

    &filesys_readchar,          // readchar
    &filesys_readchars,         // readchars

    &filesys_writechar,         // writechar
    &filesys_writechars,        // writechars
    &filesys_writestring,       // writestring
    &filesys_writeucsstring,    // writeucsstring

    &filesys_fileprintf,        // fileprintf
    &filesys_vfileprintf,       // vfileprintf
    &filesys_filescanf,         // filescanf
    &filesys_vfilescanf,        // vfilescanf

    &filesys_getfilepos,        // getfilepos
    &filesys_setfilepos,        // setfilepos

    &filesys_unreadchar,        // unreadchar
    &filesys_flushfile,         // flushfile
    &filesys_get_last_file_mod_timestamp, // get_last_file_mod_timestamp

    &filesys_get_fileno,        // get_fileno
    &filesys_get_stdio_stream,  // get_stdio_stream

    &filesys_get_cwd,           // get_cwd
    &filesys_ch_dir,            // ch_dir
    &filesys_open_dir,          // open_dir
    &filesys_close_dir,         // close_dir
    &filesys_read_dir,          // read_dir
    &filesys_make_dir,          // make_dir

    &filesys_is_filename_directory,  // is_filename_directory
};
