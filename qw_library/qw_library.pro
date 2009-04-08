# -------------------------------------------------
# Project created by QtCreator 2009-02-15T09:51:58
# -------------------------------------------------
QT -= gui
QT += network
TARGET = qw_library
TEMPLATE = lib
CONFIG += staticlib debug
SOURCES += QwMessage.cpp \
    QwSocket.cpp \
    QwRoom.cpp \
    QwUser.cpp \
    QwTrackerClientSocket.cpp \
    QwFile.cpp

HEADERS += QwMessage.h \
    QwSocket.h \
    QwRoom.h \
    QwUser.h \
    QwTrackerClientSocket.h \
    QwServerInfo.h \
    QwTransferInfo.h \
    QwFile.h

TARGET = qwlibrary
DESTDIR = ../bin/