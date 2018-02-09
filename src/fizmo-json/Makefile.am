# Makefile.am
#
# This file is part of fizmo-json.  Please see LICENSE.md for the license.

bin_PROGRAMS = fizmo-json
fizmo_bot_SOURCES = fizmo-json.c filesys.c screen.c buffer.c format.c util.c

AM_CFLAGS = $(libfizmo_CFLAGS) $(jansson_CFLAGS)
LDADD = $(libfizmo_LIBS) $(jansson_LIBS)
