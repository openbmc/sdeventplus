examples = [
  'delayed_echo',
  'follow',
  'heartbeat',
  'heartbeat_timer',
]

foreach example : examples
  executable(example, example + '.cpp',
             include_directories: includes,
             link_with: sdeventplus)
endforeach
