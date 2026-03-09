#-------------------------------------------------
#
# Project created by QtCreator 2025-11-22T16:58:37
#
#-------------------------------------------------
CONFIG += c++11
QT       += core gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = project32
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    model/usermodel.cpp \
    model/singletondb.cpp \
    model/ffmpegvideoprocessor.cpp \
    controller/usercontroller.cpp \
    thread/camerathread.cpp \
    view/monitorwidget.cpp \
    view/playbackwidget.cpp \
    view/loginwidget.cpp \
    view/myitem.cpp \
    view/myview.cpp \
    view/playwidget.cpp \
    view/registerwidget.cpp \
    view/logwidget.cpp \
    view/syssettingwidget.cpp \
    view/playlistwidget.cpp \
    view/photolistwidget.cpp \
    view/videoplaywidget.cpp \
    view/mycalendarwidget.cpp

HEADERS  +=\
    model/usermodel.h \
    model/singletondb.h \
    model/sqlite3.h \
    model/ffmpegvideoprocessor.h \
    controller/usercontroller.h \
    thread/camerathread.h \
    view/monitorwidget.h \
    view/loginwidget.h \
    view/myitem.h \
    view/myview.h \
    view/playbackwidget.h \
    view/playwidget.h \
    view/registerwidget.h \
    view/logwidget.h \
    view/syssettingwidget.h \
    view/playlistwidget.h \
    view/photolistwidget.h \
    view/videoplaywidget.h \
    view/mycalendarwidget.h

LIBS +=$$PWD/lib/sqlite3.lib
RESOURCES += \
    resource.qrc
DESTDIR += bin
INCLUDEPATH += $$PWD/ffmpeglib/include
LIBS += $$PWD/ffmpeglib/lib/avcodec.lib  \
        $$PWD/ffmpeglib/lib/avdevice.lib  \
        $$PWD/ffmpeglib/lib/avfilter.lib  \
        $$PWD/ffmpeglib/lib/avformat.lib  \
        $$PWD/ffmpeglib/lib/avutil.lib  \
        $$PWD/ffmpeglib/lib/swresample.lib  \
        $$PWD/ffmpeglib/lib/swscale.lib

DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc
UI_DIR = $$PWD/build/ui


