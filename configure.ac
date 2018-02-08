# configure.ac
#
# This file is part of fizmo-bot.  Please see LICENSE.md for the license.

AC_PREREQ([2.69])

AC_INIT(
 [fizmo-bot],
 [0.0.1],
 [https://github.com/JaredReisinger/fizmo-bot/issues],
 [fizmo-bot],
 [https://github.com/JaredReisinger/fizmo-bot])

AC_COPYRIGHT([Please see LICENSE.md for the license.])

AC_CONFIG_SRCDIR([src/fizmo-bot/fizmo-bot.c])
AC_CONFIG_AUX_DIR([build])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
# AC_PROG_RANLIB

AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_DIRENT
AC_HEADER_TIME
AC_CHECK_HEADERS([unistd.h])

PKG_CHECK_MODULES([libfizmo], [libfizmo >= 0.7.15])
PKG_CHECK_MODULES([jansson], [jansson >= 2.1.0])

AC_CONFIG_HEADERS([src/fizmo-bot/config.h])
AC_CONFIG_FILES([Makefile src/fizmo-bot/Makefile])

AC_OUTPUT
