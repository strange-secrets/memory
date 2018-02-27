project(memory_test)

add_executable(memory_test
    test_heap.cpp
)

target_include_directories(memory_test PRIVATE
    $<BUILD_INTERFACE:${gtest_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${gtest_SOURCE_DIR}>
)

target_link_libraries(memory_test PUBLIC
    gtest
    gtest_main
    ngen::memory
)

add_test(NAME memory.Test
    COMMAND ${CMAKE_COMMAND}
    --build ${CMAKE_BINARY_DIR}
    --target memory_test
)
