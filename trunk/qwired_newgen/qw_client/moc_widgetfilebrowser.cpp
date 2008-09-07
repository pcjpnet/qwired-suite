/****************************************************************************
** Meta object code from reading C++ file 'widgetfilebrowser.h'
**
** Created: Sun Jun 29 19:11:11 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/widgetfilebrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetfilebrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WidgetFileBrowser[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x08,
      56,   18,   18,   18, 0x08,
      94,   78,   18,   18, 0x0a,
     140,  134,   18,   18, 0x0a,
     176,  134,   18,   18, 0x0a,
     214,  206,   18,   18, 0x0a,
     240,  206,   18,   18, 0x0a,
     270,  206,   18,   18, 0x0a,
     298,  206,   18,   18, 0x0a,
     326,  206,   18,   18, 0x0a,
     357,  206,   18,   18, 0x0a,
     383,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WidgetFileBrowser[] = {
    "WidgetFileBrowser\0\0"
    "fileTransferDone(ClassWiredTransfer)\0"
    "downloadFile(QString)\0thePath,theFree\0"
    "doUpdateBrowserStats(QString,qlonglong)\0"
    "index\0on_fList_doubleClicked(QModelIndex)\0"
    "on_fList_clicked(QModelIndex)\0checked\0"
    "on_fBtnBack_clicked(bool)\0"
    "on_fBtnDownload_clicked(bool)\0"
    "on_fBtnUpload_clicked(bool)\0"
    "on_fBtnDelete_clicked(bool)\0"
    "on_fBtnNewFolder_clicked(bool)\0"
    "on_fBtnInfo_clicked(bool)\0"
    "on_fFilter_textEdited(QString)\0"
};

const QMetaObject WidgetFileBrowser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetFileBrowser,
      qt_meta_data_WidgetFileBrowser, 0 }
};

const QMetaObject *WidgetFileBrowser::metaObject() const
{
    return &staticMetaObject;
}

void *WidgetFileBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetFileBrowser))
	return static_cast<void*>(const_cast< WidgetFileBrowser*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetFileBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileTransferDone((*reinterpret_cast< ClassWiredTransfer(*)>(_a[1]))); break;
        case 1: downloadFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: doUpdateBrowserStats((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< qlonglong(*)>(_a[2]))); break;
        case 3: on_fList_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: on_fList_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: on_fBtnBack_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: on_fBtnDownload_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: on_fBtnUpload_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: on_fBtnDelete_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: on_fBtnNewFolder_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: on_fBtnInfo_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: on_fFilter_textEdited((*reinterpret_cast< QString(*)>(_a[1]))); break;
        }
        _id -= 12;
    }
    return _id;
}
