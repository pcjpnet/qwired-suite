/****************************************************************************
** Meta object code from reading C++ file 'widgetconnect.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetconnect.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetconnect.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetConnect[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      44,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      85,   14,   14,   14, 0x08,
     116,  109,   14,   14, 0x08,
     143,   14,   14,   14, 0x08,
     169,  159,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WidgetConnect[] = {
    "WidgetConnect\0\0theHost,theLogin,thePassword\0"
    "onConnnectReady(QString,QString,QString)\0"
    "on_btnConnect_clicked()\0action\0"
    "bookmarkSelected(QAction*)\0loadBookmarks()\0"
    "value,max\0setProgressBar(int,int)\0"
};

const QMetaObject WidgetConnect::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetConnect,
      qt_meta_data_WidgetConnect, 0 }
};

const QMetaObject *WidgetConnect::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetConnect::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetConnect))
	return static_cast<void*>(const_cast< WidgetConnect*>(this));
    if (!strcmp(_clname, "Ui::WidgetConnect"))
	return static_cast< Ui::WidgetConnect*>(const_cast< WidgetConnect*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetConnect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onConnnectReady((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 1: on_btnConnect_clicked(); break;
        case 2: bookmarkSelected((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: loadBookmarks(); break;
        case 4: setProgressBar((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void WidgetConnect::onConnnectReady(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
