ACLOCAL_AMFLAGS = -I m4

export AM_CPPFLAGS = -I$(abs_builddir)/src -I$(abs_srcdir)/src \
                     $(SYSTEMD_CFLAGS) $(CODE_COVERAGE_CPPFLAGS)
export AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
export AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

export COMMON_LIBS = $(SYSTEMD_LIBS) $(CODE_COVERAGE_LIBS)
export SDEVENTPLUS_LIBS = $(abs_builddir)/src/libsdeventplus.la $(COMMON_LIBS)


EXTRA_DIST = LICENSE MAINTAINERS README.md
SUBDIRS = src example test
