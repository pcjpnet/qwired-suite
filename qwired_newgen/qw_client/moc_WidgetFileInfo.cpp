/****************************************************************************
** Meta object code from reading C++ file 'WidgetFileInfo.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/WidgetFileInfo.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WidgetFileInfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetFileInfo[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_WidgetFileInfo[] = {
    "WidgetFileInfo\0"
};

const QMetaObject WidgetFileInfo::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetFileInfo,
      qt_meta_data_WidgetFileInfo, 0 }
};

const QMetaObject *WidgetFileInfo::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetFileInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetFileInfo))
	return static_cast<void*>(const_cast< WidgetFileInfo*>(this));
    if (!strcmp(_clname, "Ui::WidgetFileInfo"))
	return static_cast< Ui::WidgetFileInfo*>(const_cast< WidgetFileInfo*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetFileInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
