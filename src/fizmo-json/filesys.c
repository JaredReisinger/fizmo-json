/* filesys.c
 *
 * This file is part of fizmo-json.  Please see LICENSE.md for the license.
 */

// #include <string.h>
#include <stdio.h>
#include "config.h"
#include "util.h"

// fizmo includes...
#include <filesys_interface/filesys_interface.h>


z_file* filesys_openfile(char *filename, int filetype, int fileaccess) {
    dbg(__func__);
    fprintf(stderr, "  %s, %d, %d\n", filename, filetype, fileaccess);
    return NULL;
}

int filesys_closefile(z_file *file_to_close) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)file_to_close);
    return 0;
}

// Returns -1 on EOF.
int filesys_readchar(z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

// Returns number of bytes read.
size_t filesys_readchars(void *ptr, size_t len, z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_writechar(int ch, z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d, %c\n", (int)fileref, ch);
    return 0;
}

// Returns number of bytes successfully written.
size_t filesys_writechars(void *ptr, size_t len, z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_writestring(char *s, z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d, %s\n", (int)fileref, s);
    return 0;
}

int filesys_writeucsstring(z_ucs *s, z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_fileprintf(z_file *fileref, char *format, ...) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_vfileprintf(z_file *fileref, char *format, va_list ap) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_filescanf(z_file *fileref, char *format, ...) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_vfilescanf(z_file *fileref, char *format, va_list ap) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

long filesys_getfilepos(z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_setfilepos(z_file *fileref, long seek, int whence) {
    dbg(__func__);
    fprintf(stderr, "  %d, %ld, %d\n", (int)fileref, seek, whence);
    return 0;
}

int filesys_unreadchar(int c, z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_flushfile(z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

time_t filesys_get_last_file_mod_timestamp(z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

int filesys_get_fileno(z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return 0;
}

FILE* filesys_get_stdio_stream(z_file *fileref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)fileref);
    return NULL;
}

char* filesys_get_cwd() {
    dbg(__func__);
    return NULL;
}

int filesys_ch_dir(char *dirname) {
    dbg(__func__);
    fprintf(stderr, "  %s\n", dirname);
    return 0;
}

z_dir* filesys_open_dir(char *dirname) {
    dbg(__func__);
    fprintf(stderr, "  %s\n", dirname);
    return NULL;
}

int filesys_close_dir(z_dir *dirref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)dirref);
    return 0;
}

int filesys_read_dir(struct z_dir_ent *dir_ent, z_dir *dirref) {
    dbg(__func__);
    fprintf(stderr, "  %d\n", (int)dirref);
    return 0;
}

int filesys_make_dir(char *path) {
    dbg(__func__);
    fprintf(stderr, "  %s\n", path);
    return 0;
}

bool filesys_is_filename_directory(char *filename) {
    dbg(__func__);
    fprintf(stderr, "  %s\n", filename);
    return false;
}


struct z_filesys_interface json_filesys = {
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
