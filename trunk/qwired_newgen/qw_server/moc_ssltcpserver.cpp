/****************************************************************************
** Meta object code from reading C++ file 'ssltcpserver.h'
**
** Created: Sat Jun 28 12:53:39 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ssltcpserver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ssltcpserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_SslTcpServer[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_SslTcpServer[] = {
    "SslTcpServer\0\0newSslConnection()\0"
};

const QMetaObject SslTcpServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_SslTcpServer,
      qt_meta_data_SslTcpServer, 0 }
};

const QMetaObject *SslTcpServer::metaObject() const
{
    return &staticMetaObject;
}

void *SslTcpServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SslTcpServer))
	return static_cast<void*>(const_cast< SslTcpServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int SslTcpServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newSslConnection(); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void SslTcpServer::newSslConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
