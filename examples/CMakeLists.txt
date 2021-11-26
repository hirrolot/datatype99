cmake_minimum_required(VERSION 3.16)
project(examples LANGUAGES C)

include_directories(.. ../metalang99/include)

# For the reason behind -Wno-return-type, see
# <https://github.com/Hirrolot/datatype99/issues/9>.
set(GCC_OPTIONS -Wall -Wextra -pedantic -ftrack-macro-expansion=0
                -Wno-return-type -fsanitize=address)

set(CLANG_OPTIONS -fmacro-backtrace-limit=1 -Wno-return-type -fsanitize=address)

# Enable a standard-conforming C99/C11 preprocessor.
set(MSVC_OPTIONS /std:c11)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(${GCC_OPTIONS})
  add_link_options(-fsanitize=address)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  add_compile_options(${CLANG_OPTIONS})
  add_link_options(-fsanitize=address)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(${MSVC_OPTIONS})
elseif(CMAKE_C_COMPILER_ID STREQUAL "TinyCC")
  add_compile_definitions(ML99_ALLOW_POOR_DIAGNOSTICS)
endif()

add_executable(array_in_variant array_in_variant.c)
add_executable(ast ast.c)
add_executable(binary_tree_malloc binary_tree_malloc.c)
add_executable(binary_tree binary_tree.c)
add_executable(token token.c)

add_subdirectory(derive)

add_subdirectory(.. build)

get_property(
  EXAMPLES
  DIRECTORY .
  PROPERTY BUILDSYSTEM_TARGETS)

get_property(
  DERIVE_EXAMPLES
  DIRECTORY derive
  PROPERTY BUILDSYSTEM_TARGETS)

foreach(TARGET ${EXAMPLES};${DERIVE_EXAMPLES})
  target_link_libraries(${TARGET} datatype99)
  set_target_properties(${TARGET} PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED
                                                           ON)
endforeach()
