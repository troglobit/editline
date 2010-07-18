# Minix libeditline                                           -*- Autoconf -*-
#
# Copyright (c) 2008-2010  Joachim Nilsson <troglobit()vmlinux!org>
#
# Process this file with autoconf to produce a configure script.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

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
AC_CHECK_HEADERS([malloc.h signal.h stdlib.h string.h termcap.h termio.h termios.h sgtty.h])

# In termios.h or in sys/ioctl.g?
AC_HEADER_TIOCGWINSZ

# Overrides and types, should be a check.
AC_DEFINE([SYS_UNIX], [1], [Default to UNIX backend, should be detected.])
AC_DEFINE([HIDE], [1], [Enable static keyword, hides internal methods.])
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
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

AC_ARG_ENABLE(termcap,
   [  --enable-termcap               Use the termcap library for terminal size.],
   AC_DEFINE([CONFIG_USE_TERMCAP],, [Define to use the termcap library for terminal size.]))

# Default history size 1, i.e. disabled.
let HIST_SIZE=1
AC_ARG_ENABLE(history,
   [  --enable-history=LINES         Enable scrollback history, default off.],
   let HIST_SIZE=$enableval)
if test $HIST_SIZE -lt 1; then
   let HIST_SIZE=1
fi
AC_DEFINE_UNQUOTED(HIST_SIZE, $HIST_SIZE, [Number of lines in history.])

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

### The following magic for determining the location of termcap library is from GNU Texinfo
### http://www.tex.ac.uk/tex-archive/macros/texinfo/texinfo/configure.ac
# Some GNU/Linux systems (e.g., SuSE 4.3, 1996) don't have curses, but
# rather ncurses.  So we check for it.
TERMLIBS=
# Check for termlib before termcap because Solaris termcap needs libucb.
TERMLIB_VARIANTS="termlib termcap terminfo ncurses curses"
for termlib in ${TERMLIB_VARIANTS}; do
  AC_CHECK_LIB(${termlib}, tgetent,
     [TERMLIBS="${TERMLIBS} -l${termlib}"; break])
done
# don't bother warning on djgpp, it doesn't have a term library, it
# ports each termcap-needing program separately according to its needs.
if test -z "$TERMLIBS" && echo "$build" | grep -v djgpp >/dev/null; then
  AC_MSG_WARN([probably need a terminal library, one of: ${TERMLIB_VARIANTS}])
fi

# Checks for variables.
# HP-UX 9 (at least) needs -lncurses which defines termcap variables PC etc.
AC_MSG_CHECKING(for library with termcap variables)
AC_CACHE_VAL(ac_cv_var_ospeed,
[oldLIBS=$LIBS
for trylib in $termlib ${TERMLIB_VARIANTS}; do
   if test "x$trylib" != "x$termlib"; then
     LIBS="$oldLIBS -l$termlib -l$trylib"
   else
     LIBS="$oldLIBS -l$termlib"
   fi
   AC_LINK_IFELSE([AC_LANG_PROGRAM([], [[#ifdef HAVE_NCURSES_TERMCAP_H
#include <ncurses/termcap.h>
#else
#ifdef HAVE_TERMCAP_H
#include <termcap.h>
#else
#undef PC
char *BC;
char **UP;
char PC;
short ospeed;
#endif
#endif
/* Make sure all variables actually exist.  AIX 4.3 has ospeed but no BC.
   --Andreas Ley <andy@rz.uni-karlsruhe.de> 24 Aug 2000.  */
BC++;
UP++;
PC++;
return ospeed != 0;
]])], [ac_cv_var_ospeed=$trylib; break])
done
LIBS=$oldLIBS
])
AC_MSG_RESULT($ac_cv_var_ospeed)
if test -n "$ac_cv_var_ospeed" \
   && test "x$termlib" != "x$ac_cv_var_ospeed"; then
  TERMLIBS="${TERMLIBS} -l${ac_cv_var_ospeed}"
fi
AC_SUBST(TERMLIBS)#

# Do not use <ncurses/termcap.h> unless we're linking with ncurses.
# Must come after the termlib tests.
if test "x$termlib" = xncurses; then
  # Use AC_CHECK_HEADERS so the HAVE_*_H symbol gets defined.
  AC_CHECK_HEADERS(ncurses/termcap.h)
fi
### End-of-Termcap magic from http://www.tex.ac.uk/tex-archive/macros/texinfo/texinfo/configure.ac

AC_OUTPUT(Makefile src/Makefile include/Makefile man/Makefile examples/Makefile)

