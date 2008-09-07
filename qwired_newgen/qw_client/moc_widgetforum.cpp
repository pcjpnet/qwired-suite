/****************************************************************************
** Meta object code from reading C++ file 'widgetforum.h'
**
** Created: Sun Jun 29 19:05:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetforum.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetforum.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetForum[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      38,   13,   12,   12, 0x0a,
      79,   72,   12,   12, 0x0a,
     111,  105,   12,   12, 0x08,
     148,   12,   12,   12, 0x08,
     178,   12,   12,   12, 0x08,
     199,   12,   12,   12, 0x08,
     221,   12,   12,   12, 0x08,
     242,   12,   12,   12, 0x08,
     264,   12,   12,   12, 0x08,
     303,  286,   12,   12, 0x08,
     368,  361,   12,   12, 0x08,
     398,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetForum[] = {
    "WidgetForum\0\0theUser,theText,theEmote\0"
    "writeToChat(QString,QString,bool)\0"
    "theMsg\0writeEventToChat(QString)\0index\0"
    "on_fUsers_doubleClicked(QModelIndex)\0"
    "on_fChatInput_returnPressed()\0"
    "on_fBtnMsg_clicked()\0on_fBtnKick_clicked()\0"
    "on_fBtnBan_clicked()\0on_fBtnInfo_clicked()\0"
    "on_fBtnChat_clicked()\0current,previous\0"
    "onUserlistSelectionChanged(QItemSelection,QItemSelection)\0"
    "action\0inviteMenuTriggered(QAction*)\0"
    "reloadPrefs()\0"
};

const QMetaObject WidgetForum::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetForum,
      qt_meta_data_WidgetForum, 0 }
};

const QMetaObject *WidgetForum::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetForum::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetForum))
	return static_cast<void*>(const_cast< WidgetForum*>(this));
    if (!strcmp(_clname, "Ui::WidgetForum"))
	return static_cast< Ui::WidgetForum*>(const_cast< WidgetForum*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetForum::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: writeToChat((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: writeEventToChat((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: on_fUsers_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: on_fChatInput_returnPressed(); break;
        case 4: on_fBtnMsg_clicked(); break;
        case 5: on_fBtnKick_clicked(); break;
        case 6: on_fBtnBan_clicked(); break;
        case 7: on_fBtnInfo_clicked(); break;
        case 8: on_fBtnChat_clicked(); break;
        case 9: onUserlistSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 10: inviteMenuTriggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 11: reloadPrefs(); break;
        }
        _id -= 12;
    }
    return _id;
}
