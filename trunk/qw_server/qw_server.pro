SOURCES += main.cpp \
 qwservercontroller.cpp \
 wiredsocket.cpp \
 ssltcpserver.cpp \
 classwireduser.cpp \
 qwservercore.cpp \
 qwclassprivatechat.cpp \
 classwiredfile.cpp

TEMPLATE = app

QT -= gui

CONFIG-=app_bundle

QT += network \
xml \
 sql
TARGET = qwired_server

DESTDIR = ../bin/

HEADERS += qwservercontroller.h \
 wiredsocket.h \
 ssltcpserver.h \
 classwireduser.h \
 qwservercore.h \
 qwclassprivatechat.h \
 classwiredfile.h
