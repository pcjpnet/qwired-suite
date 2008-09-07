/****************************************************************************
** Meta object code from reading C++ file 'widgetsendprivmsg.h'
**
** Created: Sun Jun 29 19:05:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetsendprivmsg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetsendprivmsg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetSendPrivMsg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   19,   18,   18, 0x0a,
      61,   57,   18,   18, 0x0a,
      83,   18,   18,   18, 0x0a,
      99,   18,   18,   18, 0x08,
     136,  125,   18,   18, 0x08,
     195,  169,   18,   18, 0x08,
     243,  227,   18,   18, 0x08,
     310,  292,   18,   18, 0x08,
     345,   18,   18,   18, 0x08,
     373,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetSendPrivMsg[] = {
    "WidgetSendPrivMsg\0\0user\0"
    "initUserMessages(ClassWiredUser)\0uid\0"
    "showUserMessages(int)\0setInputFocus()\0"
    "on_fInput_returnPressed()\0currentRow\0"
    "on_fUsers_currentRowChanged(int)\0"
    "uid,text,senderId,msgType\0"
    "addMessage(int,QString,int,int)\0"
    "oldUser,newUser\0"
    "handleUserChanged(ClassWiredUser,ClassWiredUser)\0"
    "theChatID,theUser\0handleUserLeft(int,ClassWiredUser)\0"
    "on_fBtnDelete_clicked(bool)\0"
    "on_fBtnSave_clicked(bool)\0"
};

const QMetaObject WidgetSendPrivMsg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetSendPrivMsg,
      qt_meta_data_WidgetSendPrivMsg, 0 }
};

const QMetaObject *WidgetSendPrivMsg::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetSendPrivMsg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetSendPrivMsg))
	return static_cast<void*>(const_cast< WidgetSendPrivMsg*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetSendPrivMsg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: initUserMessages((*reinterpret_cast< const ClassWiredUser(*)>(_a[1]))); break;
        case 1: showUserMessages((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 2: setInputFocus(); break;
        case 3: on_fInput_returnPressed(); break;
        case 4: on_fUsers_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: addMessage((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4]))); break;
        case 6: handleUserChanged((*reinterpret_cast< const ClassWiredUser(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 7: handleUserLeft((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const ClassWiredUser(*)>(_a[2]))); break;
        case 8: on_fBtnDelete_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: on_fBtnSave_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 10;
    }
    return _id;
}
