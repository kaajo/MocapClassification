#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T07:12:59
#
#-------------------------------------------------

QT += opengl concurrent

QMAKE_CXXFLAGS += -std=c++1y -pedantic -Wall -Wextra -march=native

TARGET = ModelBase
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    modelfactory.cpp \
    mocapanimation.cpp \
    similaritymatrixcreator.cpp \
    bodynode.cpp \
    result.cpp \
    resultstats.cpp

HEADERS += \
    modelfactory.h \
    mocapanimation.h \
    similaritymatrixcreator.h \
    bodynode.hpp \
    result.h \
    resultstats.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
