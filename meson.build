project('sdeventplus', 'cpp',
        version: '0.1',
        default_options: ['cpp_std=c++17'])
includes = include_directories('src')

subdir('src')

build_tests = get_option('tests')
build_examples = get_option('examples')

if build_examples
  subdir('example')
endif
if build_tests != 'false'
  subdir('test')
endif
