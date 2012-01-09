AM_CFLAGS = \
  -I$(top_srcdir)/src \
  -I$(top_srcdir)/contrib/jansson/src \
  -L$(top_srcdir)/contrib/jansson/src/.libs \
  @AM_CFLAGS@

LIBS=-ljansson

bin_PROGRAMS = rapid_test

rapid_test_SOURCES = test.c

# the additional libraries needed to link
rapid_test_LDFLAGS = -static
rapid_test_LDADD =  $(top_builddir)/src/librapid.a \
                    $(top_builddir)/contrib/jansson/src/.libs/libjansson.a \
                    $(AM_LDFLAGS)
