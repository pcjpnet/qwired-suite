/****************************************************************************
** Meta object code from reading C++ file 'qwservercore.h'
**
** Created: Sat Jun 28 12:53:39 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qwservercore.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwservercore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QWServerCore[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      40,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      19,   14,   13,   13, 0x08,
      56,   14,   13,   13, 0x08,
      90,   14,   13,   13, 0x08,
     131,   14,   13,   13, 0x08,
     161,   14,   13,   13, 0x08,
     197,   14,   13,   13, 0x08,
     229,   14,   13,   13, 0x08,
     261,   14,   13,   13, 0x08,
     302,  289,   13,   13, 0x08,
     352,  349,   13,   13, 0x08,
     367,  349,   13,   13, 0x08,
     398,  390,   13,   13, 0x08,
     438,  429,   13,   13, 0x08,
     470,  390,   13,   13, 0x08,
     499,  429,   13,   13, 0x08,
     537,  529,   13,   13, 0x08,
     561,  529,   13,   13, 0x08,
     596,  586,   13,   13, 0x08,
     632,  624,   13,   13, 0x08,
     664,   14,   13,   13, 0x08,
     697,   14,   13,   13, 0x08,
     735,   13,   13,   13, 0x08,
     777,   14,   13,   13, 0x08,
     835,  818,   13,   13, 0x08,
     865,  586,   13,   13, 0x08,
     914,  890,   13,   13, 0x08,
     953,  945,   13,   13, 0x08,
     975,  349,   13,   13, 0x08,
     989,  349,   13,   13, 0x08,
    1021, 1007,   13,   13, 0x08,
    1060, 1052,   13,   13, 0x08,
    1087,  349,   13,   13, 0x08,
    1109,  349,   13,   13, 0x08,
    1129,  529,   13,   13, 0x08,
    1163, 1155,   13,   13, 0x08,
    1189, 1155,   13,   13, 0x08,
    1215,  529,   13,   13, 0x08,
    1252, 1242,   13,   13, 0x08,
    1274,  349,   13,   13, 0x08,
    1301, 1294,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QWServerCore[] = {
    "QWServerCore\0\0id,t\0"
    "handleTransaction(int,QWTransaction)\0"
    "sendNewsGroups(int,QWTransaction)\0"
    "sendNewsGroupArticles(int,QWTransaction)\0"
    "checkLogin(int,QWTransaction)\0"
    "sendServerBanner(int,QWTransaction)\0"
    "sendUserlist(int,QWTransaction)\0"
    "sendUserInfo(int,QWTransaction)\0"
    "sendMotd(int,QWTransaction)\0,changeFlags\0"
    "broadcastUserStatusChanged(ClassWiredUser,int)\0"
    "id\0clearNews(int)\0createPrivateChat(int)\0"
    "id,user\0createUser(int,ClassWiredUser)\0"
    "id,group\0createGroup(int,ClassWiredUser)\0"
    "editUser(int,ClassWiredUser)\0"
    "editGroup(int,ClassWiredUser)\0id,name\0"
    "deleteUser(int,QString)\0"
    "deleteGroup(int,QString)\0id,chatId\0"
    "declinePrivateChat(int,int)\0id,text\0"
    "broadcastBroadcast(int,QString)\0"
    "broadcastChat(int,QWTransaction)\0"
    "broadcastChatTopic(int,QWTransaction)\0"
    "broadcastUserImageChanged(ClassWiredUser)\0"
    "deliverPrivateMessage(int,QWTransaction)\0"
    "id,userId,chatId\0inviteUserToChat(int,int,int)\0"
    "joinPrivateChat(int,int)\0"
    "id,userId,reason,banned\0"
    "kickUser(int,int,QString,bool)\0id,news\0"
    "postNews(int,QString)\0sendNews(int)\0"
    "removeClient(int)\0userId,chatId\0"
    "removeFromPrivateChat(int,int)\0id,info\0"
    "setClientInfo(int,QString)\0"
    "sendAccountsList(int)\0sendGroupsList(int)\0"
    "sendUserSpec(int,QString)\0id,path\0"
    "sendFileList(int,QString)\0"
    "sendFileStat(int,QString)\0"
    "sendGroupSpec(int,QString)\0id,userId\0"
    "sendUserInfo(int,int)\0sendServerInfo(int)\0"
    "socket\0registerClient(WiredSocket*)\0"
};

const QMetaObject QWServerCore::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QWServerCore,
      qt_meta_data_QWServerCore, 0 }
};

const QMetaObject *QWServerCore::metaObject() const
{
    return &staticMetaObject;
}

void *QWServerCore::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWServerCore))
	return static_cast<void*>(const_cast< QWServerCore*>(this));
    return QObject::qt_metacast(_clname);
}

int QWServerCore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: handleTransaction((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 1: sendNewsGroups((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 2: sendNewsGroupArticles((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 3: checkLogin((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 4: sendServerBanner((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 5: sendUserlist((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 6: sendUserInfo((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 7: sendMotd((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 8: broadcastUserStatusChanged((*reinterpret_cast< const ClassWiredUser(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 9: clearNews((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 10: createPrivateChat((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 11: createUser((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 12: createGroup((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 13: editUser((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 14: editGroup((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 15: deleteUser((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 16: deleteGroup((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 17: declinePrivateChat((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 18: broadcastBroadcast((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 19: broadcastChat((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 20: broadcastChatTopic((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 21: broadcastUserImageChanged((*reinterpret_cast< const ClassWiredUser(*)>(_a[1]))); break;
        case 22: deliverPrivateMessage((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QWTransaction(*)>(_a[2]))); break;
        case 23: inviteUserToChat((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 24: joinPrivateChat((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 25: kickUser((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const bool(*)>(_a[4]))); break;
        case 26: postNews((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 27: sendNews((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 28: removeClient((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 29: removeFromPrivateChat((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 30: setClientInfo((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 31: sendAccountsList((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 32: sendGroupsList((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 33: sendUserSpec((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 34: sendFileList((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 35: sendFileStat((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 36: sendGroupSpec((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 37: sendUserInfo((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 38: sendServerInfo((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 39: registerClient((*reinterpret_cast< WiredSocket*(*)>(_a[1]))); break;
        }
        _id -= 40;
    }
    return _id;
}
