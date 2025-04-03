cmake_minimum_required(VERSION 3.30)
project(hello_css343)

set(CMAKE_CXX_STANDARD 20)
set(CXX_ERROR_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_ERROR_FLAGS}")
set(SRC_DIR src)

file(GLOB SRC_FILES ${SRC_DIR}/*.cpp)

add_executable(hello_css343)

target_include_directories(hello_css343 PRIVATE ${SRC_DIR})
target_sources(hello_css343 PRIVATE ${SRC_FILES})
