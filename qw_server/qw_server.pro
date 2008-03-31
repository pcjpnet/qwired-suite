SOURCES += main.cpp \
 qwservercontroller.cpp \
 wiredsocket.cpp \
 ssltcpserver.cpp \
 classwireduser.cpp

TEMPLATE = app

QT -= gui

QT += network \
xml
TARGET = qwired_server

DESTDIR = ../bin/

HEADERS += qwservercontroller.h \
 wiredsocket.h \
 ssltcpserver.h \
 classwireduser.h
