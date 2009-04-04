#include "QwSslTcpServer.h"
#include "QwsServerController.h"

QwSslTcpServer::QwSslTcpServer(QObject *parent) : QTcpServer(parent)
{
    initReadBufferSize = 0;
}


QwSslTcpServer::~QwSslTcpServer()
{
}


/**
 * Set the certificate from a PEM fil.
 * @param file The path to the file.
 * @return True if this was successful.
 */
bool QwSslTcpServer::setCertificateFromFile(QString file)
{
    QFile tmpPEMFile(file);
    if(!tmpPEMFile.open(QIODevice::ReadOnly)) {
        QString msg = QString("Fatal: Unable to read certificate file '%1'. %2.")
                .arg(file).arg(tmpPEMFile.errorString());
        qDebug() << msg;
        return false;
    } else {
        QByteArray tmpCertData = tmpPEMFile.readAll();
        pPrivateKey = QSslKey(tmpCertData, QSsl::Rsa);
        pLocalCert = QSslCertificate(tmpCertData);
        if(pPrivateKey.isNull()) {

            qDebug() << QString("Fatal: Unable to read prviate key from file '%1'.").arg(file);
            return false;
        }
        if(pLocalCert.isNull()) {
            qDebug() << QString("Fatal: Unable to read local certificate from file '%1'.").arg(file);
            return false;
        }
        qDebug() << QString("Loaded certificate and private key from '%1'.").arg(file);
    }
    return true;
}


void QwSslTcpServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "[ssltcpserver] New connection:"<<socketDescriptor << "withBufferSize:" << initReadBufferSize;
    QSslSocket *conn = new QSslSocket(this);
    if(conn->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "[ssltcpserver] Accepted connection, setting parameters and initing handshake.";
        conn->setReadBufferSize(initReadBufferSize);
        conn->setProtocol(QSsl::TlsV1);
        conn->setPrivateKey(pPrivateKey);
        conn->setLocalCertificate(pLocalCert);
        pPendingSockets.append(conn);
        emit newSslConnection();
    } else {
        qDebug() << "[ssltcpserver] Failed to set socket descriptor. Dropping.";
        delete conn;
    }
}


/**
 * Returns true if there are any waiting SSL sockets in the queue.
 */
bool QwSslTcpServer::hasPendingSslSocket()
{
    return pPendingSockets.size() > 0;
}


/**
 * Returns the oldest pending SSL socket and removes it from the queue.
 */
QSslSocket* QwSslTcpServer::nextPendingSslSocket()
{
    if (!pPendingSockets.size()) {
        return 0;
    }

    QSslSocket *newSocket = pPendingSockets.takeFirst();
    newSocket->startServerEncryption();
    return newSocket;
}


