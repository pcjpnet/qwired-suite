/****************************************************************************
** Meta object code from reading C++ file 'widgettransfers.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgettransfers.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgettransfers.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetTransfers[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   16,   16,   16, 0x08,
      85,   83,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetTransfers[] = {
    "WidgetTransfers\0\0transferCancelled(ClassWiredTransfer)\0"
    "on_fBtnCancel_clicked(bool)\0,\0"
    "transferListSelectionChanged(QItemSelection,QItemSelection)\0"
};

const QMetaObject WidgetTransfers::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetTransfers,
      qt_meta_data_WidgetTransfers, 0 }
};

const QMetaObject *WidgetTransfers::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetTransfers::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetTransfers))
	return static_cast<void*>(const_cast< WidgetTransfers*>(this));
    if (!strcmp(_clname, "Ui::WidgetTransfers"))
	return static_cast< Ui::WidgetTransfers*>(const_cast< WidgetTransfers*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetTransfers::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: transferCancelled((*reinterpret_cast< ClassWiredTransfer(*)>(_a[1]))); break;
        case 1: on_fBtnCancel_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: transferListSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void WidgetTransfers::transferCancelled(ClassWiredTransfer _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
