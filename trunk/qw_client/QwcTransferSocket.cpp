#include "QwcTransferSocket.h"
#include <QSettings>

QwcTransferSocket::QwcTransferSocket(QObject *parent) :
        QObject(parent)
{
    sslSocket = NULL;
}


QwcTransferSocket::~QwcTransferSocket()
{
    qDebug() << this << "TransferSocket: Destroyed transfer socket";
}


/*! Delete an old socket, or create a new one.
*/
void QwcTransferSocket::createNewSocket()
{
    qDebug() << this << "Creating new SSL socket for transfer.";
    if (sslSocket) {
        sslSocket->deleteLater();
    }

    sslSocket = new QSslSocket(this);
    sslSocket->setProtocol(QSsl::TlsV1);
    sslSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
    connect(sslSocket, SIGNAL(encrypted()),
            this, SLOT(handleSocketEncrypted()));
    connect(&transferTimer, SIGNAL(timeout()),
            this, SLOT(transmitFileChunk()));
}



/*! Begin the transfer by connecting to the remote server and sending the provided hash.
*/
void QwcTransferSocket::beginTransfer()
{
    createNewSocket();

    emit fileTransferStarted(this);

    qDebug() << this << "Connecting to remote host at" << serverHost << "port" << serverPort;
    sslSocket->abort();
    sslSocket->connectToHostEncrypted(serverHost, serverPort);

    // Set the read buffer size to limit download speed
    if ((transferInfo.type == Qw::TransferTypeDownload || transferInfo.type == Qw::TransferTypeFolderDownload)
        && transferInfo.transferSpeedLimit > 0) {
        sslSocket->setReadBufferSize(transferInfo.transferSpeedLimit * (float(transferTimerInterval)/1000));
    }

}


/*! Called by the SSL socket when the connection has been established.
*/
void QwcTransferSocket::handleSocketEncrypted()
{
    qDebug() << this << "Transfer socket connected, sending handshake with hash" << transferInfo.hash;

    // First we send the TRANSFER message with the secret hash. After that data transfer immediately
    // begins.
    sslSocket->write(QString("TRANSFER %1\x04").arg(transferInfo.hash).toAscii());
    sslSocket->flush();

    // Start the transfer timer which is responsible for sending chunks of data
    if (transferInfo.type == Qw::TransferTypeDownload || transferInfo.type == Qw::TransferTypeFolderDownload) {
        fileReader.setFileName(transferInfo.file.localAbsolutePath);

        bool ok;
        if (transferInfo.bytesTransferred) {
            ok = fileReader.open(QIODevice::WriteOnly | QIODevice::Append);
        } else {
            ok = fileReader.open(QIODevice::WriteOnly);
        }

        if (!ok) {
            qDebug() << this << "Unable to open file for writing:" << fileReader.errorString();
            stopTransfer();
            emit fileTransferError(this);
            return;
        }

        transferTimer.start(transferTimerInterval);

    } else if (transferInfo.type == Qw::TransferTypeUpload
               || transferInfo.type == Qw::TransferTypeFolderUpload)
    {
        fileReader.setFileName(transferInfo.file.localAbsolutePath);
        if (!fileReader.open(QIODevice::ReadOnly)) {
            qDebug() << this << "Unable to open file for reading:" << fileReader.errorString();
            stopTransfer();
            emit fileTransferError(this);
            return;
        }

        // Skip to the first byte behind the already uploaded amount of data
        fileReader.seek(transferInfo.bytesTransferred);
        // Connect the signal to allow transmission of further data after a chunk is complete.
        connect(sslSocket, SIGNAL(encryptedBytesWritten(qint64)),
                this, SLOT(transmitFileChunk()));
        // Now start the transfer by calling this manually
        transmitFileChunk();
    }

    transferInfo.state = Qw::TransferInfoStateActive;

    emit fileTransferStatus(this);
}


/*! Transfer a single chunk of data to the server or back.
    \todo Fix current speed calculation.
*/
void QwcTransferSocket::transmitFileChunk()
{    
    quint64 chunkSize = 32*1024;


    if (transferInfo.type == Qw::TransferTypeDownload
        || transferInfo.type == Qw::TransferTypeFolderDownload) {


        QByteArray dataBuffer;
        dataBuffer.resize(sslSocket->bytesAvailable());
        int readBytes = sslSocket->read(dataBuffer.data(), sslSocket->bytesAvailable());
        transferInfo.bytesTransferred += readBytes;

        fileReader.write(dataBuffer);

        // Emit a update signal if we received data, or if 4 seconds have passed
        if (readBytes > 0 || currentSpeedTimer.elapsed() > 4000) {
            transferInfo.currentTransferSpeed = 1000.0/float(currentSpeedTimer.restart()) * readBytes;
            emit fileTransferStatus(this);
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
            stopTransfer();
        }


    } else if (transferInfo.type == Qw::TransferTypeUpload
               || transferInfo.type == Qw::TransferTypeFolderUpload)
    {

        // Return if not all data hase been written yet
        if (sslSocket->encryptedBytesToWrite() > 0) {
//            qDebug() << this << "Returning because encryptedBytesToWrite() > 0";
            return;
        }

        // Also return when we have transmitted all data, but call finishTransfer() to clean up
        if (fileReader.atEnd()) {
//            qDebug() << this << "fileReader.atEnd() toWrite:" << sslSocket->bytesToWrite() << "encToWrite:" << sslSocket->encryptedBytesToWrite();
            finishTransfer();
            return;
        }

        // Also return when the connection dropped or another TCP error has occurred
        if (sslSocket->state() == QAbstractSocket::UnconnectedState) {
            qDebug() << this << "Transfer socket lost connection";
            stopTransfer();
            emit fileTransferError(this);
            return;
        }

        // Otherwise we simply keep transferring more data
        QByteArray dataBuffer;
        dataBuffer = fileReader.read(chunkSize * 2);

//        qDebug() << "Transferring" << chunkSize << "(" << dataBuffer.size() << ")" << "from" << fileReader.pos();

        sslSocket->write(dataBuffer);

        transferInfo.currentTransferSpeed = float(1000/qMax(1,currentSpeedTimer.restart())) * chunkSize;
        transferInfo.bytesTransferred += dataBuffer.size();

//        qDebug() << "Transferred" << transferInfo.bytesTransferred << "of" << fileReader.size();

        emit fileTransferStatus(this);
    }
}


/*! Stop an active transfer and halt data transmission.
*/
void QwcTransferSocket::stopTransfer()
{
    qDebug() << this << "Halting transfer.";
    transferTimer.stop();
    sslSocket->close();
//    emit fileTransferError(this);
}


/*! Prepare (reset) the transfer socket for the transmission of the next file. This is only relevant
    for folder transfers. It basically resets the socket state and does some clean-up work.
    \returns true if there were more files to be transferred, or false if there are no further files
    to be send or received.
*/
bool QwcTransferSocket::prepareNextFile()
{
    sslSocket->close();
    fileReader.close();
    if (transferInfo.recursiveFiles.isEmpty()) {
        // No more files remaining
        return false;
    }
    transferInfo.applyNextFile();
    return true;
}



/*! This slot is called when the file transfer has finished successfully. It can be used to do some
    clean-up work and rename the transfer file afterwards.
*/
void QwcTransferSocket::finishTransfer()
{
    qDebug() << this << "Finishing transfer.";
    fileReader.close();

    // Remove the suffix from the finished file
    if (transferInfo.type == Qw::TransferTypeDownload
        || transferInfo.type == Qw::TransferTypeFolderDownload) {
        if (fileReader.fileName().endsWith(".WiredTransfer")) {
            QString newFileName = fileReader.fileName();
            newFileName.chop(14);
            fileReader.rename(newFileName);
            transferInfo.file.localAbsolutePath.chop(14);
        }
    }

    sslSocket->flush();
    sslSocket->disconnectFromHost();
    sslSocket->waitForDisconnected();
    transferTimer.stop();
    transferInfo.state = Qw::TransferInfoStateNone;
    emit fileTransferDone(this);
}



/*! Set the host and port of the remote transfer socket. The port is automatically incremented by
    one.
*/
void QwcTransferSocket::setServer(QString host, int port)
{
    serverHost = host;
    serverPort = port+1; // n+1 for file transfers
}


