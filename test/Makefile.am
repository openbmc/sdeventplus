@VALGRIND_CHECK_RULES@
@CODE_COVERAGE_RULES@

AM_CPPFLAGS = -I$(top_builddir)/src -I$(top_srcdir)/src \
              $(GTEST_CFLAGS) $(GMOCK_CFLAGS) $(SYSTEMD_CFLAGS) \
              $(CODE_COVERAGE_CPPFLAGS)
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)
test_ldadd = $(top_builddir)/src/libsdeventplus.la $(SYSTEMD_LIBS) \
             $(CODE_COVERAGE_LIBS)
gtest_ldadd = $(test_ldadd) $(GTEST_LIBS) $(GMOCK_LIBS) -lgmock_main

check_PROGRAMS =
TESTS = $(check_PROGRAMS)

check_PROGRAMS += event
event_SOURCES = event.cpp
event_LDADD = $(gtest_ldadd)

check_PROGRAMS += exception
exception_SOURCES = exception.cpp
exception_LDADD = $(gtest_ldadd)

check_PROGRAMS += sdref
sdref_SOURCES = sdref.cpp
sdref_LDADD = $(gtest_ldadd)
