SOURCES += main.cpp \
 qwservercontroller.cpp \
 wiredsocket.cpp \
 ssltcpserver.cpp \
 classwireduser.cpp \
 qwservercore.cpp \
 classwiredfile.cpp \
 ../common/qwtransaction.cpp \
 qwguiserver.cpp \
 qwclassprivatechat.cpp

TEMPLATE = app

QT -= gui

CC = colorgcc

CONFIG -= app_bundle

QT += network \
xml \
 sql
TARGET = qwired_server


HEADERS += qwservercontroller.h \
 wiredsocket.h \
 ssltcpserver.h \
 classwireduser.h \
 qwservercore.h \
 classwiredfile.h \
 ../common/qwtransaction.h \
 qwguiserver.h \
 qwclassprivatechat.h
INCLUDEPATH += ../common/

DESTDIR = ../bin/

