/****************************************************************************
** Meta object code from reading C++ file 'wiredsingleton.h'
**
** Created: Sun Jun 29 19:11:11 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "general/wiredsingleton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wiredsingleton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WiredSingleton[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   31,   15,   15, 0x08,
      62,   15,   15,   15, 0x08,
     110,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WiredSingleton[] = {
    "WiredSingleton\0\0prefsChanged()\0obj\0"
    "sessionDestroyed(QObject*)\0"
    "showTrayMenu(QSystemTrayIcon::ActivationReason)\0"
    "notifyPrefsChanged()\0"
};

const QMetaObject WiredSingleton::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WiredSingleton,
      qt_meta_data_WiredSingleton, 0 }
};

const QMetaObject *WiredSingleton::metaObject() const
{
    return &staticMetaObject;
}

void *WiredSingleton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WiredSingleton))
	return static_cast<void*>(const_cast< WiredSingleton*>(this));
    return QObject::qt_metacast(_clname);
}

int WiredSingleton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: prefsChanged(); break;
        case 1: sessionDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 2: showTrayMenu((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 3: notifyPrefsChanged(); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void WiredSingleton::prefsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
