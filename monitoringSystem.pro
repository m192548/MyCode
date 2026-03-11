#-------------------------------------------------
#
# Project created by QtCreator 2025-11-22T16:58:37
#
#-------------------------------------------------
CONFIG += c++11
QT       += core gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = monitoringSystem
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


SOURCES += src/main.cpp\
    src/model/usermodel.cpp \
    src/model/singletondb.cpp \
    src/model/ffmpegvideoprocessor.cpp \
    src/controller/usercontroller.cpp \
    src/thread/camerathread.cpp \
    src/view/monitorwidget.cpp \
    src/view/playbackwidget.cpp \
    src/view/loginwidget.cpp \
   src/view/myitem.cpp \
    src/view/myview.cpp \
    src/view/playwidget.cpp \
    src/view/registerwidget.cpp \
    src/view/logwidget.cpp \
   src/view/syssettingwidget.cpp \
    src/view/playlistwidget.cpp \
    src/view/photolistwidget.cpp \
    src/view/videoplaywidget.cpp \
    src/view/mycalendarwidget.cpp

HEADERS  +=\
    src/model/usermodel.h \
    src/model/singletondb.h \
    src/model/sqlite3.h \
    src/model/ffmpegvideoprocessor.h \
    src/controller/usercontroller.h \
    src/thread/camerathread.h \
    src/view/monitorwidget.h \
    src/view/loginwidget.h \
    src/view/myitem.h \
    src/view/myview.h \
    src/view/playbackwidget.h \
    src/view/playwidget.h \
    src/view/registerwidget.h \
    src/view/logwidget.h \
    src/view/syssettingwidget.h \
    src/view/playlistwidget.h \
    src/view/photolistwidget.h \
    src/view/videoplaywidget.h \
    src/view/mycalendarwidget.h

LIBS +=$$PWD/lib/sqlite3/sqlite3.lib
RESOURCES += \
    resource/resource.qrc
DESTDIR += bin
INCLUDEPATH += $$PWD/ffmpeglib/include
INCLUDEPATH += $$PWD/src

LIBS += $$PWD/ffmpeglib/lib/avcodec.lib  \
        $$PWD/ffmpeglib/lib/avdevice.lib  \
        $$PWD/ffmpeglib/lib/avfilter.lib  \
        $$PWD/ffmpeglib/lib/avformat.lib  \
        $$PWD/ffmpeglib/lib/avutil.lib  \
        $$PWD/ffmpeglib/lib/swresample.lib  \
        $$PWD/ffmpeglib/lib/swscale.lib



