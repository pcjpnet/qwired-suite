#ifndef QWSSLTCPSERVER_H
#define QWSSLTCPSERVER_H

#include <QtCore>
#include <QtNetwork>

class QwSslTcpServer : public QTcpServer
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
    qint64 initReadBufferSize;

signals:
    void newSslConnection();

private:
    QList<QSslSocket*> pPendingSockets;



protected:
    void incomingConnection(int socketDescriptor);

};

#endif
