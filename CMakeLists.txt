cmake_minimum_required(VERSION 3.25)
project(mixr)

option(MIXR_BUILD_TESTS "Build the mixr tests" ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE MIXR_SRC CONFIGURE_DEPENDS
        include/mixr/*.h
        src/mixr/*.h
        src/mixr/*.c
)

add_library(mixr SHARED ${MIXR_SRC})
target_include_directories(mixr PUBLIC include/)

add_subdirectory(lib/SDL)
target_link_libraries(mixr PUBLIC SDL3::SDL3)

if (MIXR_BUILD_TESTS)
    add_subdirectory(tests)
endif ()