AUTOMAKE_OPTIONS        = foreign
lib_LTLIBRARIES         = libeditline.la
libeditline_la_SOURCES  = editline.c editline.h sysunix.c unix.h
if CONFIG_DEFAULT_COMPLETE
# Built-in completion handler.
libeditline_la_SOURCES += complete.c
AM_CPPFLAGS             = -DCOMPLETE
endif
