#-------------------------------------------------
#
# Project created by QtCreator 2016-03-11T16:24:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = segm2
TEMPLATE = app

INCLUDEPATH += C:/OpenCV/build/install/include

LIBS += "C:/opencv/build/bin/libopencv_calib3d2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_contrib2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_core2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_features2d2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_flann2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_gpu2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_highgui2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_imgproc2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_legacy2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_ml2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_nonfree2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_objdetect2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_ocl2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_photo2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_stitching2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_superres2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_video2412.dll"
LIBS += "C:/opencv/build/bin/libopencv_videostab2412.dll"

#INCLUDEPATH += C:/opencv3/opencv/build/include
#LIBS += "C:/opencv3/opencv/build/x64/vc14/bin/opencv_world310.dll"


SOURCES += main.cpp\
        mainwindow.cpp \
    pixmapwidget.cpp \
    disjointsetforest.cpp \
    egbs.cpp \
    kmeans.cpp \
    gmm.cpp

HEADERS  += mainwindow.h \
    pixmapwidget.h \
    disjointsetforest.h \
    egbs.h \
    cluster.h \
    kmeans.h \
    gmm.h
