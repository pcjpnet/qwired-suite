#include "QwcFiletransferSocket.h"

QwcFiletransferSocket::QwcFiletransferSocket(QObject *parent) : QObject(parent)
{
}

void QwcFiletransferSocket::run()
{
    pSocket = new QSslSocket;
    pSocket->setProtocol(QSsl::TlsV1);
    pSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
    startTransfer();
}

/// Begin to download the data
void QwcFiletransferSocket::downloadData()
{
    int timerInterval = 500;
    pTimer.start();

    qDebug() << this << "Starting download of data.";

    if(pTransfer.pTransferType == WiredTransfer::TypeFolderDownload) {
        pTransfer.pFileStatus = WiredTransfer::StatusActive;
    } else {
        pTransfer.pStatus = WiredTransfer::StatusActive;
    }

    while(1) {
        pSocket->waitForReadyRead();
        QByteArray data = pSocket->readAll();

        if(!data.isEmpty()) {
            pFile->write(data);
            pTransfer.pDoneSize += data.size();
        }

        if(pTransfer.pDoneSize==pTransfer.pTotalSize) {
            qDebug() << this << "Transfer completed.";
            pSocket->disconnectFromHost();
            pSocket->waitForDisconnected();
            pSocket->close();

            if(pTransfer.pTransferType==WiredTransfer::TypeDownload
               || pTransfer.pTransferType==WiredTransfer::TypeFolderDownload) {
                QFile tmpFile(pTransfer.pLocalPath+QString(".WiredTransfer"));
                if(tmpFile.exists()) tmpFile.rename(pTransfer.pLocalPath);
            }

            if(pTransfer.pTransferType==WiredTransfer::TypeFolderDownload)
                pTransfer.pFileStatus = WiredTransfer::StatusDone;
            else	pTransfer.pStatus = WiredTransfer::StatusDone;

            break;
        }

        if(pTimer.elapsed()>=timerInterval) calculateSpeed();

        if(!pSocket->isValid()) {
            if(pTransfer.pStatus==WiredTransfer::StatusStopping) {
                return;
            } else if(pTransfer.pDoneSize!=pTransfer.pTotalSize) {
                qDebug() << this << "ERROR:"<<pSocket->error();
                emit fileTransferError(pTransfer);
                return;
            }
        }

    }


    qDebug() << this << "End of loop. Closing file.";

    // Close file
    pFile->flush();
    pFile->close();
    killTransfer();
}


void QwcFiletransferSocket::killTransfer()
{
    if(pSocket && pSocket->state()==QAbstractSocket::ConnectedState ) {
        qDebug() << this << "Disconnecting thread";
        pSocket->close();
    }
    qDebug() << this << "Thread finished.";
}


/// Begin to upload the data
void QwcFiletransferSocket::uploadData()
{
    int timerInterval = 500;
    pTimer.start();

    if(pTransfer.pTransferType==WiredTransfer::TypeFolderUpload)
        pTransfer.pFileStatus = WiredTransfer::StatusActive;
    else	pTransfer.pStatus = WiredTransfer::StatusActive;

    while(1) {
        if(pFile->atEnd()) {
            pSocket->disconnectFromHost();
            pSocket->waitForDisconnected();
            pSocket->close();
            break;
        }
        QByteArray data = pFile->read(1024*100);
        if(!data.isEmpty()) {
            pSocket->write(data);
            pTransfer.pDoneSize += data.size();
        }

        if(pTimer.elapsed()>=timerInterval)
            calculateSpeed();

        if(!pSocket->isValid() && pTransfer.pDoneSize!=pTransfer.pTotalSize) {
            emit fileTransferError(pTransfer);
            return;
        }

        pSocket->waitForBytesWritten();
    }

    if(pTransfer.pTransferType==WiredTransfer::TypeFolderUpload)
        pTransfer.pFileStatus = WiredTransfer::StatusActive;
    else	pTransfer.pStatus = WiredTransfer::StatusActive;

    pFile->close();
}

QwcFiletransferSocket::~QwcFiletransferSocket()
{
    qDebug() << this << "TransferSocket: Destroyed thread";
    if(!pFile.isNull()) pFile->deleteLater();
    if(!pSocket.isNull()) pSocket->deleteLater();
}


/// Connect and do some magic stuff.
void QwcFiletransferSocket::startTransfer()
{
    emit fileTransferStarted(pTransfer);

    if (pTransfer.pTransferType == WiredTransfer::TypeDownload || pTransfer.pTransferType == WiredTransfer::TypeFolderDownload) {
        QString tmpPath = pTransfer.pLocalPath+QString(".WiredTransfer");
        pFile = new QFile(tmpPath);
        if (!pFile->open(QIODevice::Append)) {
            qDebug() << this << "Unable to open the file:"<<tmpPath<<":"<<pFile->errorString();
            return;
        }

    } else if (pTransfer.pTransferType == WiredTransfer::TypeUpload || pTransfer.pTransferType == WiredTransfer::TypeFolderUpload) {
        pFile = new QFile(pTransfer.pLocalPath);
        if(!pFile->open(QIODevice::ReadOnly) ) {
            qDebug() << this << "Unable to open the file:"<<pTransfer.pLocalPath<<":"<<pFile->errorString();
            return;
        }
        pFile->seek(pTransfer.pOffset);
    }

    if (!pTransfer.pEncryptTransfer)
        pSocket->setCiphers("NULL"); // <-- if we want no encryption ;) I guess morris' mac is just too slow

    // Connect for a start
    qDebug() << this << "TransferSocket: Connecting to"<<pServerHost<<":"<<pServerPort<<"hash"<<pTransfer.pHash<<"offset"<<pTransfer.pOffset;
    pSocket->connectToHostEncrypted(pServerHost, pServerPort);

    if(!pSocket->waitForConnected()) {
        qDebug() << this << "Could not connect:"<<pSocket->error();
        emit fileTransferError(pTransfer);
        return;
    }

    qDebug() << "Connected to remote host.";

    if(!pSocket->waitForEncrypted()) {
        qDebug() << this << "Could not establish an encrypted connection.";
        emit fileTransferError(pTransfer);
        return;
    }

    qDebug() << "Encrypted.";

    qDebug() << this << "Connected to remote host encrypted. Handshake follows.";
    sendHandshake();

}


/// Send the TRANSFER handshake.
void QwcFiletransferSocket::sendHandshake()
{
    qDebug() << this << "Sending handshake for TRANSFER"<<pTransfer.pHash;;
    QByteArray tmpCmd("TRANSFER ");
    tmpCmd += pTransfer.pHash + char(0x04);
    pSocket->write(tmpCmd);
    pSocket->waitForBytesWritten();

    if(pTransfer.pTransferType==WiredTransfer::TypeUpload || pTransfer.pTransferType==WiredTransfer::TypeFolderUpload)
        uploadData();
    else	downloadData();

    if(pTransfer.pTransferType==WiredTransfer::TypeFolderDownload || pTransfer.pTransferType==WiredTransfer::TypeFolderUpload)
        emit fileTransferFileDone(pTransfer);
    else	emit fileTransferDone(pTransfer);

    killTransfer();
}


/// Set the server address and port.
void QwcFiletransferSocket::setServer(QString theServer, int thePort)
{
    pServerHost = theServer;
    pServerPort = thePort+1; // n+1 for file transfers
}


/// Stop the file transfer.
void QwcFiletransferSocket::cancelTransfer()
{
    qDebug() << this << "Stopping transfer"<<pTransfer.pHash;
    if(pSocket) pSocket->close();
    pTransfer.pStatus = WiredTransfer::StatusStopping;
    killTransfer();
}


/// Update current speed
void QwcFiletransferSocket::calculateSpeed()
{
    qlonglong tmpDiff = pTransfer.pDoneSize-pLastDone; // difference between past and now
    int tmpTime = pTimer.restart(); // time difference

    // Calc the average speed
    int tmpCurrentSpeed = (tmpDiff/tmpTime)*1000;
    if (pSpeedList.count() >= 5) {
        pSpeedList.removeFirst();
    }
    pSpeedList.append(tmpCurrentSpeed);
    QListIterator<int> i(pSpeedList);
    while(i.hasNext()) { pTransfer.pCurrentSpeed += i.next(); }
    pTransfer.pCurrentSpeed = pTransfer.pCurrentSpeed/pSpeedList.count();

    pLastDone = pTransfer.pDoneSize;
    emit fileTransferStatus(pTransfer);
}




