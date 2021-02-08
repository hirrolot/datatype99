cmake_minimum_required(VERSION 3.16)
project(tests LANGUAGES C)

include_directories(.. ../metalang99/include)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -ftrack-macro-expansion=0)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  # Enable a standard-confirming C99/C11 preprocessor.
  add_compile_options("/std:c11")
endif()

add_executable(tests tests.c)
set_target_properties(tests PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
