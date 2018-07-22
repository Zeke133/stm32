cmake_minimum_required (VERSION 2.6)

# Project name
project (UnitTests)

# Path to GoogleTest Framework - change if needed
set(GOOGLE_TEST_DIR "D:/Projects/googleTest")
# Path to Unit Test sources
set(TEST_SOURCE_DIR source/tests/unit)
# Path to tested functionality
set(MODULES_HEADER_DIR source/utils/headers)
set(MODULES_SOURCE_DIR source/utils/modules)

# List of source files to build
set(SOURCES_TO_BUILD
    ${TEST_SOURCE_DIR}/runAllTests
    ${TEST_SOURCE_DIR}/crcTest
    ${TEST_SOURCE_DIR}/convertationTest
    ${MODULES_SOURCE_DIR}/crc
    ${MODULES_SOURCE_DIR}/convertation
    )

# Path to includes
include_directories (${GOOGLE_TEST_DIR}/googletest/include)
include_directories (${MODULES_HEADER_DIR})

#add_definitions(-DSOME_IMPORTANT_DEFINITION)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

# Executable file
add_executable(UnitTests ${SOURCES_TO_BUILD})
# Link with static library
target_link_libraries(UnitTests ${GOOGLE_TEST_DIR}/googletest/build/gtest_main.a)

