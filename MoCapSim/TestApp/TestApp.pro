#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T22:04:48
#
#-------------------------------------------------

QT += core gui concurrent 3dextras widgets

QMAKE_CXXFLAGS += -std=c++17 -pedantic -Wall -Wextra -march=native

TARGET = TestApp
TEMPLATE = app

DEFINES += "cimg_display=0" #wow this is cool

SOURCES += main.cpp\
        mainwindow.cpp \
    methodtester.cpp \
    categorymapper.cpp \
    plugininfo.cpp \
    weigtedmean.cpp \
    filterandrefine.cpp

HEADERS  += mainwindow.h \
    functions.h \
    methodtester.h \
    helperfunctions.h \
    comparefunctions.h \
    categorymapper.hpp \
    plugininfo.h \
    weigtedmean.h \
    idistancefunction.h \
    filterandrefine.h

FORMS    += mainwindow.ui \
    plugininfo.ui \
    weigtedmean.ui \
    filterandrefine.ui

INCLUDEPATH += /usr/include/eigen3

# CUDA
#DESTDIR     = $$system(pwd)
#OBJECTS_DIR = release/obj

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
#    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o#include <QVector3D>
#include <QDebug>
#    cuda.commands = $$CUDA_DIR/bin/nvcc $$NVCC_OPTIONS $$CUDA_INC $$NVCC_LIBS --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
#    cuda.dependency_type = TYPE_C
#    QMAKE_EXTRA_COMPILERS += cuda
#}

#INCLUDEPATH += $$PWD/../../../../../../../usr/local/lib64
#DEPENDPATH += $$PWD/../../../../../../../usr/local/lib64

CONFIG += link_pkgconfig
PKGCONFIG += opencv tbb x11

unix:!macx: LIBS += -L$$OUT_PWD/../Visualization/ -lVisualization

INCLUDEPATH += $$PWD/../Visualization
DEPENDPATH += $$PWD/../Visualization

unix:!macx: LIBS += -L$$OUT_PWD/../ModelBase/ -lModelBase

INCLUDEPATH += $$PWD/../ModelBase
DEPENDPATH += $$PWD/../ModelBase

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../ModelBase/libModelBase.a
