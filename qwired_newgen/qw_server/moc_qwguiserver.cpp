/****************************************************************************
** Meta object code from reading C++ file 'qwguiserver.h'
**
** Created: Sat Jun 28 12:53:39 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qwguiserver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwguiserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QWGuiServer[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      32,   12,   12,   12, 0x08,
      50,   12,   12,   12, 0x08,
      61,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QWGuiServer[] = {
    "QWGuiServer\0\0acceptConnection()\0"
    "clearConnection()\0readData()\0startServer()\0"
};

const QMetaObject QWGuiServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QWGuiServer,
      qt_meta_data_QWGuiServer, 0 }
};

const QMetaObject *QWGuiServer::metaObject() const
{
    return &staticMetaObject;
}

void *QWGuiServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWGuiServer))
	return static_cast<void*>(const_cast< QWGuiServer*>(this));
    return QObject::qt_metacast(_clname);
}

int QWGuiServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: acceptConnection(); break;
        case 1: clearConnection(); break;
        case 2: readData(); break;
        case 3: startServer(); break;
        }
        _id -= 4;
    }
    return _id;
}
