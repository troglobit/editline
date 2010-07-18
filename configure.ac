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

# Restore gnu89 inline semantics on gcc 4.3 and newer
saved_cflags="$CFLAGS"
CFLAGS="$CFLAGS -fgnu89-inline"
AC_COMPILE_IFELSE(AC_LANG_PROGRAM([]), inline_cflags="-fgnu89-inline", inline_cflags="")
CFLAGS="$saved_cflags"

# check for -fvisibility=hidden compiler support (GCC >= 3.4)
saved_cflags="$CFLAGS"
CFLAGS="$CFLAGS -fvisibility=hidden"
AC_COMPILE_IFELSE(AC_LANG_PROGRAM([]),
        [VISIBILITY_CFLAGS="-fvisibility=hidden"
         AC_DEFINE([API_EXPORTED], [__attribute__((visibility("default")))], [Default visibility]) ],
        [ VISIBILITY_CFLAGS=""
         AC_DEFINE([API_EXPORTED], [], [Default visibility]) ],
        ])
CFLAGS="$saved_cflags"

# check for -Wno-pointer-sign compiler support (GCC >= 4)
saved_cflags="$CFLAGS"
CFLAGS="$CFLAGS -Wno-pointer-sign"
AC_COMPILE_IFELSE(AC_LANG_PROGRAM([]),
        nopointersign_cflags="-Wno-pointer-sign", nopointersign_cflags="")
CFLAGS="$saved_cflags"

AM_CFLAGS="-std=gnu99 $inline_cflags -W -Wall -Wundef -Wunused -Wstrict-prototypes -Werror-implicit-function-declaration $nopointersign_cflags -Wshadow"
AC_SUBST(AM_CFLAGS)

AC_OUTPUT(Makefile src/Makefile include/Makefile man/Makefile examples/Makefile)

