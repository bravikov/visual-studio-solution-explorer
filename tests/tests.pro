QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++20
CONFIG += file_copies

COPIES += testSolutions
testSolutions.files = $$files(TestSolutions/*.sln)
testSolutions.path = $$OUT_PWD/TestSolutions

TEMPLATE = app

QMAKE_CXXFLAGS += -Werror -Wnon-virtual-dtor -Wsuggest-override

SOURCES += \
    StringAlgorithmsTest.cpp \
    TestSuite.cpp \
    main.cpp \
    tst_solutionparsertest.cpp \
    tst_utilsbom.cpp \

MAIN_PROJECT_DIR = ../vs-solution-explorer

INCLUDEPATH += $$MAIN_PROJECT_DIR

SOURCES += \
    $$MAIN_PROJECT_DIR/SolutionParser/FileSolutionSource.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolution.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolutionGlobal.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolutionParser.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolutionProject.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolutionSource.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/Solution.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/SolutionGlobal.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/SolutionParser.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/SolutionProject.cpp \
    $$MAIN_PROJECT_DIR/SolutionParser/SolutionProjectType.cpp \
    $$MAIN_PROJECT_DIR/Utils/Bom.cpp \
    $$MAIN_PROJECT_DIR/Utils/StringAlgorithms.cpp \

HEADERS += \
    $$MAIN_PROJECT_DIR/SolutionParser/FileSolutionSource.h \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolution.h \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolutionGlobal.h \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolutionParser.h \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolutionProject.h \
    $$MAIN_PROJECT_DIR/SolutionParser/ISolutionSource.h \
    $$MAIN_PROJECT_DIR/SolutionParser/Solution.h \
    $$MAIN_PROJECT_DIR/SolutionParser/SolutionGlobal.h \
    $$MAIN_PROJECT_DIR/SolutionParser/SolutionParser.h \
    $$MAIN_PROJECT_DIR/SolutionParser/SolutionProject.h \
    $$MAIN_PROJECT_DIR/SolutionParser/SolutionProjectType.h \
    $$MAIN_PROJECT_DIR/Utils/Bom.h \
    $$MAIN_PROJECT_DIR/Utils/StringAlgorithms.h \
    TestSuite.h

LIBS += -lfmt

DISTFILES += \
    LICENSES.md

OTHER_FILES += TestSolutions/Project1.sln

RESOURCES +=
