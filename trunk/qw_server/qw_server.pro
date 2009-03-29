SOURCES += main.cpp \
    QwsServerController.cpp \
    QwsClientSocket.cpp \
    QwSslTcpServer.cpp \
    QwsFile.cpp \
    QwsClientTransferSocket.cpp \
    QwsUser.cpp \
    QwsTransferInfo.cpp \
    QwsTransferPool.cpp
INCLUDEPATH = ../qw_library
LIBS += ../bin/libqwlibrary.a
TEMPLATE = app
QT -= gui
CONFIG += debug
CONFIG -= app_bundle
QT += network \
    xml \
    sql
TARGET = qwired_server
DESTDIR = ../bin/
HEADERS += QwsServerController.h \
    QwsClientSocket.h \
    QwSslTcpServer.h \
    QwsFile.h \
    QwsClientTransferSocket.h \
    QwsUser.h \
    QwsTransferInfo.h \
    QwsTransferPool.h
