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
CONFIG -= app_bundle
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
