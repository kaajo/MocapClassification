#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T22:04:48
#
#-------------------------------------------------

QT += core gui concurrent 3dextras widgets charts

QMAKE_CXXFLAGS += -std=c++17 -pedantic -Wall -Wextra -march=native -mtune=native

TARGET = TestApp
TEMPLATE = app

DEFINES += "cimg_display=0" #wow this is cool

SOURCES += main.cpp\
        mainwindow.cpp \
    methodtester.cpp \
    categorymapper.cpp \
    plugininfo.cpp \
    weigtedmean.cpp \
    filterandrefine.cpp \
    animplayer.cpp \
    resultvisualization.cpp

HEADERS  += mainwindow.h \
    functions.h \
    methodtester.h \
    helperfunctions.h \
    comparefunctions.h \
    categorymapper.hpp \
    plugininfo.h \
    weigtedmean.h \
    idistancefunction.h \
    filterandrefine.h \
    animplayer.h \
    resultvisualization.hpp

FORMS    += mainwindow.ui \
    plugininfo.ui \
    weigtedmean.ui \
    filterandrefine.ui \
    animplayer.ui \
    resultvisualization.ui

CONFIG += link_pkgconfig
PKGCONFIG += opencv

unix:!macx: LIBS += -L$$OUT_PWD/../ModelBase/ -lModelBase

INCLUDEPATH += $$PWD/../ModelBase
DEPENDPATH += $$PWD/../ModelBase

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../ModelBase/libModelBase.a
