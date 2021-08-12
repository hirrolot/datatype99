cmake_minimum_required(VERSION 3.16)
project(tests LANGUAGES C)

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

function(set_standard TARGET NUM)
  set_target_properties(${TARGET}
                        PROPERTIES C_STANDARD ${NUM} C_STANDARD_REQUIRED ON)
endfunction()

add_executable(tests tests.c)
add_executable(metalang99_compliant metalang99_compliant.c)
add_executable(derive derive.c)
add_executable(record_derive record_derive.c)

set_standard(tests 99)
set_standard(metalang99_compliant 99)

# For `_Static_assert`, which can be used on the same line number.
set_standard(derive 11)
set_standard(record_derive 11)
