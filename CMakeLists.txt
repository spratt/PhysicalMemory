cmake_minimum_required (VERSION 3.4)
project (PM)
set (PM_VERSION_MAJOR 1)
set (PM_VERSION_MINOR 0)
add_executable(seq sequential.cc)
add_executable(random random.cc)
add_executable(pathlin pathological_linux.cc)
add_executable(pathbit pathological_bitmap.cc)
add_executable(lall linux_allocator.cc)
set_target_properties(seq PROPERTIES CXX_STANDARD 14)
set_target_properties(random PROPERTIES CXX_STANDARD 14)
set_target_properties(pathlin PROPERTIES CXX_STANDARD 14)
set_target_properties(pathbit PROPERTIES CXX_STANDARD 14)
set_target_properties(lall PROPERTIES CXX_STANDARD 14)
add_custom_target(runseq
  COMMAND seq
)
add_custom_target(runrandom
  COMMAND random
)
add_custom_target(runpathlin
  COMMAND pathlin
)
add_custom_target(runpathbit
  COMMAND pathbit
)
add_custom_target(vgseq
  COMMAND valgrind --leak-check=full ./seq
)
add_custom_target(runlinux
  COMMAND lall
)
