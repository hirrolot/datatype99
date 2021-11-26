cmake_minimum_required(VERSION 3.16)
project(tests LANGUAGES C)

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

function(set_standard TARGET NUM)
  set_target_properties(${TARGET}
                        PROPERTIES C_STANDARD ${NUM} C_STANDARD_REQUIRED ON)
endfunction()

add_executable(tests tests.c)
add_executable(metalang99_compliant metalang99_compliant.c)
add_executable(derive derive.c)
add_executable(record_derive record_derive.c)
add_executable(version version.c)

set_standard(tests 99)
set_standard(metalang99_compliant 99)
set_standard(version 99)

# For `_Static_assert`, which can be used on the same line number.
set_standard(derive 11)
set_standard(record_derive 11)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. build)

get_property(
  TESTS
  DIRECTORY .
  PROPERTY BUILDSYSTEM_TARGETS)

foreach(TARGET ${TESTS})
  target_link_libraries(${TARGET} datatype99)
endforeach()
