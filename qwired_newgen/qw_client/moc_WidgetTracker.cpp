/****************************************************************************
** Meta object code from reading C++ file 'WidgetTracker.h'
**
** Created: Sun Jun 29 19:05:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/WidgetTracker.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WidgetTracker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetTracker[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      48,   40,   14,   14, 0x08,
     100,   98,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetTracker[] = {
    "WidgetTracker\0\0on_fBtnRefresh_clicked()\0"
    "theList\0trackerServersReceived(QList<ClassTrackerServer>)\0"
    ",\0handleSocketError(QString,int)\0"
};

const QMetaObject WidgetTracker::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetTracker,
      qt_meta_data_WidgetTracker, 0 }
};

const QMetaObject *WidgetTracker::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetTracker::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetTracker))
	return static_cast<void*>(const_cast< WidgetTracker*>(this));
    if (!strcmp(_clname, "Ui::WidgetTracker"))
	return static_cast< Ui::WidgetTracker*>(const_cast< WidgetTracker*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetTracker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_fBtnRefresh_clicked(); break;
        case 1: trackerServersReceived((*reinterpret_cast< QList<ClassTrackerServer>(*)>(_a[1]))); break;
        case 2: handleSocketError((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        }
        _id -= 3;
    }
    return _id;
}
