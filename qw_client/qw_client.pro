TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
CONFIG -= warnings

macx { 
    TARGET = "Qwired Client"
}

!macx {
    TARGET = qwired
}

RESOURCES = application.qrc
QT += gui \
    network \
    webkit
DESTDIR = ../bin/
ICON = qwired.icns
RC_FILE = qwired.rc
LIBS += ../bin/libqwlibrary.a
INCLUDEPATH += ../qw_library

TRANSLATIONS += lang/lang_de.ts \
    lang/lang_pt.ts \
    lang/lang_it.ts \
    lang/lang_fr.ts \
    lang/lang_es.ts \
    lang/lang_jp.ts \
    lang/lang_nl.ts \
    lang/lang_se.ts
SOURCES += main.cpp \
    QwcSocket.cpp \
    QwcSession.cpp \
    QwcChatWidget.cpp \
    QwcNewsWidget.cpp \
    QwcUserlistModel.cpp \
    QwcUserlistDelegate.cpp \
    QwcUserInfoWidget.cpp \
    QwcUserlistWidget.cpp \
    QwcFileBrowserWidget.cpp \
    QwcFileInfo.cpp \
    QwcPreferencesWidget.cpp \
    QwcTransferSocket.cpp \
    QwcFiletransferWidget.cpp \
    QwcFiletransferModel.cpp \
    QwcFiletransferDelegate.cpp \
    QwcAccountsWidget.cpp \
    QwcSingleton.cpp \
    QwcServerInfoWidget.cpp \
    QwcAboutWidget.cpp \
    QwcColorButton.cpp \
    QwcConnectWidget.cpp \
    QwcConnectionMainWindow.cpp \
    QwcPrivateMessager.cpp \
    QwcTransferInfo.cpp \
    ../qw_server/QwsTrackerController.cpp
FORMS += QwcConnectionMainWindow.ui \
    QwcChatWidget.ui \
    QwcNewsWidget.ui \
    QwcUserInfoWidget.ui \
    QwcServerInfoWidget.ui \
    QwcFileBrowserWidget.ui \
    QwcPreferencesWidget.ui \
    QwcConnectWidget.ui \
    QwcFiletransferWidget.ui \
    QwcAccountsWidget.ui \
    QwcAboutWidget.ui \
    QwcPrivateMessager.ui
HEADERS += QwcSession.h \
    QwcUserInfo.h \
    QwcConnectionMainWindow.h \
    QwcChatWidget.h \
    QwcSocket.h \
    QwcNewsWidget.h \
    QwcUserlistModel.h \
    QwcUserlistDelegate.h \
    QwcUserInfoWidget.h \
    QwcUserlistWidget.h \
    QwcServerInfoWidget.h \
    QwcFileBrowserWidget.h \
    QwcFileInfo.h \
    QwcPreferencesWidget.h \
    QwcConnectWidget.h \
    QwcTransferInfo.h \
    QwcTransferSocket.h \
    QwcFiletransferWidget.h \
    QwcFiletransferModel.h \
    QwcGlobals.h \
    QwcFiletransferDelegate.h \
    QwcSingleton.h \
    singleton.h \
    QwcColorButton.h \
    QwcAccountsWidget.h \
    QwcTrackerServerInfo.h \
    QwcAboutWidget.h \
    QwcPrivateMessager.h \
    ../qw_server/QwsTrackerController.h

