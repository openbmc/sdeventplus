gtest = dependency('gtest', main: true, required: false)
gmock = dependency('gmock', required: false)

if not gtest.found() and not gmock.found()
  assert(build_tests != 'true', 'GMock and GTest are required for tests')
  gtest = disabler()
  gmock = disabler()
endif

tests = [
  'clock',
  'event',
  'exception',
  'internal/sdref',
  'internal/utils',
  'source/base',
  'source/child',
  'source/event',
  'source/io',
  'source/signal',
  'source/time',
  'utility/timer',
]

foreach t : tests
  test(t, executable(t.underscorify(), t + '.cpp',
                     link_with: sdeventplus,
                     dependencies: [gtest, gmock]))
endforeach
