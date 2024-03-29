TEMPLATE = app
HEADERS += QwmMonitorWindow.h \
    QwmMonitorController.h \
    QwmConsoleSocket.h \
    QwmTransfersListDelegate.h
SOURCES += main.cpp \
    QwmMonitorWindow.cpp \
    QwmMonitorController.cpp \
    QwmConsoleSocket.cpp \
    QwmTransfersListDelegate.cpp
FORMS += QwmMonitorWindow.ui
DESTDIR = ../bin
macx:TARGET = "Qwired Server GUI"
!macx:TARGET = qw_server_gui
RESOURCES += ../qw_client/application.qrc \
    qw_server_gui.qrc
QT += network
TRANSLATIONS = lang/lang_de.ts \
    lang/lang_nl.ts \
    lang/lang_jp.ts
LIBS += ../bin/libqwlibrary.a
INCLUDEPATH += ../qw_library
ICON = icons/icon-server.icns
RC_FILE = qw_server_gui.rc
