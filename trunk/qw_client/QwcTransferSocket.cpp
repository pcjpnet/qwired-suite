#include "QwcTransferSocket.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

const int transferTimerInterval = 2; // seconds

QwcTransferSocket::QwcTransferSocket(QObject *parent) :
        QObject(parent)
{
    m_socket = NULL;
    m_currentTransferSpeed = 0;
    m_lastTransferSpeedProgress = 0;
    startTimer(transferTimerInterval * 1000);
}


QwcTransferSocket::~QwcTransferSocket()
{ }


void QwcTransferSocket::setFileInfo(const QwFile &file)
{
    QFileInfo localFile(file.localPath());

    QDir dir("/");
    dir.mkpath(localFile.path());

    m_fileReader.setFileName(file.localPath());
    m_fileInfo = file;

//    qDebug() << "Set file info with size of" << file.size();
}


QwFile QwcTransferSocket::fileInfo() const
{ return m_fileInfo; }

void QwcTransferSocket::setTransferHash(const QString &hash)
{ m_transferHash = hash; }

QString QwcTransferSocket::transferHash() const
{ return m_transferHash; }

void QwcTransferSocket::setTransferDirection(Qwc::TransferDirection direction)
{ m_transferDirection = direction; }

Qwc::TransferDirection QwcTransferSocket::transferDirection() const
{ return m_transferDirection; }


/*! Returns the number of bytes which are currently transmitted from or to the server in average
    per second. */
qint64 QwcTransferSocket::currentTransferSpeed() const
{ return m_currentTransferSpeed; }


/*! A simple timer, which allows us to measure the progress and transfer speed.
*/
void QwcTransferSocket::timerEvent(QTimerEvent *)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        m_currentTransferSpeed = -1;
        return;
    }
    qint64 transferredSinceLastTime = m_fileInfo.transferredSize() - m_lastTransferSpeedProgress;
    m_currentTransferSpeed = transferredSinceLastTime / transferTimerInterval;
    m_lastTransferSpeedProgress = m_fileInfo.transferredSize();
}


/*! Delete an old socket, or create a new one.
*/
void QwcTransferSocket::createNewSocket()
{
//    qDebug() << this << "Creating new SSL socket for transfer.";
    if (m_socket) {
        delete m_socket;
    }

    m_socket = new QSslSocket(this);
    m_socket->setProtocol(QSsl::TlsV1);
    m_socket->setPeerVerifyMode(QSslSocket::QueryPeer);
    connect(m_socket, SIGNAL(encrypted()),
            this, SLOT(handleSocketEncrypted()));

}


/*! Begin the transfer by connecting to the remote server and sending the provided hash.
*/
void QwcTransferSocket::beginTransfer()
{
    createNewSocket();
    emit fileTransferStarted(this);
    m_socket->abort();
    m_socket->connectToHostEncrypted(m_serverHost, m_serverPort);
}

/*! Stop the transfer and close the socket connection.
*/
void QwcTransferSocket::stopTransfer()
{
    haltTransfer();
}


/*! Called by the SSL socket when the connection has been established.
*/
void QwcTransferSocket::handleSocketEncrypted()
{
//    qDebug() << this << "Transfer socket connected, sending handshake with hash" << m_transferHash;

    // First we send the TRANSFER message with the secret hash. After that data transfer immediately
    // begins.
    m_socket->write(QString("TRANSFER %1\x04").arg(m_transferHash).toAscii());
    m_socket->flush();

    // Start the transfer timer which is responsible for sending chunks of data
    if (transferDirection() == Qwc::TransferDirectionDownload) {
        QIODevice::OpenMode openMode = m_fileInfo.transferredSize() > -1
                                       ? QIODevice::WriteOnly | QIODevice::Append
                                           : QIODevice::WriteOnly;

        if (!m_fileReader.open(openMode)) {
            m_errorString = tr("Unable to open file for writing: %1")
                            .arg(m_fileReader.errorString());
            haltTransfer();
            emit fileTransferError();
            return;
        }

        m_transferTimer.start(transferTimerInterval);
        connect(&m_transferTimer, SIGNAL(timeout()),
                this, SLOT(transmitFileChunk()));


    } else if (transferDirection() == Qwc::TransferDirectionUpload) {
        if (!m_fileReader.open(QIODevice::ReadOnly)) {
            m_errorString = tr("Unable to open file for reading: %1")
                            .arg(m_fileReader.errorString());
            haltTransfer();
            emit fileTransferError();
            return;
        }

        if (!m_fileReader.seek(m_fileInfo.transferredSize())) {
            m_errorString = tr("Unable to seek to position %1: %2")
                            .arg(m_fileInfo.transferredSize())
                            .arg(m_fileReader.errorString());
            haltTransfer();
            emit fileTransferError();
            return;
        }

        // Connect the signal to allow transmission of further data after a chunk is complete.
        connect(m_socket, SIGNAL(encryptedBytesWritten(qint64)),
                this, SLOT(transmitFileChunk(qint64)));
        transmitFileChunk();
    }

}




/*! Transfer a single chunk of data to the server or back.
    \param[in] amountWritten This argument is used for uploads only and its value is provided by the
                             bytesWritten() signal of the socket. It allows us to update the amount
                             of transferred bytes more smoothly.
*/
void QwcTransferSocket::transmitFileChunk(qint64 amountWritten)
{    
    qint64 chunkSize = 512 * 1000;

    if (m_transferDirection == Qwc::TransferDirectionDownload) {
        QByteArray dataBuffer;
        dataBuffer.resize(m_socket->bytesAvailable());
        qint64 readBytes = m_socket->read(dataBuffer.data(), m_socket->bytesAvailable());
        m_fileInfo.setTransferredSize(m_fileInfo.transferredSize() + readBytes);
        m_fileReader.write(dataBuffer);

        if (m_socket->state() == QAbstractSocket::UnconnectedState) {
            // Check if the file is complete
            if (m_fileInfo.transferredSize() == m_fileInfo.size()) {
//                qDebug() << this << "Completed download.";
                finishTransfer();
                return;
            }

            // Handle a premature end of the file
            m_errorString = tr("Premature loss of transfer connection, local file remains "
                               "incomplete (%1 of %2 bytes transferred)")
                            .arg(m_fileInfo.transferredSize())
                            .arg(m_fileInfo.size());
            haltTransfer();
            emit fileTransferError();
            return;
        }


    } else if (m_transferDirection == Qwc::TransferDirectionUpload) {
//        qDebug() << "Sending data chunk" << amountWritten << "written:" << m_fileInfo.transferredSize();

        // Update the amount of data written
        m_fileInfo.setTransferredSize(m_fileInfo.transferredSize() + amountWritten);

        // Return if not all data has been written yet
        if (m_socket->encryptedBytesToWrite() > 0) { return; }

        // Also return when we have transmitted all data, but call finishTransfer() to clean up
        if (m_fileReader.atEnd()) {
            finishTransfer();
            return;
        }

        // Also return when the connection dropped or another TCP error has occurred
        if (m_socket->state() == QAbstractSocket::UnconnectedState) {
            // Handle a premature end of the file
            m_errorString = tr("Premature loss of transfer connection, remote file remains "
                               "incomplete (%1 of %2 bytes transferred)")
                            .arg(m_fileInfo.transferredSize())
                            .arg(m_fileInfo.size());

            haltTransfer();
            emit fileTransferError();
            return;
        }

        // Otherwise we simply keep transferring more data
        m_socket->write(m_fileReader.read(chunkSize));
    }
}


/*! Stop an active transfer and halt data transmission.
*/
void QwcTransferSocket::haltTransfer()
{
    qWarning() << this << "Halting transfer:" << m_errorString;
    m_transferTimer.stop();
    m_socket->disconnectFromHost();
    m_fileReader.close();
}






/*! This slot is called when the file transfer has finished successfully. It can be used to do some
    clean-up work and rename the transfer file afterwards.
*/
void QwcTransferSocket::finishTransfer()
{
//    qDebug() << this << "Finishing transfer.";
    m_fileReader.close();

//    // Remove the suffix from the finished file
//    if (m_transferDirection == Qwc::TransferDirectionDownload) {
//        if (m_fileReader.fileName().endsWith(".WiredTransfer")) {
//            QString newFileName = m_fileReader.fileName();
//            newFileName.chop(14);
//            m_fileReader.rename(newFileName);
//            transferInfo.file.localAbsolutePath.chop(14);
//        }
//    }


    m_transferTimer.stop();
    m_socket->flush();
    m_socket->disconnectFromHost();
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->waitForDisconnected();
    }

    emit fileTransferFinished();
}



/*! Set the host and port of the remote transfer socket. The port is automatically incremented by
    one.
*/
void QwcTransferSocket::setServer(QString host, int port)
{
    m_serverHost = host;
    m_serverPort = port+1; // n+1 for file transfers
}


