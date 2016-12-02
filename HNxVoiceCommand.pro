#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:31:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HNxVoiceCommand
TEMPLATE = app


SOURCES += main.cpp\
           dialog.cpp \
           dialog2.cpp \
    dialog.cpp

HEADERS  += dialog.hpp \
            Recog.hpp \
            ipcsm.hpp \
            dialog2.hpp

FORMS    += dialog.ui \
            dialog2.ui

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings
