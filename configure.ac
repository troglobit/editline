#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(minix_editline, 0.1.0, joachim@vmlinux.org)
AC_CONFIG_SRCDIR([src/editline.c])
AC_CONFIG_HEADER([config.h])

# Prepare automake
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([sgtty.h stdlib.h string.h termio.h termios.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT

# Overrides and types, should be a check.
AC_DEFINE([SYS_UNIX], [1], [Glue definitions for Minix sources.])
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([strchr strdup strrchr])

AC_OUTPUT(Makefile src/Makefile examples/Makefile)
