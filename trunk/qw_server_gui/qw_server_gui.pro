TEMPLATE = app
HEADERS += QwmMonitorWindow.h \
    QwmMonitorController.h \
    QwmConsoleSocket.h

SOURCES += main.cpp \
    QwmMonitorWindow.cpp \
    QwmMonitorController.cpp \
    QwmConsoleSocket.cpp



FORMS += QwmMonitorWindow.ui
DESTDIR = ../bin
TARGET = qw_server_gui
RESOURCES += ../qw_client/application.qrc
QT += network

TRANSLATIONS = lang/lang_de.ts \
               lang/lang_nl.ts
