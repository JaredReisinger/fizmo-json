# Makefile.am
#
# This file is part of fizmo-bot.  Please see LICENSE.md for the license.

bin_PROGRAMS = fizmo-bot
fizmo_bot_SOURCES = fizmo-bot.c filesys.c screen.c util.c

AM_CFLAGS = $(libfizmo_CFLAGS) $(jansson_CFLAGS)
LDADD = $(libfizmo_LIBS) $(jansson_LIBS)
