#ifndef QWCFILETRANSFERSOCKET_H
#define QWCFILETRANSFERSOCKET_H


#include "QwcTransferInfo.h"

#include <QTime>
#include <QString>
#include <QSslSocket>
#include <QFile>
#include <QTimer>

const int transferTimerInterval = 250;

class QwcTransferSocket : public QObject
{
    Q_OBJECT

public:
    QwcTransferSocket(QObject *parent=0);
    ~QwcTransferSocket();

    void setServer(QString theServer, int thePort);
    void beginTransfer();

    /*! Information about the current transfer. */
    QwcTransferInfo transferInfo;

public slots:
    void abortTransfer();

private slots:
    void handleSocketEncrypted();
    void transmitFileChunk();
    void finishTransfer();
    
private:
    /*! The real host name of the remote server. */
    QString serverHost;
    /*! The real port of the remote server. */
    int serverPort;
    /*! A timer used to schedule the chunk transfer. */
    QTimer transferTimer;
    /*! A timer used to measure the time between data chunks. */
    QTime currentSpeedTimer;
    /*! The low-level socket for data transmission. */
    QSslSocket *sslSocket;

signals:
    /*! The connection was established and the data transfer begins. */
    void fileTransferStarted(const QwcTransferInfo &transfer);
    /*! The transfer was not successfully completed and the socket can be deleted. */
    void fileTransferError(const QwcTransferInfo &transfer);
    /*! The transfer has made progress and this can be displayed to the user. */
    void fileTransferStatus(const QwcTransferInfo &transfer);
    /*! A connection-level error occoured. The socket can be deleted. */
    void fileTransferSocketError(QAbstractSocket::SocketError error);
    /*! The transfer was successfully completed and the socket can be deleted. */
    void fileTransferDone(const QwcTransferInfo &transfer);

    void fileTransferFileDone(const QwcTransferInfo &transfer);
};

#endif
