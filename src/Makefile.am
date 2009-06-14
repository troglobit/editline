AUTOMAKE_OPTIONS       = foreign
lib_LIBRARIES          = libeditline.a
libeditline_a_SOURCES  = editline.c editline.h sysunix.c unix.h
if COMPLETE
# Built-in completion handler.
libeditline_a_SOURCES += complete.c
AM_CPPFLAGS            = -DCOMPLETE
endif
