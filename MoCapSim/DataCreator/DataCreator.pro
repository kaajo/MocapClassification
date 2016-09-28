QT += core gui

CONFIG += c++11

TARGET = DataCreator
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
    functions.h \

#internal library
unix:!macx: LIBS += -L$$OUT_PWD/../ModelBase/ -lModelBase

INCLUDEPATH += $$PWD/../ModelBase
DEPENDPATH += $$PWD/../ModelBase

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../ModelBase/libModelBase.a


CONFIG += link_pkgconfig
PKGCONFIG += opencv tbb

RESOURCES += \
    resources.qrc

HEADERS += \
    functions.h
