/****************************************************************************
** Meta object code from reading C++ file 'widgetuserinfo.h'
**
** Created: Sat Jun 28 11:12:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetuserinfo.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetuserinfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetUserInfo[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_WidgetUserInfo[] = {
    "WidgetUserInfo\0"
};

const QMetaObject WidgetUserInfo::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetUserInfo,
      qt_meta_data_WidgetUserInfo, 0 }
};

const QMetaObject *WidgetUserInfo::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetUserInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetUserInfo))
	return static_cast<void*>(const_cast< WidgetUserInfo*>(this));
    if (!strcmp(_clname, "Ui::WidgetUserInfo"))
	return static_cast< Ui::WidgetUserInfo*>(const_cast< WidgetUserInfo*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetUserInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
