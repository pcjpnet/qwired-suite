#include "QwcTransferSocket.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

const int transferTimerInterval = 250;

QwcTransferSocket::QwcTransferSocket(QObject *parent) :
        QObject(parent)
{
    m_socket = NULL;
    m_transferLimit = 0;
    m_currentTransferSpeed = 0;
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

    qDebug() << "Set file info with size of" << file.size();
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


void QwcTransferSocket::setTransferLimit(qint64 limit)
{ m_transferLimit = limit; }

qint64 QwcTransferSocket::transferLimit() const
{ return m_transferLimit; }


qint64 QwcTransferSocket::currentTransferSpeed() const
{ return m_currentTransferSpeed; }


/*! Delete an old socket, or create a new one.
*/
void QwcTransferSocket::createNewSocket()
{
    qDebug() << this << "Creating new SSL socket for transfer.";
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

    qDebug() << this << "Connecting to remote host at" << m_serverHost << "port" << m_serverPort;
    m_socket->abort();
    m_socket->connectToHostEncrypted(m_serverHost, m_serverPort);

    // Set the read buffer size to limit download speed
    if (transferDirection() == Qwc::TransferDirectionDownload && m_transferLimit > 0) {
        m_socket->setReadBufferSize(m_transferLimit * (float(transferTimerInterval)/1000));
    }
}


/*! Called by the SSL socket when the connection has been established.
*/
void QwcTransferSocket::handleSocketEncrypted()
{
    qDebug() << this << "Transfer socket connected, sending handshake with hash" << m_transferHash;

    // First we send the TRANSFER message with the secret hash. After that data transfer immediately
    // begins.
    m_socket->write(QString("TRANSFER %1\x04").arg(m_transferHash).toAscii());
    m_socket->flush();

    // Start the transfer timer which is responsible for sending chunks of data
    if (transferDirection() == Qwc::TransferDirectionDownload) {
        bool ok;
        if (m_fileInfo.transferredSize() > -1) {
            qDebug() << "Continuing..";
            ok = m_fileReader.open(QIODevice::WriteOnly | QIODevice::Append);
        } else {
            qDebug() << "Creating new file..";
            ok = m_fileReader.open(QIODevice::WriteOnly);
        }

        if (!ok) {
            qDebug() << this << "Unable to open file for writing:" << m_fileReader.errorString();
            stopTransfer();
            emit fileTransferError(this);
            return;
        }

        m_transferTimer.start(transferTimerInterval);
        connect(&m_transferTimer, SIGNAL(timeout()),
                this, SLOT(transmitFileChunk()));


    } else if (transferDirection() == Qwc::TransferDirectionUpload) {
//        qDebug() << "-UPLOAD-";
//        if (!m_fileReader.open(QIODevice::ReadOnly)) {
//            qDebug() << this << "Unable to open file for reading:" << m_fileReader.errorString();
//            stopTransfer();
//            emit fileTransferError(this);
//            return;
//        }
//
//        // Skip to the first byte behind the already uploaded amount of data
//
//        m_fileReader.seek(m_fileInfo.transferredSize());
//
//        // Connect the signal to allow transmission of further data after a chunk is complete.
//        connect(m_socket, SIGNAL(encryptedBytesWritten(qint64)),
//                this, SLOT(transmitFileChunk()));
//
//        // Now start the transfer by calling this manually
//        transmitFileChunk();
    }


    emit fileTransferStatus(this);
}




/*! Transfer a single chunk of data to the server or back.
    \todo Fix current speed calculation.
*/
void QwcTransferSocket::transmitFileChunk()
{    
//    qint64 chunkSize = 64 * 1000;

    if (m_transferDirection == Qwc::TransferDirectionDownload) {

        QByteArray dataBuffer;
        dataBuffer.resize(m_socket->bytesAvailable());
        qint64 readBytes = m_socket->read(dataBuffer.data(), m_socket->bytesAvailable());
        m_fileInfo.setTransferredSize(m_fileInfo.transferredSize() + readBytes);
        m_fileReader.write(dataBuffer);


        // Emit a update signal if we received data, or if 4 seconds have passed
        if (readBytes > 0 || m_transferSpeedTimer.elapsed() > 4000) {
            m_currentTransferSpeed = 1000.0 / float(m_transferSpeedTimer.restart()) * readBytes;
            emit fileTransferStatus(this);
            qDebug() << "Downloaded" << readBytes << "bytes of" << m_fileInfo.size() <<
                    "@" << m_currentTransferSpeed << "/s";
        }

        if (m_socket->state() == QAbstractSocket::UnconnectedState) {
            if (m_fileInfo.transferredSize() == m_fileInfo.size()) {
                qDebug() << this << "Completed download.";
                finishTransfer();
                return;
            }

            qDebug() << this << "Transfer incomplete:" << m_fileInfo.transferredSize()
                    <<"of" << m_fileInfo.size();
            if (m_fileInfo.transferredSize() == 0) {
                qDebug();
            }
            stopTransfer();
        }


    } else if (m_transferDirection == Qwc::TransferDirectionUpload) {
//
//        // Return if not all data hase been written yet
//        if (m_socket->encryptedBytesToWrite() > 0) { return; }
//
//        // Also return when we have transmitted all data, but call finishTransfer() to clean up
//        if (m_fileReader.atEnd()) {
//            finishTransfer();
//            return;
//        }
//
//        // Also return when the connection dropped or another TCP error has occurred
//        if (m_socket->state() == QAbstractSocket::UnconnectedState) {
//            qDebug() << this << "Transfer socket lost connection";
//            stopTransfer();
//            emit fileTransferError(this);
//            return;
//        }
//
//        // Otherwise we simply keep transferring more data
//        QByteArray dataBuffer;
//        dataBuffer = m_fileReader.read(chunkSize * 2);
//        m_socket->write(dataBuffer);
//
//        m_currentTransferSpeed = float(1000/qMax(1,m_transferSpeedTimer.restart())) * chunkSize;
//        m_fileInfo.setTransferredSize(m_fileInfo.transferredSize() + dataBuffer.size());
//
//        emit fileTransferStatus(this);
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






/*! This slot is called when the file transfer has finished successfully. It can be used to do some
    clean-up work and rename the transfer file afterwards.
*/
void QwcTransferSocket::finishTransfer()
{
    qDebug() << this << "Finishing transfer.";
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


