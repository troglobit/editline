# Minix libeditline                                           -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(editline, 1.13.0, troglobit@vmlinux.org)
AC_CONFIG_SRCDIR([src/editline.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Prepare automake
AM_INIT_AUTOMAKE([dist-bzip2 dist-lzma])

# Silent "kernel" style build from automake 1.11 and later.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
LT_INIT

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STAT
AC_HEADER_STDC
# Check for malloc.h instead of AC_FUNC_MALLOC/REALLOC AIX and others
# mess up the traditional malloc check.
AC_CHECK_HEADERS([malloc.h signal.h sgtty.h stdlib.h string.h termio.h termios.h])

# Overrides and types, should be a check.
AC_DEFINE([SYS_UNIX], [1], [Default to UNIX backend, should be detected.])
AC_DEFINE([HIDE], [1], [Enable static keyword, hides internal methods.])
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
# Check for malloc.h instead of AC_FUNC_MALLOC/REALLOC AIX and others
# mess up the traditional malloc check.
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([strchr strdup strrchr tcgetattr perror])

AC_ARG_ENABLE(default-complete,
   [  --enable-default-complete      Enable default completion handler.],
   complete=true; AC_DEFINE(CONFIG_DEFAULT_COMPLETE,, [Define to enable the default completion handler.]))
AM_CONDITIONAL([CONFIG_DEFAULT_COMPLETE], [test x$complete = xtrue])

AC_ARG_ENABLE(arrow-keys,
   [  --enable-arrow-keys            Enable ANSI arrow keys.],
   AC_DEFINE([CONFIG_ANSI_ARROWS],, [Define to include ANSI arrow keys support.]))

AC_ARG_ENABLE(sigstop,
   [  --enable-sigstop               Enable SIGSTOP key.],
   AC_DEFINE([CONFIG_SIGSTOP],, [Define to enable SIGSTOP (Ctrl-Z) key.]))

AC_ARG_ENABLE(terminal-bell,
   [  --enable-terminal-bell         Enable terminal bell on completion.],
   AC_DEFINE([CONFIG_ANNOYING_NOISE],, [Define to enable terminal bell on completion.]))

AC_OUTPUT(Makefile src/Makefile include/Makefile man/Makefile examples/Makefile)

