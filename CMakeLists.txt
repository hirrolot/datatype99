cmake_minimum_required(VERSION 3.11.4)
project(datatype99 LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
    metalang99
    URL https://github.com/Hirrolot/metalang99/archive/refs/tags/v1.13.1.tar.gz
)

FetchContent_MakeAvailable(metalang99)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE metalang99)
