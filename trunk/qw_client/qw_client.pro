TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    debug
TARGET = qwired
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
    release:CONFIG += x86 \
        ppc
}
TRANSLATIONS += lang/lang_de.ts \
    lang/lang_pt.ts \
    lang/lang_it.ts \
    lang/lang_fr.ts \
    lang/lang_es.ts \
    lang/lang_jp.ts
SOURCES += general/main.cpp \
    wired/wiredsocket.cpp \
    general/classwiredsession.cpp \
    gui/widgetforum.cpp \
    gui/widgetnews.cpp \
    gui/modeluserlist.cpp \
    gui/delegateuserlist.cpp \
    gui/widgetsendprivmsg.cpp \
    gui/widgetuserinfo.cpp \
    gui/widgetitemuserlist.cpp \
    gui/widgetfilebrowser.cpp \
    gui/modelfilelist.cpp \
    wired/classwiredfile.cpp \
    gui/widgetprefs.cpp \
    wired/wiredtransfersocket.cpp \
    gui/widgettransfers.cpp \
    gui/modelfiletransfers.cpp \
    gui/delegatefiletransfers.cpp \
    gui/WidgetFileSearch.cpp \
    gui/widgetaccounts.cpp \
    gui/WidgetTracker.cpp \
    general/wiredsingleton.cpp \
    gui/widgetserverinfo.cpp \
    general/ClassWiredEventFilter.cpp
FORMS += gui/WinMain.ui \
    gui/WidgetForum.ui \
    gui/WidgetNews.ui \
    gui/WidgetSendPrivMsg.ui \
    gui/WidgetUserInfo.ui \
    gui/WidgetServerInfo.ui \
    gui/WidgetFileBrowser.ui \
    gui/WidgetPrefs.ui \
    gui/WidgetConnect.ui \
    gui/WidgetTransfers.ui \
    gui/WidgetFileSearch.ui \
    gui/WidgetAccounts.ui \
    gui/WidgetFileInfo.ui \
    gui/WidgetTracker.ui \
    gui/WidgetAboutQwired.ui \
    gui/WidgetAbout.ui \
    gui/WidgetNewsPost.ui
HEADERS += general/classwiredsession.h \
    wired/classwireduser.h \
    gui/connwindow.h \
    gui/widgetforum.h \
    wired/wiredsocket.h \
    gui/widgetnews.h \
    gui/modeluserlist.h \
    gui/delegateuserlist.h \
    gui/widgetsendprivmsg.h \
    gui/widgetuserinfo.h \
    gui/widgetitemuserlist.h \
    gui/widgetserverinfo.h \
    gui/widgetfilebrowser.h \
    gui/modelfilelist.h \
    wired/classwiredfile.h \
    gui/widgetprefs.h \
    gui/widgetconnect.h \
    wired/classwiredtransfer.h \
    wired/wiredtransfersocket.h \
    gui/widgettransfers.h \
    gui/modelfiletransfers.h \
    general/wiredcommon.h \
    gui/delegatefiletransfers.h \
    general/singleton.h \
    gui/WidgetFileSearch.h \
    general/wiredsingleton.h \
    gui/WidgetColorButton.h \
    gui/widgetaccounts.h \
    gui/WidgetFileInfo.h \
    gui/WidgetTracker.h \
    wired/classtrackerserver.h \
    gui/widgetabout.h \
    gui/WidgetNewsPost.h \
    general/ClassWiredEventFilter.h
