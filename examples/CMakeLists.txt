cmake_minimum_required(VERSION 3.16)
project(examples LANGUAGES C)

include_directories(.. ../metalang99/include)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  # For the reason behind -Wno-return-type, see
  # <https://github.com/Hirrolot/datatype99/issues/9>.
  add_compile_options(-Wall -Wextra -pedantic -std=c99
                      -ftrack-macro-expansion=0 -Wno-return-type)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-fmacro-backtrace-limit=1 -Wno-return-type)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  # Enable a standard-conforming C99/C11 preprocessor.
  add_compile_options("/std:c11")
elseif(CMAKE_C_COMPILER_ID STREQUAL "TinyCC")
  add_compile_definitions(ML99_ALLOW_POOR_DIAGNOSTICS)
endif()

add_executable(array_in_variant array_in_variant.c)
add_executable(ast ast.c)
add_executable(binary_tree_malloc binary_tree_malloc.c)
add_executable(binary_tree binary_tree.c)
add_executable(token token.c)

add_subdirectory(derive)

foreach(TARGET ${BUILDSYSTEM_TARGETS})
  set_target_properties(TARGET PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
endforeach()
