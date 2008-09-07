/****************************************************************************
** Meta object code from reading C++ file 'modelfiletransfers.h'
**
** Created: Sun Jun 29 19:05:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/modelfiletransfers.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modelfiletransfers.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ModelFileTransfers[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      32,   20,   19,   19, 0x08,
      68,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ModelFileTransfers[] = {
    "ModelFileTransfers\0\0theTransfer\0"
    "updateTransfers(ClassWiredTransfer)\0"
    "reloadTransfers()\0"
};

const QMetaObject ModelFileTransfers::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_ModelFileTransfers,
      qt_meta_data_ModelFileTransfers, 0 }
};

const QMetaObject *ModelFileTransfers::metaObject() const
{
    return &staticMetaObject;
}

void *ModelFileTransfers::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModelFileTransfers))
	return static_cast<void*>(const_cast< ModelFileTransfers*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int ModelFileTransfers::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateTransfers((*reinterpret_cast< const ClassWiredTransfer(*)>(_a[1]))); break;
        case 1: reloadTransfers(); break;
        }
        _id -= 2;
    }
    return _id;
}
