@VALGRIND_CHECK_RULES@
@CODE_COVERAGE_RULES@

gtest_cppflags = $(AM_CPPFLAGS) $(GTEST_CFLAGS) $(GMOCK_CFLAGS)
gtest_ldadd = $(SDEVENTPLUS_LIBS) $(GTEST_LIBS) $(GMOCK_LIBS) -lgmock_main

check_PROGRAMS =
TESTS = $(check_PROGRAMS)

check_PROGRAMS += clock
clock_SOURCES = clock.cpp
clock_CPPFLAGS = $(gtest_cppflags)
clock_LDADD = $(gtest_ldadd)

check_PROGRAMS += event
event_SOURCES = event.cpp
event_CPPFLAGS = $(gtest_cppflags)
event_LDADD = $(gtest_ldadd)

check_PROGRAMS += exception
exception_SOURCES = exception.cpp
exception_CPPFLAGS = $(gtest_cppflags)
exception_LDADD = $(gtest_ldadd)

check_PROGRAMS += internal_sdref
internal_sdref_SOURCES = internal/sdref.cpp
internal_sdref_CPPFLAGS = $(gtest_cppflags)
internal_sdref_LDADD = $(gtest_ldadd)

check_PROGRAMS += internal_utils
internal_utils_SOURCES = internal/utils.cpp
internal_utils_CPPFLAGS = $(gtest_cppflags)
internal_utils_LDADD = $(gtest_ldadd)

check_PROGRAMS += source_base
source_base_SOURCES = source/base.cpp
source_base_CPPFLAGS = $(gtest_cppflags)
source_base_LDADD = $(gtest_ldadd)
