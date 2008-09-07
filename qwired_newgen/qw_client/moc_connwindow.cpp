/****************************************************************************
** Meta object code from reading C++ file 'connwindow.h'
**
** Created: Sun Jun 29 19:05:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/connwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ConnWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      28,   11,   11,   11, 0x0a,
      50,   44,   11,   11, 0x08,
      76,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ConnWindow[] = {
    "ConnWindow\0\0onCloseButton()\0toggleVisible()\0"
    "index\0onTab_currentChanged(int)\0"
    "on_actionAbout_triggered(bool)\0"
};

const QMetaObject ConnWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ConnWindow,
      qt_meta_data_ConnWindow, 0 }
};

const QMetaObject *ConnWindow::metaObject() const
{
    return &staticMetaObject;
}

void *ConnWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConnWindow))
	return static_cast<void*>(const_cast< ConnWindow*>(this));
    if (!strcmp(_clname, "Ui::MainWindow"))
	return static_cast< Ui::MainWindow*>(const_cast< ConnWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ConnWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onCloseButton(); break;
        case 1: toggleVisible(); break;
        case 2: onTab_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: on_actionAbout_triggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
    return _id;
}
