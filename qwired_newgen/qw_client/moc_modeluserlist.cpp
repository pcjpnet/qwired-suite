/****************************************************************************
** Meta object code from reading C++ file 'modeluserlist.h'
**
** Created: Sun Jun 29 19:05:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/modeluserlist.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modeluserlist.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ModelUserList[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      25,   15,   14,   14, 0x08,
      57,   43,   14,   14, 0x08,
     126,  108,   14,   14, 0x08,
     165,  108,   14,   14, 0x08,
     202,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ModelUserList[] = {
    "ModelUserList\0\0theChatID\0onDataUpdate(int)\0"
    "theOld,theNew\0"
    "onServerUserChanged(ClassWiredUser,ClassWiredUser)\0"
    "theChatID,theUser\0"
    "onServerUserJoined(int,ClassWiredUser)\0"
    "onServerUserLeft(int,ClassWiredUser)\0"
    "reloadPrefs()\0"
};

const QMetaObject ModelUserList::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_ModelUserList,
      qt_meta_data_ModelUserList, 0 }
};

const QMetaObject *ModelUserList::metaObject() const
{
    return &staticMetaObject;
}

void *ModelUserList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModelUserList))
	return static_cast<void*>(const_cast< ModelUserList*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int ModelUserList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onDataUpdate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: onServerUserChanged((*reinterpret_cast< const ClassWiredUser(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 2: onServerUserJoined((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 3: onServerUserLeft((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 4: reloadPrefs(); break;
        }
        _id -= 5;
    }
    return _id;
}
