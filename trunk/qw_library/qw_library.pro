QT -= gui
QT += network
TARGET = qw_library
TEMPLATE = lib
CONFIG += silent

    CONFIG += staticlib


SOURCES += QwMessage.cpp \
    QwSocket.cpp \
    QwRoom.cpp \
    QwUser.cpp \
    QwTrackerClientSocket.cpp \
    QwFile.cpp \
    QwSocksSocket.cpp \
    QwTransferInfo.cpp
HEADERS += QwMessage.h \
    QwSocket.h \
    QwRoom.h \
    QwUser.h \
    QwTrackerClientSocket.h \
    QwServerInfo.h \
    QwTransferInfo.h \
    QwFile.h \
    QwSocksSocket.h \
    QwGlobals.h
TARGET = qwlibrary
DESTDIR = ../bin/
