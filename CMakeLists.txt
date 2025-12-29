# CAPS Test Suites
# Unit, integration, regression, conformance, backend, stdlib,
# deadlock, channel graph, determinism, platform, fuzzing, verification.

# Unit tests
add_executable(unit_tests unit_tests.cpp)
target_link_libraries(unit_tests gtest_main)

# Integration tests
add_executable(integration_tests integration_tests.cpp)
target_link_libraries(integration_tests gtest_main)

# Add other test executables
add_executable(deadlock_tests deadlock/deadlock_tests.cpp)
target_link_libraries(deadlock_tests gtest_main)

add_executable(channel_graph_tests channel_graph/channel_graph_tests.cpp)
target_link_libraries(channel_graph_tests gtest_main)

add_executable(determinism_tests determinism/determinism_tests.cpp)
target_link_libraries(determinism_tests gtest_main)

add_executable(backend_tests backend/backend_tests.cpp)
target_link_libraries(backend_tests gtest_main)

add_executable(platform_tests platform/platform_tests.cpp)
target_link_libraries(platform_tests gtest_main)

add_executable(regression_tests regression/regression_tests.cpp)
target_link_libraries(regression_tests gtest_main)