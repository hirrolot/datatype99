cmake_minimum_required(VERSION 3.0.2)
project(datatype99)

include(FetchContent)

FetchContent_Declare(
    metalang99
    GIT_REPOSITORY https://github.com/Hirrolot/metalang99.git
    GIT_TAG master # TODO: change it to vx.y.z.
)

FetchContent_MakeAvailable(metalang99)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE metalang99)
