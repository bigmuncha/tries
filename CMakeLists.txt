cmake_minimum_required (VERSION 3.8)

project(Tries)

# Turn on if you need verbose compilation
set(CMAKE_VERBOSE_MAKEFILE OFF)

SET(CMAKE_BUILD_TYPE distribution)
SET(CMAKE_C_COMPILER_ID "GNU")

set(SOURCE_EXE main.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(helpers "./helpers")
set(test "./test")
set(tries "./tries")
include_directories(helpers test tries)

add_executable(main ${SOURCE_EXE} ${helpers} ${test} ${tries})

target_link_libraries(main )
target_compile_options(main PRIVATE -Wall -O0 -g -std=gnu++2b -Wno-error=format-truncation)
