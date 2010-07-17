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

AC_ARG_ENABLE([default-complete],
   [  --enable-default-complete      Enable default completion handler.],[
   case "${enableval}" in
       yes)
        complete=true
        ;;
       no)
        complete=false
        ;;
       *)
        AC_MSG_ERROR([bad value ${enableval} for --enable-default-complete])
        ;;
   esac],[complete=false])
AM_CONDITIONAL([CONFIG_DEFAULT_COMPLETE], [test x$complete = xtrue])

ac_enable_arrow_keys=no
AC_ARG_ENABLE(arrow-keys,
   [  --enable-arrow-keys            Enable ANSI arrow keys.],
   [ ac_enable_arrow_keys=$enableval ], [ ac_enable_arrow_keys=no])

if test "x${ac_enable_arrow_keys}" = xyes ; then
    AC_DEFINE([CONFIG_ANSI_ARROWS], [],
  [Define to include ANSI arrow keys support.])
fi

ac_enable_sigstop=no
AC_ARG_ENABLE(sigstop,
   [  --enable-sigstop               Enable SIGSTOP key.],
   [ ac_enable_sigstop=$enableval ], [ ac_enable_sigstop=no])

if test "x${ac_enable_sigstop}" = xyes ; then
    AC_DEFINE([CONFIG_SIGSTOP], [],
  [Define to enable SIGSTOP (Ctrl-Z) key.])
fi

ac_enable_terminal_bell=no
AC_ARG_ENABLE(terminal-bell,
   [  --enable-terminal-bell         Enable terminal bell on completion.],
   [ ac_enable_terminal_bell=$enableval ], [ ac_enable_terminal_bell=no])

if test "x${ac_enable_terminal_bell}" = xyes ; then
    AC_DEFINE([CONFIG_ANNOYING_NOISE], [],
  [Define to enable terminal bell on completion.])
fi

AC_OUTPUT(Makefile src/Makefile include/Makefile man/Makefile examples/Makefile)

