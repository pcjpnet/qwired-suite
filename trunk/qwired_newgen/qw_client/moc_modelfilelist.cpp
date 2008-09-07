/****************************************************************************
** Meta object code from reading C++ file 'modelfilelist.h'
**
** Created: Sat Jun 28 11:12:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/modelfilelist.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modelfilelist.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ModelFileList[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      20,   15,   14,   14, 0x0a,
      78,   57,   14,   14, 0x0a,
     118,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ModelFileList[] = {
    "ModelFileList\0\0file\0"
    "onServerFileListItem(ClassWiredFile)\0"
    "thePath,theFreeSpace\0"
    "onServerFileListDone(QString,qlonglong)\0"
    "clearList()\0"
};

const QMetaObject ModelFileList::staticMetaObject = {
    { &QStandardItemModel::staticMetaObject, qt_meta_stringdata_ModelFileList,
      qt_meta_data_ModelFileList, 0 }
};

const QMetaObject *ModelFileList::metaObject() const
{
    return &staticMetaObject;
}

void *ModelFileList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModelFileList))
	return static_cast<void*>(const_cast< ModelFileList*>(this));
    return QStandardItemModel::qt_metacast(_clname);
}

int ModelFileList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStandardItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onServerFileListItem((*reinterpret_cast< ClassWiredFile(*)>(_a[1]))); break;
        case 1: onServerFileListDone((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< qlonglong(*)>(_a[2]))); break;
        case 2: clearList(); break;
        }
        _id -= 3;
    }
    return _id;
}
