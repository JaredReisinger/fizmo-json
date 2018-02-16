# This file is part of fizmo-json.  Please see LICENSE.md for the license.

bin_PROGRAMS = fizmo-json
fizmo_json_SOURCES = fizmo-json.cpp \
	blockbuf.cpp \
	buffer.cpp \
	columns.cpp \
	filesys.cpp \
	format.cpp \
	paragraph.cpp \
	screen.cpp \
	span.cpp \
	util.cpp


fizmo_json_CPPFLAGS = -std=c++14 $(libfizmo_CFLAGS) $(jansson_CFLAGS)
fizmo_json_LDADD = $(libfizmo_LIBS) $(jansson_LIBS)

# -static DOES NOT WORK on macOS, but that's okay; we can use LDFLAGS=-static
# in our Dockerfile specifically to create a statically-linked image in that
# particular case.
# fizmo_json_LDFLAGS = -static
