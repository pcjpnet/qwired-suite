TEMPLATE = app
HEADERS += ServerMonitorWindow.h \
    ServerMonitorController.h \
    QwmConsoleSocket.h
SOURCES += main.cpp \
    ServerMonitorWindow.cpp \
    ServerMonitorController.cpp \
    QwmConsoleSocket.cpp
FORMS += ServerMonitorWindow.ui
DESTDIR = ../bin
TARGET = qw_server_gui
RESOURCES += ../qw_client/application.qrc
QT += network
