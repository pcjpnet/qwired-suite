/****************************************************************************
** Meta object code from reading C++ file 'WidgetColorButton.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/WidgetColorButton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WidgetColorButton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetColorButton[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      41,   18,   18,   18, 0x08,
      56,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WidgetColorButton[] = {
    "WidgetColorButton\0\0colorSelected(QColor)\0"
    "requestColor()\0setColor(QColor)\0"
};

const QMetaObject WidgetColorButton::staticMetaObject = {
    { &QToolButton::staticMetaObject, qt_meta_stringdata_WidgetColorButton,
      qt_meta_data_WidgetColorButton, 0 }
};

const QMetaObject *WidgetColorButton::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetColorButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetColorButton))
	return static_cast<void*>(const_cast< WidgetColorButton*>(this));
    return QToolButton::qt_metacast(_clname);
}

int WidgetColorButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: colorSelected((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 1: requestColor(); break;
        case 2: setColor((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void WidgetColorButton::colorSelected(QColor _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
