libsystemd = dependency('libsystemd')

sdeventplus = library(
  'sdeventplus',
  [
    'sdeventplus/clock.cpp',
    'sdeventplus/event.cpp',
    'sdeventplus/exception.cpp',
    'sdeventplus/internal/sdevent.cpp',
    'sdeventplus/internal/sdref.cpp',
    'sdeventplus/source/base.cpp',
    'sdeventplus/source/child.cpp',
    'sdeventplus/source/event.cpp',
    'sdeventplus/source/io.cpp',
    'sdeventplus/source/signal.cpp',
    'sdeventplus/source/time.cpp',
    'sdeventplus/utility/timer.cpp'
  ],
  include_directories: includes,
  dependencies: libsystemd,
  install: true)

install_headers(
  'sdeventplus/clock.hpp',
  'sdeventplus/event.hpp',
  'sdeventplus/exception.hpp',
  subdir: 'sdeventplus')

install_headers(
  'sdeventplus/internal/sdevent.hpp',
  'sdeventplus/internal/sdref.hpp',
  'sdeventplus/internal/utils.hpp',
  subdir: 'sdeventplus/internal')

install_headers(
  'sdeventplus/source/base.hpp',
  'sdeventplus/source/child.hpp',
  'sdeventplus/source/event.hpp',
  'sdeventplus/source/io.hpp',
  'sdeventplus/source/signal.hpp',
  'sdeventplus/source/time.hpp',
  subdir: 'sdeventplus/source')

install_headers(
  'sdeventplus/test/sdevent.hpp',
  subdir: 'sdeventplus/test')

install_headers(
  'sdeventplus/utility/timer.hpp',
  subdir: 'sdeventplus/utility')
