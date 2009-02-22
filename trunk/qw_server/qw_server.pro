SOURCES += main.cpp \
    QwsServerController.cpp \
    QwsClientSocket.cpp \
    QwSslTcpServer.cpp \
    QwsUser.cpp \
    QwsRoom.cpp \
    QwsFile.cpp
INCLUDEPATH = ../qw_library
LIBS += ../bin/libqwlibrary.a
TEMPLATE = app
QT -= gui

CONFIG += debug \
    ppc \
    x86
QT += network \
    xml \
    sql
TARGET = qwired_server
DESTDIR = ../bin/
HEADERS += QwsServerController.h \
    QwsClientSocket.h \
    QwSslTcpServer.h \
    QwsUser.h \
    QwsRoom.h \
    QwsFile.h
