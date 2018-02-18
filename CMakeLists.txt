cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES x86_64)

add_subdirectory(external)

option(NGEN_BUILD_TESTS "Build unit tests." ON)

project(ngen_memory)

include_directories(include)

set(SOURCE_FILES
        source/heap.cpp)

set(INCLUDE_FILES
        include/heap.h
        include/allocation_strategy.h
        include/ngen_memory.h)

add_library(ngen_memory ${SOURCE_FILES} ${INCLUDE_FILES})

if (NGEN_BUILD_TESTS)
    add_subdirectory(test)
endif()
