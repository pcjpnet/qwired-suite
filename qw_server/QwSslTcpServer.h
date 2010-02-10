#ifndef QWSSLTCPSERVER_H
#define QWSSLTCPSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslCertificate>

class QSslSocket;

class QwSslTcpServer :
        public QTcpServer
{
    Q_OBJECT


public:
    QwSslTcpServer(QObject *parent=0);
    virtual ~QwSslTcpServer();

    bool hasPendingSslSocket();
    bool setCertificateFromFile(QString file);
    bool setCertificateFromData(const QString data);
    QSslSocket* nextPendingSslSocket();
    QSslKey pPrivateKey;
    QSslCertificate pLocalCert;

    /*! Initialize new sockets with the specified ReadBufferSize to prevent start-peaks. Default is 0. */
    qint64 m_readBufferSize;

signals:
    void newSslConnection();

private:
    QList<QSslSocket*> pPendingSockets;



protected:
    void incomingConnection(int socketDescriptor);

};

#endif
