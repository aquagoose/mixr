cmake_minimum_required(VERSION 3.25)
project(mixr)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(GLOB MIXR_SRC CONFIGURE_DEPENDS
        include/mixr/*.h
        src/mixr/*.h
        src/mixr/*.c
)

add_library(mixr SHARED ${MIXR_SRC})
target_include_directories(mixr PUBLIC include/)