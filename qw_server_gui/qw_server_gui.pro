
TEMPLATE = app

HEADERS += ServerMonitorWindow.h \
    ServerMonitorController.h

SOURCES += main.cpp \
    ServerMonitorWindow.cpp \
    ServerMonitorController.cpp

FORMS += ServerMonitorWindow.ui

DESTDIR = ../bin

TARGET = qw_server_gui
