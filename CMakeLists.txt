cmake_minimum_required(VERSION 3.30)
project(hello_css343)

set(CMAKE_CXX_STANDARD 20)
set(SRC_DIR src)

include_directories(${SRC_DIR})
file(GLOB SRC_FILES ${SRC_DIR}/*.cpp)
add_executable(hello_css343 ${SRC_FILES})
