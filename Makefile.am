ACLOCAL_AMFLAGS = -I m4

# Ignore system headers
CODE_COVERAGE_IGNORE_PATTERN = '/include/*' '/usr/include/*' '$(includedir)/*'
# Ignore the real implementation sources for sd_event
CODE_COVERAGE_IGNORE_PATTERN += \
    '$(abs_builddir)/src/sdeventplus/internal/sdevent.*'
export CODE_COVERAGE_IGNORE_PATTERN
CODE_COVERAGE_GENHTML_OPTIONS = $(CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT)
# Don't generate the absolute path for each file in the HTML output
CODE_COVERAGE_GENHTML_OPTIONS += --prefix $(abs_srcdir) --prefix $(abs_builddir)
export CODE_COVERAGE_GENHTML_OPTIONS

export AM_CPPFLAGS = -I$(abs_builddir)/src -I$(abs_srcdir)/src \
                     $(SYSTEMD_CFLAGS) $(CODE_COVERAGE_CPPFLAGS)
export AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
export AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

export COMMON_LIBS = $(SYSTEMD_LIBS) $(CODE_COVERAGE_LIBS)
export SDEVENTPLUS_LIBS = $(abs_builddir)/src/libsdeventplus.la $(COMMON_LIBS)


EXTRA_DIST = LICENSE MAINTAINERS README.md
SUBDIRS = src example test
