## Shared pty harness, compiled once and linked into every test program.
check_LIBRARIES        = libeltest.a
libeltest_a_SOURCES    = eltest.c eltest.h
libeltest_a_CPPFLAGS   = -I$(top_srcdir)/include

## One test program per phase, so a regression points at the area.
check_PROGRAMS         = basic utf8-move utf8-delete utf8-word
TESTS                  = $(check_PROGRAMS)

AM_CPPFLAGS            = -I$(top_srcdir)/include
LDADD                  = libeltest.a $(top_builddir)/src/libeditline.la $(PTY_LIBS)

basic_SOURCES          = basic.c
utf8_move_SOURCES      = utf8-move.c
utf8_delete_SOURCES    = utf8-delete.c
utf8_word_SOURCES      = utf8-word.c
