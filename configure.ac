# configure.ac
#
# This file is part of fizmo-json.  Please see LICENSE.md for the license.

AC_PREREQ([2.69])

AC_INIT(
 [fizmo-json],
 [0.0.1],
 [https://github.com/JaredReisinger/fizmo-json/issues],
 [fizmo-json],
 [https://github.com/JaredReisinger/fizmo-json])

AC_COPYRIGHT([Please see LICENSE.md for the license.])

AC_CONFIG_SRCDIR([src/fizmo-json/fizmo-json.c])
AC_CONFIG_AUX_DIR([build])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
# AC_PROG_RANLIB

PKG_CHECK_MODULES([libfizmo], [libfizmo >= 0.7.15])

AC_CONFIG_FILES([Makefile src/fizmo-json/Makefile])

AC_OUTPUT
