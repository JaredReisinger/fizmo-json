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


AM_CPPFLAGS = -std=c++14 $(libfizmo_CFLAGS) $(jansson_CFLAGS)
LDADD = $(libfizmo_LIBS) $(jansson_LIBS)
