#include "QwsClientTransferSocket.h"

/*! \class QwsClientTransferSocket
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-02-23

    This class implements the simple TRANSFER-protocol for file transfers to clients.
*/

bool QwsClientTransferSocket::sendFileToClient()
{
    qDebug() << this << "Starting to send file to client.";
}


void QwsClientTransferSocket::transmitFileChunk()
{

    qDebug() << this << "Timer fired.";

    if (transferInfo.type == Qws::TransferTypeDownload) {
        qint64 chunkSize = 8*1024;
        if (speedLimit > 0) {
            chunkSize = speedLimit;
        }
        if (socket->encryptedBytesToWrite() > 0) {
            return;
        }
        if (fileReader.atEnd()) {
            finishTransfer();
        }
        QByteArray dataBuffer;
        dataBuffer = fileReader.read(chunkSize);
        socket->write(dataBuffer);


    } else if (transferInfo.type == Qws::TransferTypeUpload) {
        // The upload is throttled. The buffer size was limited before and the timer is fired
        // every second. All we have to do is to read as much data as possible from the socket's
        // buffer and hand it off.
       // qDebug() << "Reading:" << socket->bytesAvailable() << "Enc=" << socket->encryptedBytesAvailable();
       // socket->readAll();
       // qDebug() << "After: Reading:" << socket->bytesAvailable() << "Enc=" << socket->encryptedBytesAvailable();
        //socket->read(speedLimit);
//        handleSocketReadyRead();
    }

}


/*! This method starts writing or reading data from the remote socket, opens the required file
    and does some other preparation work. */
void QwsClientTransferSocket::beginDataTransmission()
{

    if (transferInfo.type == Qws::TransferTypeDownload) {
        // Download (to the client)

        if (!fileReader.isOpen()) {
            qDebug() << "Warning: Unable to read from file:" << fileReader.fileName() << "-" << fileReader.errorString();
            emit transferError(Qws::TransferSocketErrorFileOpen, transferInfo);
            return;
        } else {
            transmitFileChunk();
            if (speedLimit == 0) {
                // Full speed transfer
                connect(socket, SIGNAL(encryptedBytesWritten(qint64)),
                        this, SLOT(transmitFileChunk()));
            } else {
                // Throttled speed transfer
                connect(&transferTimer, SIGNAL(timeout()), this, SLOT(transmitFileChunk()));
                transferTimer.start(1024);
            }
        }


    } else if (transferInfo.type == Qws::TransferTypeUpload) {
        // Set the read buffer size according to the speed limit.
        socket->setReadBufferSize(speedLimit);
        qDebug() << this << "Preparing data transmission";
        if (speedLimit > 0) {
            // Throttled speed transfer
            // First disconnect the readyRead() signal, since we'll be using the time to read the
            // data from the socket every second.
            disconnect(socket, SIGNAL(readyRead()), this, SLOT(handleSocketReadyRead()));
            connect(&transferTimer, SIGNAL(timeout()), this, SLOT(transmitFileChunk()));
            transferTimer.start(1024);
            qDebug() << "Starting timer";
        }
    }
}


/*! Abort a running transfer (emits a transferError() signal as if the connection was aborted)
*/
void QwsClientTransferSocket::abortTransfer()
{
    qDebug() << this << "Aborting transfer";
    socket->close();
    emit transferError(Qws::TransferSocketErrorNetwork, transferInfo);
}


void QwsClientTransferSocket::handleSocketReadyRead()
{

    if (transferState == Qws::TransferSocketStatusWaitingForHash) {
        // Check if we got enough bytes to form a TRANSFER message, this is really the only message
        // in this subprotocol, so we don't need to bloat of a whole protocol implementation.

        // It should be sufficient to peek the first 100 bytes from the buffer to see if there is
        // a TRANSFER message. If this should not be enough, something bad happend and we should
        // kill the connection anyway.

        QByteArray peekBuffer = socket->peek(100);

        // Now check if we got a EOT terminator after length("TRANSFER x")+1 bytes
        int positionEot = peekBuffer.indexOf('\x04');
        if (positionEot >= 10) {
            if (!peekBuffer.startsWith("TRANSFER ")) {
                // The data does not start with a TRANSFER command, we should give up at this
                // point.
                qDebug() << this << "Expected TRANSFER, but got garbage.";
                emit transferError(Qws::TransferSocketErrorHash, transferInfo);
                return;
            }

            int positionSpace = peekBuffer.indexOf(' ');
            QByteArray providedHash = peekBuffer.mid(positionSpace+1, positionEot-positionSpace-1);
            qDebug() << this << "Received transfer hash:" << providedHash;

            // Now jump over the message in the socket buffer, so that we have the raw data at hand.
            socket->seek(positionEot+1);

            // Emit a signal, so that the session handler can verify the hash.
            if (transferPool->hasTransferWithHash(providedHash)) {
                this->transferInfo = transferPool->takeTransferFromQueueWithHash(providedHash);
                transferState = Qws::TransferSocketStatusTransferring;
                qDebug() << "Found hash with file:" << transferInfo.file.path;
                if (!this->openLocalFile()) {
                    qDebug() << this << "Unable to open local file.";
                    emit transferError(Qws::TransferSocketErrorFileOpen, transferInfo);
                    return;
                }

                beginDataTransmission();
            } else {
                // There is no hash in the pool - we should give up here.
                qDebug() << this << "Unknown hash:" << providedHash;
                emit transferError(Qws::TransferSocketErrorHash, transferInfo);
                return;
            }

        } else {
            // At this point we are supposed to have a complete message, but it's too short to
            // even fit useful information in there.
            qDebug() << this << "Expected TRANSFER, but message was too short.";
            emit transferError(Qws::TransferSocketErrorHash, transferInfo);
            return;
        }

    } else if (transferState == Qws::TransferSocketStatusTransferring) {
        // The transfer is running and we should handle the upload of files somewhere else.

        if (transferInfo.type == Qws::TransferTypeUpload) {
            if (fileReader.isOpen()) {
                fileReader.write(socket->readAll());
                if (fileReader.size() == transferInfo.file.size) {
                    finishTransfer();
                }
            }
        }

    }
}


void QwsClientTransferSocket::handleSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << this << "Socket error:" << socketError;
    emit transferError(Qws::TransferSocketErrorNetwork, transferInfo);
}


bool QwsClientTransferSocket::openLocalFile()
{
    qDebug() << this << "Opening local file:" << transferInfo.file.localAbsolutePath;
    fileReader.setFileName(transferInfo.file.localAbsolutePath);

    if (transferInfo.type == Qws::TransferTypeDownload) {
        // We only need to read the file.
        if (fileReader.open(QIODevice::ReadOnly)) {
            qDebug() << this << "Opened file for reading, offset" << transferInfo.offset;
            if (!fileReader.seek(transferInfo.offset)) {
                qDebug() << this << "Unable to seek to pos" << transferInfo.offset;
                emit transferError(Qws::TransferSocketErrorFileOpen, transferInfo);
                return false;
            }
            return true;
        } else {
            qDebug() << this << "Error: Unable to read from file:" << fileReader.error();
            emit transferError(Qws::TransferSocketErrorFileOpen, transferInfo);
            return false;
        }

    } else if (transferInfo.type == Qws::TransferTypeUpload) {
       if (fileReader.open(QIODevice::Append)) {
            qDebug() << this << "Opened file for appending, offset" << transferInfo.offset;
            return true;
        } else {
            qDebug() << this << "Error: Unable to write to file:" << fileReader.error();
            emit transferError(Qws::TransferSocketErrorFileOpen, transferInfo);
            return false;
        }

    }
}


void QwsClientTransferSocket::beginTransfer()
{
    if (!socket) { return; }
    //openLocalFile();
    transferState = Qws::TransferSocketStatusWaitingForHash;
    qDebug() << this << "Beginning transfer - waiting for hash.";
}


void QwsClientTransferSocket::finishTransfer()
{
    qDebug() << this << "Finishing transfer.";

    if (transferInfo.type == Qws::TransferTypeUpload) {
        // Rename the upload file
        QFile targetFile(transferInfo.file.localAbsolutePath);
        if (targetFile.fileName().endsWith(".WiredTransfer")) {
            QString newFileName = targetFile.fileName();
            targetFile.rename(newFileName.left(newFileName.length()-14));
        }

    }
    //socket->waitForBytesWritten(5000);
    socket->disconnectFromHost();
    transferTimer.stop();
    emit transferDone(transferInfo);
    //this->deleteLater();
}


/*! This function takes over the socket and moves it to this thread. Also it takes QObject
    ownership.
*/
void QwsClientTransferSocket::setSocket(QSslSocket *socket)
{
    if (!socket) { return; }
    this->socket = socket;
    this->socket->setParent(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(handleSocketReadyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
}


void QwsClientTransferSocket::setTransferPool(QwsTransferPool *pool)
{
    qDebug() << this << "Setting transfer pool.";
    this->transferPool = pool;
}


QwsClientTransferSocket::~QwsClientTransferSocket()
{
    qDebug() << this << "Deleting.";
}

QwsClientTransferSocket::QwsClientTransferSocket(QObject *parent=0) : QObject(parent)
{
    qDebug() << this << "Created new transfer socket.";
    transferPool = NULL;
    speedLimit = 0; //1024*1024;
}
