/****************************************************************************
** Meta object code from reading C++ file 'delegateuserlist.h'
**
** Created: Sat Jun 28 11:12:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/delegateuserlist.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'delegateuserlist.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_DelegateUserlist[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DelegateUserlist[] = {
    "DelegateUserlist\0\0reloadPrefs()\0"
};

const QMetaObject DelegateUserlist::staticMetaObject = {
    { &QItemDelegate::staticMetaObject, qt_meta_stringdata_DelegateUserlist,
      qt_meta_data_DelegateUserlist, 0 }
};

const QMetaObject *DelegateUserlist::metaObject() const
{
    return &staticMetaObject;
}

void *DelegateUserlist::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DelegateUserlist))
	return static_cast<void*>(const_cast< DelegateUserlist*>(this));
    return QItemDelegate::qt_metacast(_clname);
}

int DelegateUserlist::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reloadPrefs(); break;
        }
        _id -= 1;
    }
    return _id;
}
