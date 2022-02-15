cmake_minimum_required(VERSION 3.16)

project(vs-solution-explorer_tests)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    RecentValuesTest.cpp
    StringAlgorithmsTest.cpp
    TestSuite.cpp
    tst_solutionparsertest.cpp
    tst_utilsbom.cpp
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        vs-solution-explorer_lib
        Qt${QT_VERSION_MAJOR}::Test
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TestSolutions
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
