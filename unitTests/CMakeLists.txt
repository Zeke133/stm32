cmake_minimum_required (VERSION 3.9)

# Project name
project (
    unitTests
    VERSION 0.1.1
    DESCRIPTION "Firmware for vintage motorcycle board computer on STM32F103C8T6 base"
    HOMEPAGE_URL "https://github.com/Zeke133/stm32"
    LANGUAGES
        CXX
)

set(CMAKE_CXX_COMPILER g++)

# Path to GoogleTest Framework - change if needed
set(GOOGLE_TEST_DIR ${PROJECT_SOURCE_DIR}/../../googleTest)
# Path to Unit Test sources
set(TEST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/testSuit)
# Path to tested functionality
set(MODULES_HEADER_DIR ${PROJECT_SOURCE_DIR}/../source/utils/headers)
set(MODULES_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../source/utils/modules)

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

# Executable file
add_executable(unitTests ${SOURCES_TO_BUILD})
# Link with static library
target_link_libraries(unitTests ${GOOGLE_TEST_DIR}/googletest/build/gtest_main.a)
