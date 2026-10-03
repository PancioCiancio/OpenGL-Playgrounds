add_executable(test_buddy_alloc
    "${CMAKE_SOURCE_DIR}/test/buddy_allocator/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/memory/buddy_allocator.cpp")

# This generator expression perfectly applies the DEBUG macro 
# only when -DCMAKE_BUILD_TYPE=Debug is passed.
target_compile_definitions(test_buddy_alloc
    PRIVATE
    "$<$<CONFIG:DEBUG>:DEBUG>")

target_include_directories(test_buddy_alloc
    PRIVATE
    "${CMAKE_SOURCE_DIR}//src")