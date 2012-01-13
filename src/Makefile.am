AM_CFLAGS = \
  -I$(top_srcdir)/src \
  -I$(top_srcdir)/contrib/jansson/src \
  @AM_CFLAGS@

LIBS="-ljansson $LIBS"

lib_LIBRARIES = librapid.a

# where to install the headers on the system
librapid_adir = $(includedir)/rapid

# the list of header files that belong to the library (to be installed later)
librapid_a_HEADERS =    \
                        api.h \
                        message.h

# the sources to add to the library and to add to the source distribution
librapid_a_SOURCES =    \
                        $(librapid_a_HEADERS) \
                        api.c \
                        message.c
