AM_CPPFLAGS = -I$(top_builddir)/src -I$(top_srcdir)/src \
              $(SYSTEMD_CFLAGS) $(CODE_COVERAGE_CPPFLAGS)
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)
common_ldadd = $(top_builddir)/src/libsdeventplus.la

noinst_PROGRAMS =

noinst_PROGRAMS += follow
follow_SOURCES = follow.cpp
follow_LDADD = $(common_ldadd)
