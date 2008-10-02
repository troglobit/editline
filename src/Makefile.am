AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libedit.a

libedit_a_SOURCES  = editline.c editline.h sysunix.c unix.h
if COMPLETE
# Built-in completion handler.
libedit_a_SOURCES += complete.c
AM_CPPFLAGS       = -DCOMPLETE
endif
