#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T22:04:48
#
#-------------------------------------------------

QT       += core gui concurrent 3dextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++1y -pedantic -Wall -Wextra -march=native

TARGET = TestApp
TEMPLATE = app


# CUDA
DESTDIR     = $$system(pwd)
OBJECTS_DIR = release/obj

SOURCES += main.cpp\
        mainwindow.cpp \
    methodtester.cpp \
    result.cpp \
    resultstats.cpp

HEADERS  += mainwindow.h \
    functions.h \
    methodtester.h \
    result.h \
    resultstats.hpp

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/include/eigen3

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Visualization/release/ -lVisualization
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Visualization/debug/ -lVisualization
else:unix: LIBS += -L$$OUT_PWD/../Visualization/ -lVisualization

INCLUDEPATH += $$PWD/../Visualization
DEPENDPATH += $$PWD/../Visualization

#CUDA_SOURCES += cuda_code.cu general_metrics.cu
#CUDA_DIR = /usr/local/cuda-7.5

#SYSTEM_NAME = unix         # Depending on your system either 'Win32', 'x64', or 'Win64'
#SYSTEM_TYPE = 64
#CUDA_ARCH = sm_20          # (tested with sm_30 on my comp) Type of CUDA architecture, for example 'compute_10', 'compute_11', 'sm_10'
#NVCC_OPTIONS = --use_fast_math -std=c++11

## include paths
#INCLUDEPATH += $$CUDA_DIR/include $$CUDA_DIR/samples/common/inc

## library directories
#QMAKE_LIBDIR += $$CUDA_DIR/lib64

#CUDA_OBJECTS_DIR = ./


## Add the necessary libraries
#CUDA_LIBS = -lcuda -lcudart -L/usr/local/cuda-7.5/lib64

## The following makes sure all path names (which often include spaces) are put between quotation marks
#CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')
#LIBS += $$CUDA_LIBS

## Configuration of the Cuda compiler
#CONFIG(debug, debug|release) {
#    # Debug mode
#    cuda_d.input = CUDA_SOURCES
#    cuda_d.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
#    cuda_d.commands = $$CUDA_DIR/bin/nvcc -D_DEBUG $$NVCC_OPTIONS $$CUDA_INC $$NVCC_LIBS --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
#    cuda_d.dependency_type = TYPE_C
#    QMAKE_EXTRA_COMPILERS += cuda_d
#}
#else {
#    # Release mode
#    cuda.input = CUDA_SOURCES
#    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
#    cuda.commands = $$CUDA_DIR/bin/nvcc $$NVCC_OPTIONS $$CUDA_INC $$NVCC_LIBS --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
#    cuda.dependency_type = TYPE_C
#    QMAKE_EXTRA_COMPILERS += cuda
#}

INCLUDEPATH += $$PWD/../../../../../../../usr/local/lib64
DEPENDPATH += $$PWD/../../../../../../../usr/local/lib64

CONFIG += link_pkgconfig
PKGCONFIG += opencv tbb
