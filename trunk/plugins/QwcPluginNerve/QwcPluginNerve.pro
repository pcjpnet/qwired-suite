TEMPLATE = lib
CONFIG += plugin
QT += core \
    gui
SOURCES += QwcPluginNerve.cpp
HEADERS += QwcPluginNerve.h
FORMS += QwcPluginNerveWidget.ui
TARGET = QwcPluginNerve
DESTDIR = ../../bin/plugins
INCLUDEPATH += ../../qw_client/
RESOURCES += resources.qrc
