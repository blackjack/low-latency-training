add_definitions(-march=native)


add_executable(
  read_access

  read_access.cpp
)

target_link_libraries(
  read_access
  benchmark
)
