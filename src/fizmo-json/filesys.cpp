// This file is part of fizmo-json.  Please see LICENSE.md for the license.

extern "C" {
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <dirent.h>
    #include <unistd.h>
}

#include "config.h"
#include "util.h"

extern "C" {
    // fizmo includes...
    #include <interpreter/fizmo.h>
    #include <filesys_interface/filesys_interface.h>
}

z_file* filesys_openfile(char *filename, int filetype, int fileaccess) {
    trace(1, "%s, %d, %d", filename, filetype, fileaccess);

    z_file *result = (z_file *)fizmo_malloc(sizeof(z_file));
    if (!result) {
        return NULL;
    }

    const char *mode;
    switch (fileaccess) {
        case FILEACCESS_READ:   mode = "r"; break;
        case FILEACCESS_WRITE:  mode = "w"; break;
        case FILEACCESS_APPEND: mode = "a"; break;
        default:
            tracex(1, "unexpected fileaccess: %d", fileaccess);
            return NULL;
    }

    FILE *file = fopen(filename, mode);
    if (!file) {
        tracex(1, "open file failed");
        return NULL;
    }

    tracex(1, "open file succeeded!");

    // Use C99 anonymous literal to set the fields, because it also zeros any
    // unmentioned fields!
    *result = (z_file) {
        .file_object = (void *)file,
        .filename = strdup(filename),
        .filetype = filetype,
        .fileaccess = fileaccess,
    };

    return result;
}

int filesys_closefile(z_file *file_to_close) {
    trace(1, "%s", file_to_close ? file_to_close->filename : "(NULL)");

    int result = -1;

    if (file_to_close) {
        if (file_to_close->file_object) {
            result = fclose((FILE *)(file_to_close->file_object));
        }
        free(file_to_close->filename);
        free(file_to_close);
    }
    return result;
}

// Returns -1 on EOF.
int filesys_readchar(z_file *fileref) {
    trace(4, "%s", fileref ? fileref->filename : "(NULL)");
    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    int ch = fgetc((FILE *)(fileref->file_object));
    long pos = ftell((FILE *)(fileref->file_object));
    tracex(4, "read % 3d '%c' (now at pos %d)", ch, ch > 20 ? (char)ch : '?', pos);

    return ch;
}

// Returns number of bytes read.
size_t filesys_readchars(void *ptr, size_t len, z_file *fileref) {
    trace(4, "%s, %d", fileref ? fileref->filename : "(NULL)", len);

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    size_t result = fread(ptr, 1, len, (FILE *)(fileref->file_object));
    tracex(4, "read %d bytes", result);

    return result;
}

int filesys_writechar(int ch, z_file *fileref) {
    trace(1, "'%c', %s", ch, fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    int result = fputc(ch, (FILE *)(fileref->file_object));
    tracex(1, "wrote '%1$c' (%1$d)", result);

    return result;
}

// Returns number of bytes successfully written.
size_t filesys_writechars(void *ptr, size_t len, z_file *fileref) {
    trace(1, "%p, %d, %s", ptr, len, fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    size_t result = fwrite(ptr, 1, len, (FILE *)(fileref->file_object));
    tracex(1, "wrote %d bytes", result);

    return result;
}

int filesys_writestring(char *s, z_file *fileref) {
    trace(1, "\"%s\", %s", s, fileref ? fileref->filename : "(NULL)");
    return filesys_writechars(s, strlen(s), fileref);
}

int filesys_writeucsstring(z_ucs *s, z_file *fileref) {
    trace(1, "\"%s\", %s", (char*)s, fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    // TODO: Use proper UTF8 conversion?  maybe not, as the reading needs to
    // match...

    return 0;
}

int filesys_fileprintf(z_file *fileref, char *format, ...) {
    trace(1, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    return 0;
}

int filesys_vfileprintf(z_file *fileref, char *format, va_list ap) {
    trace(1, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    return 0;
}

int filesys_filescanf(z_file *fileref, char *format, ...) {
    trace(1, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    return 0;
}

int filesys_vfilescanf(z_file *fileref, char *format, va_list ap) {
    trace(1, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    return 0;
}

long filesys_getfilepos(z_file *fileref) {
    trace(3, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    long result = ftell((FILE *)(fileref->file_object));
    tracex(3, "tell returned %d", result);

    return result;
}

int filesys_setfilepos(z_file *fileref, long seek, int whence) {
    trace(4, "%s, %d, %d", fileref ? fileref->filename : "(NULL)", seek, whence);

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    int result = fseek((FILE *)(fileref->file_object), seek, whence);
    long pos = ftell((FILE *)(fileref->file_object));
    tracex(4, "seek returned %d (now at pos %d)", result, pos);

    return result;
}

int filesys_unreadchar(int c, z_file *fileref) {
    trace(4, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    int result = ungetc(c, (FILE *)(fileref->file_object));
    long pos = ftell((FILE *)(fileref->file_object));
    tracex(4, "ungetc returned %d (now at pos %d)", result, pos);

    return result;
}

int filesys_flushfile(z_file *fileref) {
    trace(1, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    int result = fflush((FILE *)(fileref->file_object));
    tracex(1, "fflush returned %d", result);

    return result;
}

time_t filesys_get_last_file_mod_timestamp(z_file *fileref) {
    trace(1, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    return 0;
}

int filesys_get_fileno(z_file *fileref) {
    trace(1, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return -1;
    }

    return 0;
}

FILE* filesys_get_stdio_stream(z_file *fileref) {
    trace(1, "%s", fileref ? fileref->filename : "(NULL)");

    if (!fileref || !fileref->file_object) {
        tracex(1, "no file, bailing");
        return NULL;
    }

    return NULL;
}

char* filesys_get_cwd() {
    trace(2, "");

    // DANGEROUS: assumes that the caller can (and will) call free() on the
    // returned value.
    char *result = getcwd(NULL, 0);
    tracex(1, "getcwd returned %s", result);

    return result;
}

int filesys_ch_dir(char *dirname) {
    trace(1, "%s", dirname);
    return 0;
}

z_dir* filesys_open_dir(char *dirname) {
    trace(1, "%s", dirname);

    z_dir *result = (z_dir *)fizmo_malloc(sizeof(z_dir));
    if (!result) {
        return NULL;
    }

    DIR *dir = opendir(dirname);
    if (!dir) {
        tracex(1, "open dir failed");
        return NULL;
    }

    tracex(1, "open dir succeeded!");

    result->dir_object = dir;

    return result;
}

int filesys_close_dir(z_dir *dirref) {
    trace(1, "[%p]", dirref);

    int result = -1;

    if (!dirref) {
        tracex(1, "no dir, bailing");
        return -1;
    }

    if (dirref->dir_object) {
        result = closedir((DIR *)(dirref->dir_object));
    }

    free(dirref);

    return result;
}

int filesys_read_dir(struct z_dir_ent *dir_ent, z_dir *dirref) {
    trace(4, "[%p]", dirref);

    if (!dirref || !dirref->dir_object) {
        tracex(1, "no dir, bailing");
        return -1;
    }

    struct dirent *entry = readdir((DIR *)(dirref->dir_object));

    if (!entry) {
        tracex(1, "end of directory (or error)");
        return -1;
    }

    // DANGEROUS! The z_dir_ent struct keeps a "live" pointer to the
    // underlying directory entry, rather than its own buffer.  That means
    // it requires the memory to stay intact (presumably until close_dir is
    // called), and it's never explicitly free()d.
    dir_ent->d_name = entry->d_name;
    tracex(1, "dir entry: %s", dir_ent->d_name);

    return 0;
}

int filesys_make_dir(char *path) {
    trace(1, "%s", path);
    return 0;
}

bool filesys_is_filename_directory(char *filename) {
    trace(1, "%s", filename);
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
