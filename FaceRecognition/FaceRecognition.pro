#-------------------------------------------------
#
# Project created by QtCreator 2019-03-18T12:05:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FaceRecognition
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    facerec.cpp \
    tools.cpp \
    imgthread.cpp

HEADERS += \
        mainwindow.h \
    facerec.h \
    imgthread.h

FORMS += \
        mainwindow.ui \
    facerec.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 加入OpenCV的头文件路径和库路径
INCLUDEPATH += D:\opencv-3.4.5\opencv-3.4.5-build\install\include

LIBS += D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_core345.dll \
    D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_highgui345.dll \
    D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_imgcodecs345.dll \
    D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_imgproc345.dll \
    D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_features2d345.dll \
    D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_calib3d345.dll \
    D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_shape345.dll \
    D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_videoio345.dll \
    D:\opencv-3.4.5\opencv-3.4.5-build\install\x86\mingw\bin\libopencv_objdetect345.dll

