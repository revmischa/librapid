#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([librapid], [0.1], [mischa@int80.biz])

# support --target option for cross-compiling
AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM

AC_CONFIG_SRCDIR([src/api.h])

AC_CONFIG_SUBDIRS([contrib/jansson])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([contrib/jansson/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/bin/Makefile])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_RANLIB

AM_INIT_AUTOMAKE([1.9])

AC_LANG([C])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

AC_OUTPUT
