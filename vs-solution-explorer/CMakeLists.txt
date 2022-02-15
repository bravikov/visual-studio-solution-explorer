cmake_minimum_required(VERSION 3.16)

project(vs-solution-explorer)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Core Gui REQUIRED)

find_package(fmt REQUIRED)

set(PROJECT_SOURCES
    SolutionParser/FileSolutionSource.h
    SolutionParser/FileSolutionSource.cpp
    SolutionParser/ISolution.h
    SolutionParser/ISolution.cpp
    SolutionParser/ISolutionGlobal.h
    SolutionParser/ISolutionGlobal.cpp
    SolutionParser/ISolutionParser.h
    SolutionParser/ISolutionParser.cpp
    SolutionParser/ISolutionProject.h
    SolutionParser/ISolutionProject.cpp
    SolutionParser/ISolutionSource.h
    SolutionParser/ISolutionSource.cpp
    SolutionParser/Solution.h
    SolutionParser/Solution.cpp
    SolutionParser/SolutionGlobal.h
    SolutionParser/SolutionGlobal.cpp
    SolutionParser/SolutionParser.h
    SolutionParser/SolutionParser.cpp
    SolutionParser/SolutionProject.h
    SolutionParser/SolutionProject.cpp
    SolutionParser/SolutionProjectType.h
    SolutionParser/SolutionProjectType.cpp
    SolutionTreeModel.h
    SolutionTreeModel.cpp
    Utils/Bom.h
    Utils/Bom.cpp
    Utils/RecentValues.h
    Utils/RecentValues.cpp
    Utils/StringAlgorithms.h
    Utils/StringAlgorithms.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
)

add_library(${PROJECT_NAME}_lib STATIC
    ${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_NAME}_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_lib
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        fmt::fmt
)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_NAME}_lib
)
