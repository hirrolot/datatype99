cmake_minimum_required(VERSION 3.16)
project(examples LANGUAGES C)

include_directories(.. ../metalang99/include)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -pedantic -ftrack-macro-expansion=0)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  # Enable a standard-confirming C99/C11 preprocessor.
  add_compile_options("/std:c11")
endif()

file(GLOB EXAMPLES *.c)

add_executable(array_in_variant array_in_variant.c)
add_executable(ast ast.c)
add_executable(binary_tree_malloc binary_tree_malloc.c)
add_executable(binary_tree binary_tree.c)
add_executable(token token.c)

add_subdirectory(derive)

foreach(TARGET ${BUILDSYSTEM_TARGETS})
  set_target_properties(TARGET PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
endforeach()
