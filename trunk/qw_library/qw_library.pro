# -------------------------------------------------
# Project created by QtCreator 2009-02-15T09:51:58
# -------------------------------------------------
QT -= gui
QT += network
TARGET = qw_library
TEMPLATE = lib
CONFIG += staticlib debug
SOURCES += QwMessage.cpp \
    QwSocket.cpp
HEADERS += QwMessage.h \
    QwSocket.h
TARGET = qwlibrary
DESTDIR = ../bin/
