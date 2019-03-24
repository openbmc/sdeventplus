noinst_PROGRAMS =

if BUILD_EXAMPLES

noinst_PROGRAMS += delayed_echo
delayed_echo_SOURCES = delayed_echo.cpp
delayed_echo_LDADD = $(SDEVENTPLUS_LIBS)

noinst_PROGRAMS += follow
follow_SOURCES = follow.cpp
follow_LDADD = $(SDEVENTPLUS_LIBS)

noinst_PROGRAMS += heartbeat
heartbeat_SOURCES = heartbeat.cpp
heartbeat_LDADD = $(SDEVENTPLUS_LIBS) $(STDPLUS_LIBS)

noinst_PROGRAMS += heartbeat_timer
heartbeat_timer_SOURCES = heartbeat_timer.cpp
heartbeat_timer_LDADD = $(SDEVENTPLUS_LIBS) $(STDPLUS_LIBS)

endif
