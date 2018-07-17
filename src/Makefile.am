AM_CPPFLAGS = $(SYSTEMD_CFLAGS) $(CODE_COVERAGE_CPPFLAGS)
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

nobase_include_HEADERS =
pkgconfig_DATA = sdeventplus.pc
lib_LTLIBRARIES = libsdeventplus.la
libsdeventplus_la_SOURCES =
libsdeventplus_la_LIBADD = $(SYSTEMD_LIBS) $(CODE_COVERAGE_LIBS)

nobase_include_HEADERS += sdeventplus/event.hpp
libsdeventplus_la_SOURCES += sdeventplus/event.cpp

nobase_include_HEADERS += sdeventplus/exception.hpp
libsdeventplus_la_SOURCES += sdeventplus/exception.cpp

nobase_include_HEADERS += sdeventplus/source.hpp
libsdeventplus_la_SOURCES += sdeventplus/source.cpp

nobase_include_HEADERS += sdeventplus/internal/sdevent.hpp
libsdeventplus_la_SOURCES += sdeventplus/internal/sdevent.cpp

nobase_include_HEADERS += sdeventplus/internal/sdref.hpp

nobase_include_HEADERS += sdeventplus/test/sdevent.hpp
