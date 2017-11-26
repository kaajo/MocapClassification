QT += core gui concurrent

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/x86_64-linux-gnu/release/ -lX11
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/x86_64-linux-gnu/debug/ -lX11
else:unix: LIBS += -L$$PWD/../../../../../../../usr/lib/x86_64-linux-gnu/ -lX11

INCLUDEPATH += $$PWD/../../../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../../../usr/lib/x86_64-linux-gnu
