cmake_minimum_required(VERSION 3.16)

project(QDiffX_Tests LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Test)

# Add the test executable
add_executable(tst_algorithm_manager unit_tests/tst_algorithm_manager.cpp)

# Link the test executable with necessary libraries
target_link_libraries(tst_algorithm_manager PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Widgets
    QDiffXCore # Link against the new core library
)

# Link with the QDiffX project's sources/libraries if necessary
# This assumes QDiffX is built as a library or its headers are accessible
target_include_directories(tst_algorithm_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Add the test to CTest
add_test(NAME QAlgorithmManagerTests COMMAND tst_algorithm_manager) 