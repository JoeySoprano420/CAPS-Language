cmake_minimum_required(VERSION 3.20)
project(caps_frontend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(caps_frontend
  src/main.cpp
  src/util/diag.cpp
  src/util/str.cpp
  src/lexer/lexer.cpp
  src/parser/parser.cpp
  src/sema/types.cpp
  src/sema/sema.cpp
  src/ir/lowering.cpp
  src/analysis/pipeline.cpp
  src/pretty/pretty.cpp
)

target_include_directories(caps_frontend PRIVATE src)
