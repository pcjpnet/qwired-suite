TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    debug
macx:TARGET = "Qwired Client"
!macx:TARGET = qwired
RESOURCES = application.qrc
QT += gui \
    network
DESTDIR = ../bin/
ICON = qwired.icns
RC_FILE = qwired.rc
INCLUDEPATH = .
macx { 
    QMAKE_CXXFLAGS_RELEASE += -fvisibility=hidden
    QMAKE_CXXFLAGS_DEBUG += -fvisibility=hidden
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.3
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
}

# release:CONFIG += x86 \
# ppc
TRANSLATIONS += lang/lang_de.ts \
    lang/lang_pt.ts \
    lang/lang_it.ts \
    lang/lang_fr.ts \
    lang/lang_es.ts \
    lang/lang_jp.ts
SOURCES += main.cpp \
    QwcSocket.cpp \
    QwcSession.cpp \
    QwcChatWidget.cpp \
    QwcNewsWidget.cpp \
    QwcUserlistModel.cpp \
    QwcUserlistDelegate.cpp \
    SendPrivateMessageWidget.cpp \
    QwcUserInfoWidget.cpp \
    QwcUserlistWidget.cpp \
    QwcFileBrowserWidget.cpp \
    QwcFilelistModel.cpp \
    QwcFileInfo.cpp \
    QwcPreferencesWidget.cpp \
    QwcFiletransferSocket.cpp \
    QwcFiletransferWidget.cpp \
    QwcFiletransferModel.cpp \
    QwcFiletransferDelegate.cpp \
    QwcFileSearchWidget.cpp \
    QwcAccountsWidget.cpp \
    QwcTrackerlistWidget.cpp \
    QwcSingleton.cpp \
    QwcServerInfoWidget.cpp \
    QwcEventFilter.cpp \
    QwcAboutWidget.cpp \
    QwcColorButton.cpp \
    QwcConnectWidget.cpp \
    QwcConnectionMainWindow.cpp
FORMS += QwcConnectionMainWindow.ui \
    QwcChatWidget.ui \
    QwcNewsWidget.ui \
    SendPrivateMessageWidget.ui \
    QwcUserInfoWidget.ui \
    QwcServerInfoWidget.ui \
    QwcFileBrowserWidget.ui \
    QwcPreferencesWidget.ui \
    QwcConnectWidget.ui \
    QwcFiletransferWidget.ui \
    QwcFileSearchWidget.ui \
    QwcAccountsWidget.ui \
    QwcFileInfoWidget.ui \
    QwcTrackerlistWidget.ui \
    QwcAboutWidget.ui \
    QwcNewsPostWidget.ui
HEADERS += QwcSession.h \
    QwcUserInfo.h \
    QwcConnectionMainWindow.h \
    QwcChatWidget.h \
    QwcSocket.h \
    QwcNewsWidget.h \
    QwcUserlistModel.h \
    QwcUserlistDelegate.h \
    SendPrivateMessageWidget.h \
    QwcUserInfoWidget.h \
    QwcUserlistWidget.h \
    QwcServerInfoWidget.h \
    QwcFileBrowserWidget.h \
    QwcFilelistModel.h \
    QwcFileInfo.h \
    QwcPreferencesWidget.h \
    QwcConnectWidget.h \
    QwcFiletransferInfo.h \
    QwcFiletransferSocket.h \
    QwcFiletransferWidget.h \
    QwcFiletransferModel.h \
    QwcGlobals.h \
    QwcFiletransferDelegate.h \
    QwcSingleton.h \
    QwcFileSearchWidget.h \
    singleton.h \
    QwcColorButton.h \
    QwcAccountsWidget.h \
    QwcFileInfoWidget.h \
    QwcTrackerlistWidget.h \
    QwcTrackerInfo.h \
    QwcAboutWidget.h \
    QwcNewsPostWidget.h \
    QwcEventFilter.h
