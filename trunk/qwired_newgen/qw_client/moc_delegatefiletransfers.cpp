/****************************************************************************
** Meta object code from reading C++ file 'delegatefiletransfers.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/delegatefiletransfers.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'delegatefiletransfers.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_DelegateFileTransfers[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_DelegateFileTransfers[] = {
    "DelegateFileTransfers\0"
};

const QMetaObject DelegateFileTransfers::staticMetaObject = {
    { &QAbstractItemDelegate::staticMetaObject, qt_meta_stringdata_DelegateFileTransfers,
      qt_meta_data_DelegateFileTransfers, 0 }
};

const QMetaObject *DelegateFileTransfers::metaObject() const
{
    return &staticMetaObject;
}

void *DelegateFileTransfers::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DelegateFileTransfers))
	return static_cast<void*>(const_cast< DelegateFileTransfers*>(this));
    return QAbstractItemDelegate::qt_metacast(_clname);
}

int DelegateFileTransfers::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
