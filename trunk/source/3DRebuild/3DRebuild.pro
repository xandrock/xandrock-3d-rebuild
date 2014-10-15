#-------------------------------------------------
#
# Project created by QtCreator 2014-10-13T14:58:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = 3DRebuild
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#this file specific the library path, may not be same path in different host so don't add it to repo
# sample file :
#INCLUDEPATH += ../../../../SDK/opencv/build/include
#contains(QMAKE_HOST.arch, x86_64){
#	win32-msvc2010{
#		OPENCV_LIB += ../../../../SDK/opencv/build/x64/vc10/lib/
#	}
#}else{
#	win32-msvc2013{
#		OPENCV_LIB += ../../../../SDK/opencv/build/x86/vc12/lib/
#	}
#}
include(Dependence.pri)

INCLUDEPATH += ../common/3dmath

SOURCES += main.cpp \
    ../common/3dmath/math3d.cpp \
    ../common/3dmath/MathUtil.cpp \
    ../common/3dmath/Quaternion.cpp \
    ../common/3dmath/Matrix.cpp

HEADERS += \
    ../common/3dmath/math3d.h \
    ../common/3dmath/MathUtil.h \
    ../common/3dmath/Quaternion.h \
    ../common/3dmath/Vector.h \
    ../common/3dmath/Matrix.h

contains(QMAKE_HOST.arch, x86_64){
    release{
	DESTDIR = ../../release/x64/$$TARGET
    }else{
	DESTDIR = ../../debug/x64/$$TARGET
    }
}else{
    release{
	DESTDIR = ../../release/win32/$$TARGET
    }else{
	DESTDIR = ../../debug/win32/$$TARGET
    }
}

LIBS += -l$${OPENCV_LIB}opencv_core249d \
	-l$${OPENCV_LIB}opencv_core249d \
	-l$${OPENCV_LIB}opencv_imgproc249d \
	-l$${OPENCV_LIB}opencv_calib3d249d \
	-l$${OPENCV_LIB}opencv_photo249d \
	-l$${OPENCV_LIB}opencv_contrib249d \
	-l$${OPENCV_LIB}opencv_features2d249d \
	-l$${OPENCV_LIB}opencv_flann249d \
	-l$${OPENCV_LIB}opencv_gpu249d \
	-l$${OPENCV_LIB}opencv_highgui249d \
	-l$${OPENCV_LIB}opencv_legacy249d \
	-l$${OPENCV_LIB}opencv_ml249d \
	-l$${OPENCV_LIB}opencv_objdetect249d \
	-l$${OPENCV_LIB}opencv_ts249d \
	-l$${OPENCV_LIB}opencv_video249d

MOC_DIR = ../../build/$$TARGET
OBJECTS_DIR = ../../build/$$TARGET
UI_DIR = ../../build/$$TARGET
RCC_DIR = ../../build/$$TARGET
