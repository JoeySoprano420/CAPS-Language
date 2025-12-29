# CAPS-Language Build System
# Enhanced with tools: formatter, linter, package manager, debugger, LSP, build, profiler, docgen, analyzer, perf, simulator.
# Tests: unit, integration, deadlock, channel_graph, determinism, backend, platform, regression, fuzzing, verification.

cmake_minimum_required(VERSION 3.15)
project(CAPS)

# Main compiler
add_subdirectory(src)

# Tools
add_subdirectory(tools/formatter)
add_subdirectory(tools/linter)
add_subdirectory(tools/pm)
add_subdirectory(tools/debugger)
add_subdirectory(tools/lsp)
add_subdirectory(tools/build)
add_subdirectory(tools/profiler)
add_subdirectory(tools/docgen)
add_subdirectory(tools/analyzer)
add_subdirectory(tools/perf)
add_subdirectory(tools/simulator)

# Tests
add_subdirectory(tests)