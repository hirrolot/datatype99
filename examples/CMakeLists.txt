cmake_minimum_required(VERSION 3.0)
project(examples LANGUAGES C)

include_directories(.. ../epilepsy/include)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-Weverything)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -ftrack-macro-expansion=0)
endif()

add_executable(binary_tree binary_tree.c)
add_executable(binary_tree_malloc binary_tree_malloc.c)
add_executable(ast ast.c)

foreach(TARGET ${BUILDSYSTEM_TARGETS})
  set_target_properties(TARGET PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
endforeach()
