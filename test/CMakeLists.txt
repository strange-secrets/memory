project(ngen_memory_tests)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(ngen_memory_tests
        test_heap.cpp)

target_link_libraries(ngen_memory_tests gtest gtest_main)
target_link_libraries(ngen_memory_tests ngen_memory)
