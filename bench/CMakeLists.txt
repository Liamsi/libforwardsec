add_executable(benchmarks bench.cpp Benchmark.cpp)
target_link_libraries (benchmarks LINK_PUBLIC forwardsec )
add_dependencies(benchmarks forwardsec)
add_custom_target(bench COMMAND ${CMAKE_BINARY_DIR}/bench/benchmarks PROPERTIES DEPENDS)
add_dependencies(bench benchmarks)