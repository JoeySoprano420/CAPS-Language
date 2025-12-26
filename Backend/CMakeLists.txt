cmake_minimum_required(VERSION 3.16)
project(caps_backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(caps_backend STATIC
  src/backend/value.cpp
  src/backend/channel.cpp
  src/backend/ir.cpp
  src/backend/runtime.cpp
  src/backend/eval.cpp
  src/backend/exec.cpp
  src/backend/scheduler.cpp
  src/backend/trace.cpp
)

target_include_directories(caps_backend PUBLIC src)
