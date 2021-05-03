cmake_minimum_required(VERSION 3.16)
project(tests LANGUAGES C)

include_directories(.. ../metalang99/include)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -pedantic -std=c99
                      -ftrack-macro-expansion=0)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  # Enable a standard-confirming C99/C11 preprocessor.
  add_compile_options("/std:c11")
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
