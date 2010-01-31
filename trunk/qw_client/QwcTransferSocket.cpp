#include "QwcTransferSocket.h"
#include <QSettings>

QwcTransferSocket::QwcTransferSocket(QObject *parent) :
        QObject(parent)
{
    m_socket = NULL;
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
    if (m_socket) {
        m_socket->deleteLater();
    }

    m_socket = new QSslSocket(this);
    m_socket->setProtocol(QSsl::TlsV1);
    m_socket->setPeerVerifyMode(QSslSocket::QueryPeer);
    connect(m_socket, SIGNAL(encrypted()),
            this, SLOT(handleSocketEncrypted()));
    connect(&m_transferTimer, SIGNAL(timeout()),
            this, SLOT(transmitFileChunk()));
}



/*! Begin the transfer by connecting to the remote server and sending the provided hash.
*/
void QwcTransferSocket::beginTransfer()
{
    createNewSocket();

    emit fileTransferStarted(this);

    qDebug() << this << "Connecting to remote host at" << m_serverHost << "port" << m_serverPort;
    m_socket->abort();
    m_socket->connectToHostEncrypted(m_serverHost, m_serverPort);

    // Set the read buffer size to limit download speed
    if ((transferInfo.type == Qw::TransferTypeDownload || transferInfo.type == Qw::TransferTypeFolderDownload)
        && transferInfo.transferSpeedLimit > 0) {
        m_socket->setReadBufferSize(transferInfo.transferSpeedLimit * (float(transferTimerInterval)/1000));
    }

}


/*! Called by the SSL socket when the connection has been established.
*/
void QwcTransferSocket::handleSocketEncrypted()
{
    qDebug() << this << "Transfer socket connected, sending handshake with hash" << transferInfo.hash;

    // First we send the TRANSFER message with the secret hash. After that data transfer immediately
    // begins.
    m_socket->write(QString("TRANSFER %1\x04").arg(transferInfo.hash).toAscii());
    m_socket->flush();

    // Start the transfer timer which is responsible for sending chunks of data
    if (transferInfo.type == Qw::TransferTypeDownload || transferInfo.type == Qw::TransferTypeFolderDownload) {
        m_fileReader.setFileName(transferInfo.file.localAbsolutePath);

        bool ok;
        if (transferInfo.bytesTransferred) {
            ok = m_fileReader.open(QIODevice::WriteOnly | QIODevice::Append);
        } else {
            ok = m_fileReader.open(QIODevice::WriteOnly);
        }

        if (!ok) {
            qDebug() << this << "Unable to open file for writing:" << m_fileReader.errorString();
            stopTransfer();
            emit fileTransferError(this);
            return;
        }

        m_transferTimer.start(transferTimerInterval);

    } else if (transferInfo.type == Qw::TransferTypeUpload
               || transferInfo.type == Qw::TransferTypeFolderUpload)
    {
        m_fileReader.setFileName(transferInfo.file.localAbsolutePath);
        if (!m_fileReader.open(QIODevice::ReadOnly)) {
            qDebug() << this << "Unable to open file for reading:" << m_fileReader.errorString();
            stopTransfer();
            emit fileTransferError(this);
            return;
        }

        // Skip to the first byte behind the already uploaded amount of data
        m_fileReader.seek(transferInfo.bytesTransferred);
        // Connect the signal to allow transmission of further data after a chunk is complete.
        connect(m_socket, SIGNAL(encryptedBytesWritten(qint64)),
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
        dataBuffer.resize(m_socket->bytesAvailable());
        int readBytes = m_socket->read(dataBuffer.data(), m_socket->bytesAvailable());
        transferInfo.bytesTransferred += readBytes;

        m_fileReader.write(dataBuffer);

        // Emit a update signal if we received data, or if 4 seconds have passed
        if (readBytes > 0 || m_transferSpeedTimer.elapsed() > 4000) {
            transferInfo.currentTransferSpeed = 1000.0/float(m_transferSpeedTimer.restart()) * readBytes;
            emit fileTransferStatus(this);
            qDebug() << "Downloaded" << readBytes << "bytes." << transferInfo.bytesTransferred
                    << "of" << transferInfo.file.size() << "at" << transferInfo.currentTransferSpeed;
        }

        if (m_socket->state() == QAbstractSocket::UnconnectedState) {
            qDebug() << this << "Lost transfer socket connection.";
            if (transferInfo.bytesTransferred == transferInfo.file.size()) {
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
        if (m_socket->encryptedBytesToWrite() > 0) {
//            qDebug() << this << "Returning because encryptedBytesToWrite() > 0";
            return;
        }

        // Also return when we have transmitted all data, but call finishTransfer() to clean up
        if (m_fileReader.atEnd()) {
//            qDebug() << this << "fileReader.atEnd() toWrite:" << sslSocket->bytesToWrite() << "encToWrite:" << sslSocket->encryptedBytesToWrite();
            finishTransfer();
            return;
        }

        // Also return when the connection dropped or another TCP error has occurred
        if (m_socket->state() == QAbstractSocket::UnconnectedState) {
            qDebug() << this << "Transfer socket lost connection";
            stopTransfer();
            emit fileTransferError(this);
            return;
        }

        // Otherwise we simply keep transferring more data
        QByteArray dataBuffer;
        dataBuffer = m_fileReader.read(chunkSize * 2);

//        qDebug() << "Transferring" << chunkSize << "(" << dataBuffer.size() << ")" << "from" << fileReader.pos();

        m_socket->write(dataBuffer);

        transferInfo.currentTransferSpeed = float(1000/qMax(1,m_transferSpeedTimer.restart())) * chunkSize;
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
    m_transferTimer.stop();
    m_socket->close();
//    emit fileTransferError(this);
}


/*! Prepare (reset) the transfer socket for the transmission of the next file. This is only relevant
    for folder transfers. It basically resets the socket state and does some clean-up work.
    \returns true if there were more files to be transferred, or false if there are no further files
    to be send or received.
*/
bool QwcTransferSocket::prepareNextFile()
{
    m_socket->close();
    m_fileReader.close();
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
    m_fileReader.close();

    // Remove the suffix from the finished file
    if (transferInfo.type == Qw::TransferTypeDownload
        || transferInfo.type == Qw::TransferTypeFolderDownload) {
        if (m_fileReader.fileName().endsWith(".WiredTransfer")) {
            QString newFileName = m_fileReader.fileName();
            newFileName.chop(14);
            m_fileReader.rename(newFileName);
            transferInfo.file.localAbsolutePath.chop(14);
        }
    }

    m_socket->flush();
    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();
    m_transferTimer.stop();
    transferInfo.state = Qw::TransferInfoStateNone;
    emit fileTransferDone(this);
}



/*! Set the host and port of the remote transfer socket. The port is automatically incremented by
    one.
*/
void QwcTransferSocket::setServer(QString host, int port)
{
    m_serverHost = host;
    m_serverPort = port+1; // n+1 for file transfers
}


