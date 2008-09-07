/****************************************************************************
** Meta object code from reading C++ file 'WidgetFileSearch.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/WidgetFileSearch.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WidgetFileSearch.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetFileSearch[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      28,   18,   17,   17, 0x05,
      52,   44,   17,   17, 0x05,
      82,   74,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     102,   17,   17,   17, 0x08,
     132,   17,   17,   17, 0x08,
     163,  159,   17,   17, 0x08,
     198,   17,   17,   17, 0x08,
     237,  226,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WidgetFileSearch[] = {
    "WidgetFileSearch\0\0theSearch\0search(QString)\0"
    "theFile\0downloadFile(QString)\0thePath\0"
    "revealFile(QString)\0on_fBtnDownload_clicked(bool)\0"
    "on_fSearch_returnPressed()\0row\0"
    "on_fResults_currentRowChanged(int)\0"
    "on_fBtnReveal_clicked(bool)\0theResults\0"
    "updateResults(QList<ClassWiredFile>)\0"
};

const QMetaObject WidgetFileSearch::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetFileSearch,
      qt_meta_data_WidgetFileSearch, 0 }
};

const QMetaObject *WidgetFileSearch::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetFileSearch::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetFileSearch))
	return static_cast<void*>(const_cast< WidgetFileSearch*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetFileSearch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: search((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: downloadFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: revealFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: on_fBtnDownload_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: on_fSearch_returnPressed(); break;
        case 5: on_fResults_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: on_fBtnReveal_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: updateResults((*reinterpret_cast< QList<ClassWiredFile>(*)>(_a[1]))); break;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void WidgetFileSearch::search(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WidgetFileSearch::downloadFile(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WidgetFileSearch::revealFile(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
