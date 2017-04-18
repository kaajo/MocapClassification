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
    similaritymatrixcreator.cpp

HEADERS += \
    modelfactory.h \
    mocapanimation.h \
    similaritymatrixcreator.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
