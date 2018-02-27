cmake_minimum_required(VERSION 3.1)

enable_testing()

#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_EXTENSIONS ON)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_OSX_ARCHITECTURES x86_64)

add_subdirectory(external)

option(NGEN_BUILD_TESTS "Build unit tests." ON)

project(memory)

set(SOURCE_FILES
    source/heap.cpp
)

set(INCLUDE_FILES
    include/heap.h
    include/allocation_strategy.h
    include/ngen_memory.h
)

add_library(memory STATIC
    ${SOURCE_FILES}
    ${INCLUDE_FILES}
)
add_library(ngen::memory ALIAS memory)

target_compile_features(memory
    PUBLIC
        cxx_strong_enums
    PRIVATE
        cxx_auto_type
)

target_include_directories(memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/ngen/memory>
)

install(TARGETS memory EXPORT memory_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(EXPORT memory_targets
    FILE memory_targets.cmake
    NAMESPACE ngen::
    DESTINATION lib/cmake/ngen
)

if (NGEN_BUILD_TESTS)
    add_subdirectory(test)
endif()
