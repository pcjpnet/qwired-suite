SOURCES += main.cpp \
 general/qwiredservercontroller.cpp \
 qwiredconnection.cpp \
 wiredsocket.cpp

TEMPLATE = app

QT -= gui

QT += network \
xml
TARGET = qwired_server

DESTDIR = ../bin/

HEADERS += general/qwiredservercontroller.h \
qwiredconnection.h \
wiredsocket.h
