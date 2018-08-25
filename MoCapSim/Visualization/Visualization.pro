#
#    Copyright (C) 2017  Miroslav Krajíček

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

#-------------------------------------------------
#
# Project created by QtCreator 2017-04-09T00:34:00
#
#-------------------------------------------------

QT += core gui widgets charts 3dcore 3drender 3dextras
TARGET = Visualization
TEMPLATE = lib

QMAKE_CXXFLAGS += -std=c++1y -pedantic -Wall -Wextra -march=native

DEFINES += VISUALIZATION_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    metricvisualization.cpp \
    animplayer.cpp \
    resultvisualization.cpp

HEADERS +=\
        visualization_global.h \
    metricvisualization.h \
    animplayer.h \
    resultvisualization.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ModelBase/release/ -lModelBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ModelBase/debug/ -lModelBase
else:unix: LIBS += -L$$OUT_PWD/../ModelBase/ -lModelBase

INCLUDEPATH += $$PWD/../ModelBase
DEPENDPATH += $$PWD/../ModelBase

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelBase/release/libModelBase.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelBase/debug/libModelBase.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelBase/release/ModelBase.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelBase/debug/ModelBase.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../ModelBase/libModelBase.a

FORMS += \
    metricvisualization.ui \
    animplayer.ui \
    resultvisualization.ui
