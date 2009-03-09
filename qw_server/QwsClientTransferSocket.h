#ifndef QWSCLIENTTRANSFERSOCKET_H
#define QWSCLIENTTRANSFERSOCKET_H

#include <QThread>
#include <QObject>
#include <QSslSocket>

#include "QwsFile.h"

class QwsClientTransferSocket : public QThread
{

    Q_OBJECT

public:
    QwsClientTransferSocket(QObject *parent);

    void run();
    void setSocket(QSslSocket *socket);

private:
    /*! This is a pointer to the raw socket that connects to the client. */
    QSslSocket *socket;


};

#endif // QWSCLIENTTRANSFERSOCKET_H
