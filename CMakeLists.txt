cmake_minimum_required(VERSION 3.11.4)
project(datatype99 LANGUAGES C)

# Fix the warnings about `DOWNLOAD_EXTRACT_TIMESTAMP` in newer CMake versions.
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

FetchContent_Declare(
    metalang99
    URL https://github.com/Hirrolot/metalang99/archive/refs/tags/v1.13.2.tar.gz
)

FetchContent_MakeAvailable(metalang99)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE metalang99)
