# Makefile.am
#
# This file is part of fizmo-json.  Please see LICENSE.md for the license.

bin_PROGRAMS = fizmo-json
fizmo_json_SOURCES = fizmo-json.cpp filesys.cpp screen.cpp buffer.cpp columns.cpp format.cpp util.cpp

AM_CPPFLAGS = $(libfizmo_CFLAGS) $(jansson_CFLAGS)
LDADD = $(libfizmo_LIBS) $(jansson_LIBS)
