/****************************************************************************
** Meta object code from reading C++ file 'wiredtransfersocket.h'
**
** Created: Sat Jun 28 11:12:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "wired/wiredtransfersocket.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wiredtransfersocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_WiredTransferSocket[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,
      61,   20,   20,   20, 0x05,
      98,   20,   20,   20, 0x05,
     136,   20,   20,   20, 0x05,
     175,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
     229,   20,   20,   20, 0x0a,
     246,   20,   20,   20, 0x08,
     266,   20,   20,   20, 0x08,
     282,   20,   20,   20, 0x08,
     312,  305,   20,   20, 0x08,
     360,  350,   20,   20, 0x08,
     386,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WiredTransferSocket[] = {
    "WiredTransferSocket\0\0"
    "fileTransferStarted(ClassWiredTransfer)\0"
    "fileTransferDone(ClassWiredTransfer)\0"
    "fileTransferError(ClassWiredTransfer)\0"
    "fileTransferStatus(ClassWiredTransfer)\0"
    "fileTransferSocketError(QAbstractSocket::SocketError)\0"
    "cancelTransfer()\0onSocketConnected()\0"
    "onSocketReady()\0onSocketDisconnected()\0"
    "errors\0on_socket_sslErrors(QList<QSslError>)\0"
    "theOffset\0sendNextFileChunk(qint64)\0"
    "onSocketError(QAbstractSocket::SocketError)\0"
};

const QMetaObject WiredTransferSocket::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_WiredTransferSocket,
      qt_meta_data_WiredTransferSocket, 0 }
};

const QMetaObject *WiredTransferSocket::metaObject() const
{
    return &staticMetaObject;
}

void *WiredTransferSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WiredTransferSocket))
	return static_cast<void*>(const_cast< WiredTransferSocket*>(this));
    return QThread::qt_metacast(_clname);
}

int WiredTransferSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileTransferStarted((*reinterpret_cast< const ClassWiredTransfer(*)>(_a[1]))); break;
        case 1: fileTransferDone((*reinterpret_cast< const ClassWiredTransfer(*)>(_a[1]))); break;
        case 2: fileTransferError((*reinterpret_cast< const ClassWiredTransfer(*)>(_a[1]))); break;
        case 3: fileTransferStatus((*reinterpret_cast< const ClassWiredTransfer(*)>(_a[1]))); break;
        case 4: fileTransferSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 5: cancelTransfer(); break;
        case 6: onSocketConnected(); break;
        case 7: onSocketReady(); break;
        case 8: onSocketDisconnected(); break;
        case 9: on_socket_sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 10: sendNextFileChunk((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 11: onSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void WiredTransferSocket::fileTransferStarted(const ClassWiredTransfer _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WiredTransferSocket::fileTransferDone(const ClassWiredTransfer _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WiredTransferSocket::fileTransferError(const ClassWiredTransfer _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WiredTransferSocket::fileTransferStatus(const ClassWiredTransfer _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WiredTransferSocket::fileTransferSocketError(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
