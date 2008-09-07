/****************************************************************************
** Meta object code from reading C++ file 'widgetaccounts.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetaccounts.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetaccounts.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetAccounts[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      24,   16,   15,   15, 0x05,
      51,   16,   15,   15, 0x05,
      79,   15,   15,   15, 0x05,
     108,   15,   15,   15, 0x05,
     138,   15,   15,   15, 0x05,
     159,   15,   15,   15, 0x05,
     181,   15,   15,   15, 0x05,
     208,   15,   15,   15, 0x05,
     233,   15,   15,   15, 0x05,
     261,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     304,  287,   15,   15, 0x08,
     367,   15,   15,   15, 0x08,
     390,   15,   15,   15, 0x08,
     414,   15,   15,   15, 0x08,
     435,   15,   15,   15, 0x08,
     469,   15,   15,   15, 0x08,
     504,   15,   15,   15, 0x0a,
     533,   15,   15,   15, 0x0a,
     563,   15,   15,   15, 0x0a,
     592,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WidgetAccounts[] = {
    "WidgetAccounts\0\0theName\0"
    "userSpecRequested(QString)\0"
    "groupSpecRequested(QString)\0"
    "saveUserSpec(ClassWiredUser)\0"
    "saveGroupSpec(ClassWiredUser)\0"
    "userDeleted(QString)\0groupDeleted(QString)\0"
    "createUser(ClassWiredUser)\0"
    "editUser(ClassWiredUser)\0"
    "createGroup(ClassWiredUser)\0"
    "editGroup(ClassWiredUser)\0current,previous\0"
    "on_fList_currentItemChanged(QListWidgetItem*,QListWidgetItem*)\0"
    "on_fBtnApply_clicked()\0on_fBtnDelete_clicked()\0"
    "on_fBtnNew_clicked()\0"
    "on_fType_currentIndexChanged(int)\0"
    "on_fGroup_currentIndexChanged(int)\0"
    "appendUserNames(QStringList)\0"
    "appendGroupNames(QStringList)\0"
    "loadUserSpec(ClassWiredUser)\0"
    "loadGroupSpec(ClassWiredUser)\0"
};

const QMetaObject WidgetAccounts::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetAccounts,
      qt_meta_data_WidgetAccounts, 0 }
};

const QMetaObject *WidgetAccounts::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetAccounts::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetAccounts))
	return static_cast<void*>(const_cast< WidgetAccounts*>(this));
    if (!strcmp(_clname, "Ui::WidgetAccounts"))
	return static_cast< Ui::WidgetAccounts*>(const_cast< WidgetAccounts*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetAccounts::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: userSpecRequested((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: groupSpecRequested((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: saveUserSpec((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 3: saveGroupSpec((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 4: userDeleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: groupDeleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: createUser((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 7: editUser((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 8: createGroup((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 9: editGroup((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 10: on_fList_currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 11: on_fBtnApply_clicked(); break;
        case 12: on_fBtnDelete_clicked(); break;
        case 13: on_fBtnNew_clicked(); break;
        case 14: on_fType_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: on_fGroup_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: appendUserNames((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 17: appendGroupNames((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 18: loadUserSpec((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        case 19: loadGroupSpec((*reinterpret_cast< ClassWiredUser(*)>(_a[1]))); break;
        }
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void WidgetAccounts::userSpecRequested(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WidgetAccounts::groupSpecRequested(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WidgetAccounts::saveUserSpec(ClassWiredUser _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WidgetAccounts::saveGroupSpec(ClassWiredUser _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WidgetAccounts::userDeleted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void WidgetAccounts::groupDeleted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void WidgetAccounts::createUser(ClassWiredUser _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void WidgetAccounts::editUser(ClassWiredUser _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void WidgetAccounts::createGroup(ClassWiredUser _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void WidgetAccounts::editGroup(ClassWiredUser _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
