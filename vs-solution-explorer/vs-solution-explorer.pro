QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20 warn_on

QMAKE_CXXFLAGS += -Werror -Wnon-virtual-dtor -Wsuggest-override

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SolutionParser/FileSolutionSource.cpp \
    SolutionParser/ISolution.cpp \
    SolutionParser/ISolutionGlobal.cpp \
    SolutionParser/ISolutionParser.cpp \
    SolutionParser/ISolutionProject.cpp \
    SolutionParser/ISolutionSource.cpp \
    SolutionParser/Solution.cpp \
    SolutionParser/SolutionGlobal.cpp \
    SolutionParser/SolutionParser.cpp \
    SolutionParser/SolutionProject.cpp \
    SolutionParser/SolutionProjectType.cpp \
    Utils/Bom.cpp \
    Utils/StringAlgorithms.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    SolutionParser/FileSolutionSource.h \
    SolutionParser/ISolution.h \
    SolutionParser/ISolutionGlobal.h \
    SolutionParser/ISolutionParser.h \
    SolutionParser/ISolutionProject.h \
    SolutionParser/ISolutionSource.h \
    SolutionParser/Solution.h \
    SolutionParser/SolutionGlobal.h \
    SolutionParser/SolutionParser.h \
    SolutionParser/SolutionProject.h \
    SolutionParser/SolutionProjectType.h \
    Utils/Bom.h \
    Utils/StringAlgorithms.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

LIBS += -lfmt

TRANSLATIONS += \
    vs-solution-explorer_ru_RU.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

OTHER_FILES += SolutionParser/SLN-format.md
