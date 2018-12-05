examples = [
  'delayed_echo',
  'follow',
  'heartbeat',
  'heartbeat_timer',
]

foreach example : examples
  executable(example, example + '.cpp',
             include_directories: includes,
			 implicit_include_directories: false,
             link_with: sdeventplus)
endforeach
