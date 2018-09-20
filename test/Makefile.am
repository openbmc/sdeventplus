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

check_PROGRAMS += internal/sdref
internal_sdref_SOURCES = internal/sdref.cpp
internal_sdref_CPPFLAGS = $(gtest_cppflags)
internal_sdref_LDADD = $(gtest_ldadd)

check_PROGRAMS += internal/utils
internal_utils_SOURCES = internal/utils.cpp
internal_utils_CPPFLAGS = $(gtest_cppflags)
internal_utils_LDADD = $(gtest_ldadd)

check_PROGRAMS += source/base
source_base_SOURCES = source/base.cpp
source_base_CPPFLAGS = $(gtest_cppflags)
source_base_LDADD = $(gtest_ldadd)

check_PROGRAMS += source/child
source_child_SOURCES = source/child.cpp
source_child_CPPFLAGS = $(gtest_cppflags)
source_child_LDADD = $(gtest_ldadd)

check_PROGRAMS += source/event
source_event_SOURCES = source/event.cpp
source_event_CPPFLAGS = $(gtest_cppflags)
source_event_LDADD = $(gtest_ldadd)

check_PROGRAMS += source/io
source_io_SOURCES = source/io.cpp
source_io_CPPFLAGS = $(gtest_cppflags)
source_io_LDADD = $(gtest_ldadd)

check_PROGRAMS += source/signal
source_signal_SOURCES = source/signal.cpp
source_signal_CPPFLAGS = $(gtest_cppflags)
source_signal_LDADD = $(gtest_ldadd)

check_PROGRAMS += source/time
source_time_SOURCES = source/time.cpp
source_time_CPPFLAGS = $(gtest_cppflags)
source_time_LDADD = $(gtest_ldadd)

check_PROGRAMS += utility/timer
utility_timer_SOURCES = utility/timer.cpp
utility_timer_CPPFLAGS = $(gtest_cppflags)
utility_timer_LDADD = $(gtest_ldadd)
