#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T07:12:59
#
#-------------------------------------------------

QT       += opengl concurrent

QT       -= gui

QMAKE_CXXFLAGS += -std=c++11 -pedantic -Wall -Wextra

TARGET = ModelBase
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    modelfactory.cpp \
    mocapanimation.cpp \
    mocapstats.cpp \
    similaritymatrixcreator.cpp

HEADERS += \
    modelfactory.h \
    mocapanimation.h \
    mocapstats.h \
    similaritymatrixcreator.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
