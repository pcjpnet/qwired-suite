INCLUDEPATH = ../qw_library
LIBS += ../bin/libqwlibrary.a
DESTDIR = ../bin/
TARGET = qwired_server
TEMPLATE = app
QT -= gui
CONFIG -= app_bundle
win32:CONFIG += console
QT += network \
    xml \
    sql

macx {
  # Copy the binary into the GUI bundle if it exists
  message("Hint: Copying server binary to GUI bundle!")
  DESTDIR = "$${DESTDIR}/Qwired Server GUI.app/Contents/MacOS/"
}


SOURCES += main.cpp \
    QwsServerController.cpp \
    QwsClientSocket.cpp \
    QwSslTcpServer.cpp \
    QwsFile.cpp \
    QwsClientTransferSocket.cpp \
    QwsUser.cpp \
    QwsTransferPool.cpp \
    QwsConsoleSocket.cpp \
    QwsFileIndexerThread.cpp
HEADERS += QwsServerController.h \
    QwsClientSocket.h \
    QwSslTcpServer.h \
    QwsFile.h \
    QwsClientTransferSocket.h \
    QwsUser.h \
    QwsTransferInfo.h \
    QwsTransferPool.h \
    QwsConsoleSocket.h \
    QwsFileIndexerThread.h
RESOURCES += qw_server.qrc
