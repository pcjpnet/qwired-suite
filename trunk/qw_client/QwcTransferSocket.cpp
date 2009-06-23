#include "QwcTransferSocket.h"
#include <QSettings>

QwcTransferSocket::QwcTransferSocket(QObject *parent) : QObject(parent)
{
    sslSocket = new QSslSocket(this);
    sslSocket->setProtocol(QSsl::TlsV1);
    sslSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
    connect(sslSocket, SIGNAL(encrypted()),
            this, SLOT(handleSocketEncrypted()));

    connect(&transferTimer, SIGNAL(timeout()),
            this, SLOT(transmitFileChunk()));

}


QwcTransferSocket::~QwcTransferSocket()
{
    qDebug() << this << "TransferSocket: Destroyed transfer socket";
}


/*! Begin the transfer by connecting to the remote server and sending the provided hash.
*/
void QwcTransferSocket::beginTransfer()
{
    emit fileTransferStarted(transferInfo);

    qDebug() << this << "Connecting to remote host at" << serverHost << "port" << serverPort;
    sslSocket->connectToHostEncrypted(serverHost, serverPort);

    // Set the read buffer size to limit download speed
    if (transferInfo.type == Qw::TransferTypeDownload && transferInfo.transferSpeedLimit > 0) {
        sslSocket->setReadBufferSize(transferInfo.transferSpeedLimit * (float(transferTimerInterval)/1000));
    }

}


/*! Called by the SSL socket when the connection has been established.
*/
void QwcTransferSocket::handleSocketEncrypted()
{
    qDebug() << this << "Transfer socket connected, sending handshake with hash" << transferInfo.hash;
    sslSocket->write(QString("TRANSFER %1\x04").arg(transferInfo.hash).toUtf8());

    // Start the transfer timer which is responsible for sending chunks of data
    if (transferInfo.type == Qw::TransferTypeDownload) {
        fileReader.setFileName(transferInfo.file.localAbsolutePath);
        if (!fileReader.open(QIODevice::WriteOnly)) {
            qDebug() << this << "Unable to open file for writing.";
            abortTransfer();
            return;
        }
        transferTimer.start(transferTimerInterval);

    } else if (transferInfo.type == Qw::TransferTypeUpload) {
        fileReader.setFileName(transferInfo.file.localAbsolutePath);
        if (!fileReader.open(QIODevice::ReadOnly)) {
            qDebug() << this << "Unable to open file for reading.";
            abortTransfer();
            return;
        }
        fileReader.seek(transferInfo.bytesTransferred);
        connect(sslSocket, SIGNAL(encryptedBytesWritten(qint64)),
                this, SLOT(transmitFileChunk()));
        transmitFileChunk();
    }

    transferInfo.state = Qw::TransferInfoStateActive;

    emit fileTransferStatus(transferInfo);
}


/*! Transfer a single chunk of data to the server or back.
    \todo Fix current speed calculation.
*/
void QwcTransferSocket::transmitFileChunk()
{
    quint64 chunkSize = 32*1024;

    if (transferInfo.type == Qw::TransferTypeDownload) {


        QByteArray dataBuffer;
        dataBuffer.resize(sslSocket->bytesAvailable());
        int readBytes = sslSocket->read(dataBuffer.data(), sslSocket->bytesAvailable());
        transferInfo.bytesTransferred += readBytes;

        fileReader.write(dataBuffer);

        // Emit a update signal if we received data, or if 4 seconds have passed
        if (readBytes > 0 || currentSpeedTimer.elapsed() > 4000) {
            transferInfo.currentTransferSpeed = 1000.0/float(currentSpeedTimer.restart()) * readBytes;
            emit fileTransferStatus(transferInfo);
            qDebug() << "Downloaded" << readBytes << "bytes." << transferInfo.bytesTransferred
                    << "of" << transferInfo.file.size << "at" << transferInfo.currentTransferSpeed;
        }

        if (sslSocket->state() == QAbstractSocket::UnconnectedState) {
            qDebug() << this << "Lost transfer socket connection.";
            if (transferInfo.bytesTransferred == transferInfo.file.size) {
                qDebug() << this << "Completed download.";
                finishTransfer();
                return;
            }

            qDebug() << this << "Transfer incomplete.";
            abortTransfer();
        }


    } else if (transferInfo.type == Qw::TransferTypeUpload) {
        if (sslSocket->encryptedBytesToWrite() > 0) {
            qDebug() << this << "Returning because encryptedBytesToWrite() > 0";
            return;
        }

        if (fileReader.atEnd()) {
            finishTransfer();
            return;
        }

        if (sslSocket->state() == QAbstractSocket::UnconnectedState) {
            qDebug() << this << "Transfer socket lost connection";
            abortTransfer();
            return;
        }

        QByteArray dataBuffer;
        dataBuffer = fileReader.read(chunkSize);
        sslSocket->write(dataBuffer);

        transferInfo.currentTransferSpeed = float(1000/qMax(1,currentSpeedTimer.restart())) * chunkSize;
        transferInfo.bytesTransferred += dataBuffer.size();

        emit fileTransferStatus(transferInfo);

    }
}


void QwcTransferSocket::abortTransfer()
{
    qDebug() << this << "Aborting transfer.";
    transferTimer.stop();
    sslSocket->close();
    emit fileTransferError(transferInfo);
}


void QwcTransferSocket::finishTransfer()
{
    qDebug() << this << "Finishing transfer.";
    fileReader.close();

    sslSocket->disconnectFromHost();
    transferTimer.stop();
    emit fileTransferDone(transferInfo);
}



/*! Set the host and port of the remote transfer socket. The port is automatically incremented by
    one.
*/
void QwcTransferSocket::setServer(QString host, int port)
{
    serverHost = host;
    serverPort = port+1; // n+1 for file transfers
}

