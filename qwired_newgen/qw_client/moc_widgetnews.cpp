/****************************************************************************
** Meta object code from reading C++ file 'widgetnews.h'
**
** Created: Sun Jun 29 19:11:11 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetnews.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetnews.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetNews[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      28,   11,   11,   11, 0x05,
      48,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      87,   63,   11,   11, 0x0a,
     124,   63,   11,   11, 0x0a,
     166,   11,   11,   11, 0x0a,
     185,   11,   11,   11, 0x0a,
     206,  198,   11,   11, 0x08,
     235,  198,   11,   11, 0x08,
     261,  198,   11,   11, 0x08,
     289,   11,   11,   11, 0x08,
     303,   11,   11,   11, 0x08,
     315,   11,   11,   11, 0x08,
     367,  352,   11,   11, 0x08,
     404,   11,   11,   11, 0x08,
     450,  426,   11,   11, 0x08,
     504,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetNews[] = {
    "WidgetNews\0\0doRefreshNews()\0"
    "doPostNews(QString)\0onDeleteNews()\0"
    "theNick,theTime,thePost\0"
    "addNewsItem(QString,QString,QString)\0"
    "addFreshNewsItem(QString,QString,QString)\0"
    "onServerNewsDone()\0doSendNews()\0checked\0"
    "on_fBtnRefresh_clicked(bool)\0"
    "on_fBtnPost_clicked(bool)\0"
    "on_fBtnDelete_clicked(bool)\0reloadPrefs()\0"
    "initPrefs()\0on_listGroups_itemSelectionChanged()\0"
    "nid,name,count\0onNewsGroupListItem(int,QString,int)\0"
    "onNewsGroupListDone()\0aid,subject,author,date\0"
    "onNewsGroupArticleListItem(int,QString,QString,QDate)\0"
    "onNewsGroupArticleListDone()\0"
};

const QMetaObject WidgetNews::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetNews,
      qt_meta_data_WidgetNews, 0 }
};

const QMetaObject *WidgetNews::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetNews::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetNews))
	return static_cast<void*>(const_cast< WidgetNews*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetNews::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: doRefreshNews(); break;
        case 1: doPostNews((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: onDeleteNews(); break;
        case 3: addNewsItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 4: addFreshNewsItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 5: onServerNewsDone(); break;
        case 6: doSendNews(); break;
        case 7: on_fBtnRefresh_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: on_fBtnPost_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: on_fBtnDelete_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: reloadPrefs(); break;
        case 11: initPrefs(); break;
        case 12: on_listGroups_itemSelectionChanged(); break;
        case 13: onNewsGroupListItem((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 14: onNewsGroupListDone(); break;
        case 15: onNewsGroupArticleListItem((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QDate(*)>(_a[4]))); break;
        case 16: onNewsGroupArticleListDone(); break;
        }
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void WidgetNews::doRefreshNews()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void WidgetNews::doPostNews(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WidgetNews::onDeleteNews()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
