#-------------------------------------------------
#
# Project created by QtCreator 2019-08-27T15:24:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScreenDemo
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

#OPENCV
unix{
message($$TARGET "unix")
INCLUDEPATH += /usr/local/opencv4.0.0/include
LIBS += -L/lib -I/usr/local/opencv4.0.0 \
    -lopencv_highgui \
    -lopencv_imgcodecs \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_videoio
}
win32{
message($$TARGET "windows")
win32:CONFIG(release, debug|release): LIBS += -L'D:/OpenCV4.0.0/opencv/build/x64/vc15/lib/' -lopencv_world400
else:win32:CONFIG(debug, debug|release): LIBS += -L'D:/OpenCV4.0.0/opencv/build/x64/vc15/lib/' -lopencv_world400d
INCLUDEPATH += 'D:/OpenCV4.0.0/opencv/build/include'
}


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
