#ifndef QWSCLIENTTRANSFERSOCKET_H
#define QWSCLIENTTRANSFERSOCKET_H

#include <QObject>
#include <QSslSocket>
#include <QFile>
#include <QTimer>

#include "QwsFile.h"
#include "QwsTransferInfo.h"
#include "QwsTransferPool.h"


namespace Qws {
    enum TransferSocketState {
        /*! The connection was established, we are waiting for a hash. */
        TransferSocketStatusWaitingForHash,
        /*! The hash was received and the session manager is verifying it. */
        TransferSocketStatusReceivedHash,
        /*! The has was OK and the session manager initiated the actual transfer. */
        TransferSocketStatusTransferring
    };

    enum TransferSocketError {
        /*! The target file could not be opened for reading or writing. */
        TransferSocketErrorFileOpen,
        /*! The provided hash was not found in the pool or the client sent garbage. */
        TransferSocketErrorHash,
        /*! The network problem occurred, resulting in the connection to be lost. */
        TransferSocketErrorNetwork

    };
}

class QwsClientTransferSocket : public QObject
{

    Q_OBJECT

public:
    QwsClientTransferSocket(QObject *parent);
    ~QwsClientTransferSocket();

    void finishTransfer();
    void abortTransfer();

    void setSocket(QSslSocket *socket);
    void setMaximumTransferSpeed(qint64 bytesPerSecond);
    bool openLocalFile();
    void setTransferPool(QwsTransferPool *pool);
    void setTransferInfo(QwsTransferInfo info);

    Qws::TransferSocketState state() const
    { return this->transferState; }

    const QwsTransferInfo info() const
    { return this->transferInfo; }

    /*! This is a pointer to the relevant server controller. */
    QwsTransferPool *transferPool;
    QFile fileReader;




private slots:
    void handleSocketReadyRead();
    void handleSocketError(QAbstractSocket::SocketError socketError);
    void transmitFileChunk();

private:
    Qws::TransferSocketState transferState;
    /*! The transfer information for this file transfer. */
    QwsTransferInfo transferInfo;
    /*! This is a pointer to the raw socket that connects to the client. */
    QSslSocket *socket;
    /*! The target file we are reading data from. */
    QTimer transferTimer;
    QTime transferSpeedTimer;
    void beginDataTransmission();

signals:
    /*! This is emitted when there is a error with the transfer, such as the file can't be read from
        or a timeout happens, or the client disconnectes suddenly. */
    void transferError(Qws::TransferSocketError error, const QwsTransferInfo transfer);
    /*! This is emitted when the transfer has successfully finished, and all (remaining) data has
        been transmitted. */
    void transferDone(const QwsTransferInfo transfer);

    /*! This is emitted when we receive a transfer hash from the remote server. It should be
        connected to the session handler, which in turn checks the hash for validity and takes
        action. */
    void receivedTransferHash(const QByteArray hash);


};

#endif // QWSCLIENTTRANSFERSOCKET_H
