cmake_minimum_required(VERSION 3.30)
project(hello_css343)

set(MAIN_EXECUTABLE_NAME "${PROJECT_NAME}")
set(TEST_EXECUTABLE_NAME "${PROJECT_NAME}Test")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_ERROR_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_ERROR_FLAGS}")

set(MAIN_SRC_DIR src)
set(TEST_SRC_DIR test)

# Source Files
set(MAIN_SRC_FILE src/main.cpp)
set(MAIN_SRC_FILES)
set(TEST_SRC_FILES test/tests.cpp)

add_executable(${MAIN_EXECUTABLE_NAME})

target_include_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${MAIN_SRC_DIR})
target_sources(${MAIN_EXECUTABLE_NAME} PRIVATE ${MAIN_SRC_FILE} ${MAIN_SRC_FILES})

# Testing
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(${TEST_EXECUTABLE_NAME})
target_include_directories(${TEST_EXECUTABLE_NAME} PRIVATE ${MAIN_SRC_DIR} ${TEST_SRC_DIR})
target_sources(${TEST_EXECUTABLE_NAME} PRIVATE ${MAIN_SRC_FILES} ${TEST_SRC_FILES})
target_link_libraries(${TEST_EXECUTABLE_NAME} PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${TEST_EXECUTABLE_NAME})
