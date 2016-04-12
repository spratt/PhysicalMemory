cmake_minimum_required (VERSION 3.4)
project (PM)
set (PM_VERSION_MAJOR 1)
set (PM_VERSION_MINOR 0)
add_executable(main main.cc)
add_executable(lall linux_allocator.cc)
set_target_properties(main PROPERTIES CXX_STANDARD 14)
set_target_properties(lall PROPERTIES CXX_STANDARD 14)
add_custom_target(run
  COMMAND main
)
add_custom_target(valgrind
  COMMAND valgrind --leak-check=full ./main
)
add_custom_target(runlinux
  COMMAND lall
)
