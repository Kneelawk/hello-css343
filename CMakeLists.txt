cmake_minimum_required(VERSION 3.30)
project(hello_css343)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/)
file(GLOB SRC_FILES src/*.cpp)
add_executable(hello_css343 ${SRC_FILES})
