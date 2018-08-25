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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/x86_64-linux-gnu/release/ -lX11
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/x86_64-linux-gnu/debug/ -lX11
else:unix: LIBS += -L$$PWD/../../../../../../../usr/lib/x86_64-linux-gnu/ -lX11

INCLUDEPATH += $$PWD/../../../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../../../usr/lib/x86_64-linux-gnu
