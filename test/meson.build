gtest = dependency('gtest', main: true, disabler: true, required: build_tests)
gmock = dependency('gmock', disabler: true, required: build_tests)

tests = [
  'clock',
  'event',
  'exception',
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
                     implicit_include_directories: false,
                     dependencies: [sdeventplus, gtest, gmock]))
endforeach
