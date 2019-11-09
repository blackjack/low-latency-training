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

add_executable(
  associated_containers
  associated_containers.cpp
)

target_link_libraries(
  associated_containers
  benchmark
)


add_executable(
  ipc
  ipc.cpp
)

target_link_libraries(
  ipc
  benchmark
  pthread
)


add_executable(
  volatile
  volatile.cpp
)

target_link_libraries(
  volatile
  benchmark
)

