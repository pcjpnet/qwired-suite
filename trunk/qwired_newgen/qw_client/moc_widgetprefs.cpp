/****************************************************************************
** Meta object code from reading C++ file 'widgetprefs.h'
**
** Created: Sat Jun 28 11:12:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetprefs.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetprefs.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetPrefs[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      25,   12,   12,   12, 0x08,
      56,   12,   12,   12, 0x08,
      91,   12,   12,   12, 0x08,
     122,   12,   12,   12, 0x08,
     151,   12,   12,   12, 0x08,
     179,   12,   12,   12, 0x08,
     219,  211,   12,   12, 0x08,
     263,  211,   12,   12, 0x08,
     313,  307,   12,   12, 0x08,
     361,   12,   12,   12, 0x08,
     389,   12,   12,   12, 0x08,
     420,   12,   12,   12, 0x08,
     448,   12,   12,   12, 0x08,
     479,   12,   12,   12, 0x08,
     518,   12,   12,   12, 0x08,
     552,   12,   12,   12, 0x08,
     589,   12,   12,   12, 0x08,
     630,   12,   12,   12, 0x08,
     652,   12,   12,   12, 0x08,
     677,   12,   12,   12, 0x08,
     706,   12,   12,   12, 0x08,
     735,   12,   12,   12, 0x08,
     765,   12,   12,   12, 0x08,
     798,   12,   12,   12, 0x08,
     834,   12,   12,   12, 0x08,
     872,   12,   12,   12, 0x08,
     907,   12,   12,   12, 0x08,
     933,   12,   12,   12, 0x08,
     953,  307,   12,   12, 0x08,
     990,  307,   12,   12, 0x08,
    1033,  307,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetPrefs[] = {
    "WidgetPrefs\0\0savePrefs()\0"
    "on_btnGeneralSetIcon_clicked()\0"
    "on_btnGeneralDefaultIcon_clicked()\0"
    "on_fNickname_editingFinished()\0"
    "on_fStatus_editingFinished()\0"
    "on_fBtnIfChatFont_clicked()\0"
    "on_fBtnIfUserlistFont_clicked()\0checked\0"
    "on_fBtnIfUserlistCompactLarge_toggled(bool)\0"
    "on_fBtnIfUserlistCompactSmall_toggled(bool)\0"
    "state\0on_fBtnIfUserlistAlternateRow_stateChanged(int)\0"
    "on_fBtnIfNewsFont_clicked()\0"
    "on_btnFilesFolderSet_clicked()\0"
    "on_fBtnTrackerNew_clicked()\0"
    "on_fBtnTrackerDelete_clicked()\0"
    "on_fTrackerList_currentRowChanged(int)\0"
    "on_fTrackerName_editingFinished()\0"
    "on_fTrackerAddress_editingFinished()\0"
    "on_fBookmarksList_currentRowChanged(int)\0"
    "on_btnBMAdd_clicked()\0on_btnBMRemove_clicked()\0"
    "on_fBMName_editingFinished()\0"
    "on_fBMAddr_editingFinished()\0"
    "on_fBMLogin_editingFinished()\0"
    "on_fBMPassword_editingFinished()\0"
    "on_fBMAutoConnect_stateChanged(int)\0"
    "on_fEventsList_currentRowChanged(int)\0"
    "on_fEventsSysCmd_editingFinished()\0"
    "on_fEventsSound_clicked()\0on_fBtnOk_clicked()\0"
    "on_fEventsPostChat_stateChanged(int)\0"
    "on_fEventsDisplayTrayMsg_stateChanged(int)\0"
    "on_fEventsSysCmdEnabled_stateChanged(int)\0"
};

const QMetaObject WidgetPrefs::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetPrefs,
      qt_meta_data_WidgetPrefs, 0 }
};

const QMetaObject *WidgetPrefs::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetPrefs::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetPrefs))
	return static_cast<void*>(const_cast< WidgetPrefs*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetPrefs::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: savePrefs(); break;
        case 1: on_btnGeneralSetIcon_clicked(); break;
        case 2: on_btnGeneralDefaultIcon_clicked(); break;
        case 3: on_fNickname_editingFinished(); break;
        case 4: on_fStatus_editingFinished(); break;
        case 5: on_fBtnIfChatFont_clicked(); break;
        case 6: on_fBtnIfUserlistFont_clicked(); break;
        case 7: on_fBtnIfUserlistCompactLarge_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: on_fBtnIfUserlistCompactSmall_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: on_fBtnIfUserlistAlternateRow_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: on_fBtnIfNewsFont_clicked(); break;
        case 11: on_btnFilesFolderSet_clicked(); break;
        case 12: on_fBtnTrackerNew_clicked(); break;
        case 13: on_fBtnTrackerDelete_clicked(); break;
        case 14: on_fTrackerList_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: on_fTrackerName_editingFinished(); break;
        case 16: on_fTrackerAddress_editingFinished(); break;
        case 17: on_fBookmarksList_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: on_btnBMAdd_clicked(); break;
        case 19: on_btnBMRemove_clicked(); break;
        case 20: on_fBMName_editingFinished(); break;
        case 21: on_fBMAddr_editingFinished(); break;
        case 22: on_fBMLogin_editingFinished(); break;
        case 23: on_fBMPassword_editingFinished(); break;
        case 24: on_fBMAutoConnect_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: on_fEventsList_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: on_fEventsSysCmd_editingFinished(); break;
        case 27: on_fEventsSound_clicked(); break;
        case 28: on_fBtnOk_clicked(); break;
        case 29: on_fEventsPostChat_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: on_fEventsDisplayTrayMsg_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 31: on_fEventsSysCmdEnabled_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 32;
    }
    return _id;
}
