add_definitions(-march=native)


add_executable(
  read_access
  read_access.cpp
)

target_link_libraries(
  read_access
  benchmark
  papipp
)

add_executable(
  papi_benchmark
  papi_benchmark.cpp
)

target_link_libraries(
  papi_benchmark
  benchmark
  papipp
)
