cmake_minimum_required(VERSION 4.4)
project(ImageProcessing VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(main 
    "${CMAKE_SOURCE_DIR}//src/main.cpp")

target_link_libraries(main
    PRIVATE
    "${CMAKE_SOURCE_DIR}//libs//libglfw3.a")

target_include_directories(main 
    PRIVATE 
    "${CMAKE_SOURCE_DIR}//src"
    "${CMAKE_SOURCE_DIR}//include")