TEMPLATE = app
CONFIG += warn_on thread qt release
TARGET = qwired
RESOURCES = misc/application.qrc
QT += gui network
DESTDIR = ../bin/

MOC_DIR = build
OBJECTS_DIR = build
UI_DIR = build
RCC_DIR = build

ICON = misc/qwired.icns
RC_FILE = misc/qwired.rc

macx {
 QMAKE_CXXFLAGS_RELEASE += -fvisibility=hidden
 QMAKE_CXXFLAGS_DEBUG += -fvisibility=hidden
 QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.3
 QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
}

macx:release {
 CONFIG += x86 ppc
}

 
TRANSLATIONS += lang/lang_de.ts \
 lang/lang_pt.ts \
 lang/lang_it.ts \
 lang/lang_fr.ts



SOURCES += general/main.cpp \
 wired/wiredsocket.cpp \
 general/classwiredsession.cpp \
 gui/widgetforum.cpp \
 gui/widgetnews.cpp \
 gui/dialognewspost.cpp \
 gui/modeluserlist.cpp \
 gui/delegateuserlist.cpp \
 gui/widgetsendprivmsg.cpp \
 gui/widgetuserinfo.cpp \
 gui/widgetitemuserlist.cpp \
 gui/widgetserverinfo.cpp \
 gui/widgetfilebrowser.cpp \
 gui/modelfilelist.cpp \
 wired/classwiredfile.cpp \
 gui/widgetprefs.cpp \
 wired/wiredtransfersocket.cpp \
 gui/widgettransfers.cpp \
 gui/modelfiletransfers.cpp \
 gui/delegatefiletransfers.cpp \
 gui/WidgetFileSearch.cpp \
 gui/WidgetColorButton.cpp \
 gui/widgetaccounts.cpp \
 gui/WidgetFileInfo.cpp \
 gui/WidgetTracker.cpp \
 general/wiredsingleton.cpp


FORMS += gui/WinMain.ui \
 gui/WidgetForum.ui \
 gui/WidgetNews.ui \
 gui/DialogNewsPost.ui \
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
 gui/WidgetAbout.ui

HEADERS += wired/classwireduser.h \
 gui/connwindow.h \
 gui/widgetforum.h \
 wired/wiredsocket.h \
 general/classwiredsession.h \
 gui/widgetnews.h \
 gui/dialognewspost.h \
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
 gui/widgetabout.h
