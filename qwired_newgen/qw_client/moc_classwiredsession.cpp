/****************************************************************************
** Meta object code from reading C++ file 'classwiredsession.h'
**
** Created: Sun Jun 29 19:11:11 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "general/classwiredsession.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'classwiredsession.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ClassWiredSession[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      45,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   19,   18,   18, 0x08,
      81,   45,   18,   18, 0x08,
     135,  131,   18,   18, 0x08,
     171,   18,   18,   18, 0x08,
     185,   18,   18,   18, 0x08,
     220,   18,   18,   18, 0x08,
     235,   18,   18,   18, 0x08,
     265,  257,   18,   18, 0x08,
     314,  298,   18,   18, 0x08,
     345,  298,   18,   18, 0x08,
     388,  374,   18,   18, 0x08,
     455,  431,   18,   18, 0x08,
     491,   18,   18,   18, 0x08,
     527,   18,   18,   18, 0x08,
     560,   18,   18,   18, 0x08,
     610,   18,   18,   18, 0x0a,
     642,  625,   18,   18, 0x0a,
     676,   18,   18,   18, 0x0a,
     706,  702,   18,   18, 0x0a,
     751,   18,   18,   18, 0x0a,
     774,   18,   18,   18, 0x0a,
     795,   18,   18,   18, 0x0a,
     813,   18,   18,   18, 0x0a,
     843,  835,   18,   18, 0x0a,
     871,   18,   18,   18, 0x2a,
     892,   18,   18,   18, 0x0a,
     908,   18,   18,   18, 0x0a,
     924,   18,   18,   18, 0x0a,
     937,   18,   18,   18, 0x0a,
     952,   18,   18,   18, 0x0a,
     982,   18,   18,   18, 0x0a,
    1030, 1010,   18,   18, 0x0a,
    1097, 1068,   18,   18, 0x0a,
    1188, 1134,   18,   18, 0x0a,
    1277, 1258,   18,   18, 0x0a,
    1317, 1258,   18,   18, 0x0a,
    1367, 1359,   18,   18, 0x0a,
    1418, 1400,   18,   18, 0x0a,
    1478, 1468,   18,   18, 0x0a,
    1501, 1499,   18,   18, 0x0a,
    1528,   18,   18,   18, 0x0a,
    1549,   18,   18,   18, 0x0a,
    1575,   18,   18,   18, 0x0a,
    1607, 1597,   18,   18, 0x0a,
    1639, 1630,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClassWiredSession[] = {
    "ClassWiredSession\0\0text\0displayMotd(QString)\0"
    "cid,topic,users,chat_protected,type\0"
    "handleConferenceChanged(int,QString,int,bool,int)\0"
    "obj\0connectionWindowDestroyed(QObject*)\0"
    "reloadPrefs()\0onSocketPrivileges(ClassWiredUser)\0"
    "showTrackers()\0createNewConnection()\0"
    "theFile\0onServerFileInfo(ClassWiredFile)\0"
    "theChat,theUser\0userJoined(int,ClassWiredUser)\0"
    "userLeft(int,ClassWiredUser)\0theOld,theNew\0"
    "userChanged(ClassWiredUser,ClassWiredUser)\0"
    "theNick,theTime,thePost\0"
    "newsPosted(QString,QString,QString)\0"
    "transferStarted(ClassWiredTransfer)\0"
    "transferDone(ClassWiredTransfer)\0"
    "transferSocketError(QAbstractSocket::SocketError)\0"
    "showMessages()\0event,parameters\0"
    "triggerEvent(QString,QStringList)\0"
    "setTrayMenuAction(QMenu*)\0,,,\0"
    "do_handle_chat_message(int,int,QString,bool)\0"
    "disconnectFromServer()\0do_show_serverinfo()\0"
    "showNewsBrowser()\0do_new_broadcastmsg()\0"
    "thePath\0do_new_filebrowser(QString)\0"
    "do_new_filebrowser()\0do_show_prefs()\0"
    "showTransfers()\0showSearch()\0"
    "showAccounts()\0search_download_file(QString)\0"
    "search_reveal_file(QString)\0"
    "theText,theIsAction\0"
    "doHandlePublicChatInput(QString,bool)\0"
    "theHost,theLogin,thePassword\0"
    "onDoConnect(QString,QString,QString)\0"
    "theChatID,theNick,theLogin,theIP,theDateTime,theTopic\0"
    "doHandleChatTopic(int,QString,QString,QHostAddress,QDateTime,QString)\0"
    "theUser,theMessage\0"
    "doHandlePrivMsg(ClassWiredUser,QString)\0"
    "doHandleBroadcast(ClassWiredUser,QString)\0"
    "theUser\0doHandleUserInfo(ClassWiredUser)\0"
    "theChatID,theUser\0"
    "doHandlePrivateChatInvitation(int,ClassWiredUser)\0"
    "theChatID\0doCreateNewChat(int)\0,\0"
    "onSocketError(QString,int)\0"
    "onSocketServerInfo()\0onSocketLoginSuccessful()\0"
    "onSocketLoginFailed()\0theBanner\0"
    "setBannerView(QPixmap)\0theError\0"
    "handleErrorOccoured(int)\0"
};

const QMetaObject ClassWiredSession::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ClassWiredSession,
      qt_meta_data_ClassWiredSession, 0 }
};

const QMetaObject *ClassWiredSession::metaObject() const
{
    return &staticMetaObject;
}

void *ClassWiredSession::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClassWiredSession))
	return static_cast<void*>(const_cast< ClassWiredSession*>(this));
    return QObject::qt_metacast(_clname);
}

int ClassWiredSession::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: displayMotd((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: handleConferenceChanged((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const bool(*)>(_a[4])),(*reinterpret_cast< const int(*)>(_a[5]))); break;
        case 2: connectionWindowDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 3: reloadPrefs(); break;
        case 4: onSocketPrivileges((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 5: showTrackers(); break;
        case 6: createNewConnection(); break;
        case 7: onServerFileInfo((*reinterpret_cast< ClassWiredFile(*)>(_a[1]))); break;
        case 8: userJoined((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< ClassWiredUser(*)>(_a[2]))); break;
        case 9: userLeft((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< ClassWiredUser(*)>(_a[2]))); break;
        case 10: userChanged((*reinterpret_cast< ClassWiredUser(*)>(_a[1])),(*reinterpret_cast< ClassWiredUser(*)>(_a[2]))); break;
        case 11: newsPosted((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 12: transferStarted((*reinterpret_cast< ClassWiredTransfer(*)>(_a[1]))); break;
        case 13: transferDone((*reinterpret_cast< ClassWiredTransfer(*)>(_a[1]))); break;
        case 14: transferSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 15: showMessages(); break;
        case 16: triggerEvent((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2]))); break;
        case 17: setTrayMenuAction((*reinterpret_cast< QMenu*(*)>(_a[1]))); break;
        case 18: do_handle_chat_message((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 19: disconnectFromServer(); break;
        case 20: do_show_serverinfo(); break;
        case 21: showNewsBrowser(); break;
        case 22: do_new_broadcastmsg(); break;
        case 23: do_new_filebrowser((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 24: do_new_filebrowser(); break;
        case 25: do_show_prefs(); break;
        case 26: showTransfers(); break;
        case 27: showSearch(); break;
        case 28: showAccounts(); break;
        case 29: search_download_file((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 30: search_reveal_file((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 31: doHandlePublicChatInput((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 32: onDoConnect((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 33: doHandleChatTopic((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QHostAddress(*)>(_a[4])),(*reinterpret_cast< QDateTime(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6]))); break;
        case 34: doHandlePrivMsg((*reinterpret_cast< ClassWiredUser(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 35: doHandleBroadcast((*reinterpret_cast< ClassWiredUser(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 36: doHandleUserInfo((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 37: doHandlePrivateChatInvitation((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< ClassWiredUser(*)>(_a[2]))); break;
        case 38: doCreateNewChat((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 39: onSocketError((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 40: onSocketServerInfo(); break;
        case 41: onSocketLoginSuccessful(); break;
        case 42: onSocketLoginFailed(); break;
        case 43: setBannerView((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        case 44: handleErrorOccoured((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 45;
    }
    return _id;
}
