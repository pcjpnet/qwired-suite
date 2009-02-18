SOURCES += main.cpp \
    QwsServerController.cpp \
    wiredsocket.cpp \
    QwSslTcpServer.cpp \
    classwireduser.cpp \
    QwsConnection.cpp \
    QwsRoom.cpp \
    classwiredfile.cpp
INCLUDEPATH = ../qw_library
LIBS += ../bin/libqwlibrary.a
TEMPLATE = app
QT -= gui

CONFIG += debug ppc x86
QT += network \
    xml \
    sql
TARGET = qwired_server

DESTDIR = ../bin/

HEADERS += QwsServerController.h \
    wiredsocket.h \
    QwSslTcpServer.h \
    classwireduser.h \
    QwsConnection.h \
    QwsRoom.h \
    classwiredfile.h
