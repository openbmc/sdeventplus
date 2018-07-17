@VALGRIND_CHECK_RULES@
@CODE_COVERAGE_RULES@

gtest_cppflags = $(AM_CPPFLAGS) $(GTEST_CFLAGS) $(GMOCK_CFLAGS)
gtest_ldadd = $(SDEVENTPLUS_LIBS) $(GTEST_LIBS) $(GMOCK_LIBS) -lgmock_main

check_PROGRAMS =
TESTS = $(check_PROGRAMS)

check_PROGRAMS += event
event_SOURCES = event.cpp
event_CPPFLAGS = $(gtest_cppflags)
event_LDADD = $(gtest_ldadd)

check_PROGRAMS += exception
exception_SOURCES = exception.cpp
exception_CPPFLAGS = $(gtest_cppflags)
exception_LDADD = $(gtest_ldadd)

check_PROGRAMS += sdref
sdref_SOURCES = sdref.cpp
sdref_CPPFLAGS = $(gtest_cppflags)
sdref_LDADD = $(gtest_ldadd)

check_PROGRAMS += source
source_SOURCES = source.cpp
source_CPPFLAGS = $(gtest_cppflags)
source_LDADD = $(gtest_ldadd)
