/****************************************************************************
** Meta object code from reading C++ file 'widgetmotd.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetmotd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetmotd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetMotd[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_WidgetMotd[] = {
    "WidgetMotd\0"
};

const QMetaObject WidgetMotd::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetMotd,
      qt_meta_data_WidgetMotd, 0 }
};

const QMetaObject *WidgetMotd::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetMotd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetMotd))
	return static_cast<void*>(const_cast< WidgetMotd*>(this));
    if (!strcmp(_clname, "Ui_WidgetMotd"))
	return static_cast< Ui_WidgetMotd*>(const_cast< WidgetMotd*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetMotd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
