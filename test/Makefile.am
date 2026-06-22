## Shared pty harness, compiled once and linked into every test program.
check_LIBRARIES        = libeltest.a
libeltest_a_SOURCES    = eltest.c eltest.h
libeltest_a_CPPFLAGS   = -I$(top_srcdir)/include

## One in-process test program per phase, plus eltty, a helper the tmux
## script drives in a real terminal (built, but not a test on its own).
check_PROGRAMS         = basic utf8-move utf8-delete utf8-word utf8-wrap history homeend eltty

## Run the in-process programs, then the tmux-driven script (skips if no tmux).
## TEST_EXTENSIONS+SH_LOG_COMPILER run the script via $(SHELL) regardless of
## its exec bit, which a tarball or git export may not preserve.
TESTS                  = basic utf8-move utf8-delete utf8-word utf8-wrap history homeend \
			 wrap-tmux.sh
TEST_EXTENSIONS        = .sh
SH_LOG_COMPILER        = $(SHELL)
EXTRA_DIST             = wrap-tmux.sh

AM_CPPFLAGS            = -I$(top_srcdir)/include
LDADD                  = libeltest.a $(top_builddir)/src/libeditline.la $(PTY_LIBS)

basic_SOURCES          = basic.c
utf8_move_SOURCES      = utf8-move.c
utf8_delete_SOURCES    = utf8-delete.c
utf8_word_SOURCES      = utf8-word.c
utf8_wrap_SOURCES      = utf8-wrap.c
history_SOURCES        = history.c
eltty_SOURCES          = eltty.c
homeend_SOURCES        = homeend.c
