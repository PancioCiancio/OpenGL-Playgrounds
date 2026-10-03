cmake_minimum_required(VERSION 3.20)
project(ImageProcessing VERSION 1.0 LANGUAGES CXX)

# Export the compile commands database for VSCode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main 
    "${CMAKE_SOURCE_DIR}/src/main.cpp")

# This generator expression perfectly applies the DEBUG macro 
# only when -DCMAKE_BUILD_TYPE=Debug is passed.
target_compile_definitions(main
    PRIVATE
    "$<$<CONFIG:DEBUG>:DEBUG>")

target_link_libraries(main
    PRIVATE
    "${CMAKE_SOURCE_DIR}//libs//libglfw3.a")

target_include_directories(main 
    PRIVATE 
    "${CMAKE_SOURCE_DIR}//src"
    "${CMAKE_SOURCE_DIR}//include"
    "${CMAKE_SOURCE_DIR}//third_party"
    "${CMAKE_SOURCE_DIR}//assets")


# TESTS

add_subdirectory(test)