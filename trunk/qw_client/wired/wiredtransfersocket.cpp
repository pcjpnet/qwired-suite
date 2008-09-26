/***************************************************************************
 * QWired - a free Wired client.
 * Copyright (C) 2008 Bastian Bense
 ***************************************************************************
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                         *
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                   *
 ***************************************************************************/


#include "wiredtransfersocket.h"

void WiredTransferSocket::run() {
	pSocket = new QSslSocket;
	pSocket->setProtocol(QSsl::TlsV1);
	pSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
	startTransfer();
}




/// Send the TRANSFER handshake.
void WiredTransferSocket::sendHandshake()
{
	qDebug() << this << "Sending handshake for TRANSFER"<<pTransfer.pHash;;
	QByteArray tmpCmd("TRANSFER ");
	tmpCmd += pTransfer.pHash;
	tmpCmd += 0x04; // EOT
	pSocket->write(tmpCmd);
	pSocket->waitForBytesWritten();

	if(pTransfer.pTransferType == WiredTransfer::TypeUpload)
			uploadData();
	else	downloadData();

	emit fileTransferDone(pTransfer);
	killTransfer();
}


void WiredTransferSocket::killTransfer() {
	if(pSocket && pSocket->state()==QAbstractSocket::ConnectedState ) {
		qDebug() << this << "Disconnecting thread";
		pSocket->close();
	}
	qDebug() << this << "Thread finished.";
//  	this->deleteLater();
}


/// Begin to download the data
void WiredTransferSocket::downloadData()
{
	int timerInterval = 500;
	pTimer.start();
	qDebug() << this << "Starting download of data.";
	pTransfer.pStatus = WiredTransfer::StatusActive;
	while(1) {
		pSocket->waitForReadyRead();
		QByteArray data = pSocket->readAll();

		if(!data.isEmpty()) {
			pFile->write(data);
			pTransfer.pDoneSize += data.size();
		}

		if(pTransfer.pDoneSize == pTransfer.pTotalSize) {
			qDebug() << this << "Transfer completed.";
			pSocket->disconnectFromHost();
			pSocket->waitForDisconnected();

			pSocket->close();
			break;
		}

		if(pTimer.elapsed()>=timerInterval)
			calculateSpeed();

		if(!pSocket->isValid() && pTransfer.pDoneSize!=pTransfer.pTotalSize) {
			emit fileTransferError(pTransfer);
			return;
		}

	}
	pTransfer.pStatus = WiredTransfer::StatusDone;
	qDebug() << this << "End of loop. Closing file.";

	// Close file
	pFile->flush();
	pFile->close();

// 	emit fileTransferDone(pTransfer);
	
 	
	killTransfer();
}


/// Begin to upload the data
void WiredTransferSocket::uploadData()
{
	int timerInterval = 500;
	pTimer.start();
// 	qDebug() << this << "Starting upload of data.";
	pTransfer.pStatus = WiredTransfer::StatusActive;
	while(1) {
		//qDebug() << this << "X: Reading from file";
		
		if(pFile->atEnd()) {
// 			qDebug() << this << "Transfer completed. Wait by bytes written. FileAtEnd:"<<pFile->atEnd()<<"Wrote:"<<pTransfer.pDoneSize<<"of"<<pTransfer.pTotalSize;
			pSocket->disconnectFromHost();
// 			qDebug() << this << "X: Wait for disconnected";
			pSocket->waitForDisconnected();
// 			qDebug() << this << "X: Close()";
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
	pTransfer.pStatus = WiredTransfer::StatusDone;
	pFile->close();
}

WiredTransferSocket::~WiredTransferSocket() {
	qDebug() << this << "TransferSocket: Destroyed thread";
	if(!pFile.isNull()) pFile->deleteLater();
	if(!pSocket.isNull()) pSocket->deleteLater();
}


/// Connect and do some magic stuff.
void WiredTransferSocket::startTransfer()
{
	emit fileTransferStarted(pTransfer);
	if( pTransfer.pTransferType == WiredTransfer::TypeDownload) {
		QString tmpPath = pTransfer.pLocalPath+QString(".WiredTransfer");
		pFile = new QFile(tmpPath);
		if( !pFile->open(QIODevice::Append) ) {
			qDebug() << this << " Unable to open the file:"<<tmpPath<<":"<<pFile->errorString();
			return;
		}

	} else if(pTransfer.pTransferType == WiredTransfer::TypeUpload) {
		pFile = new QFile(pTransfer.pLocalPath);
		if(!pFile->open(QIODevice::ReadOnly) ) {
			qDebug() << this << " Unable to open the file:"<<pTransfer.pLocalPath<<":"<<pFile->errorString();
			return;
		}
		pFile->seek(pTransfer.pOffset);

	}

	if(!pTransfer.pEncryptTransfer)
		pSocket->setCiphers("NULL"); // <-- if we want no encryption ;) I guess morris' mac is just too slow

	// Connect for a start
	qDebug() << this << "TransferSocket: Connecting to"<<pServerHost<<":"<<pServerPort<<"hash"<<pTransfer.pHash<<"offset"<<pTransfer.pOffset;
	pSocket->connectToHostEncrypted(pServerHost, pServerPort);

	if(!pSocket->waitForConnected()) {
		qDebug() << this << "Could not connect:"<<pSocket->error();
		emit fileTransferError(pTransfer);
		return;
	}

	if(!pSocket->waitForEncrypted()) {
		qDebug() << this << "Could not establish an encrypted connection.";
		emit fileTransferError(pTransfer);
		return;
	}

	qDebug() << this << "Connected to remote host encrypted. Handshake follows.";
	sendHandshake();

}


/// Set the server address and port.
void WiredTransferSocket::setServer(QString theServer, int thePort)
{
	pServerHost = theServer;
	pServerPort = thePort+1; // n+1 for file transfers
}


/// Stop the file transfer.
void WiredTransferSocket::cancelTransfer() {
	qDebug() << this << "Stopping transfer"<<pTransfer.pHash;
	if(pSocket) pSocket->close();
	pTransfer.pStatus = WiredTransfer::StatusDone;
	killTransfer();
}


/// Update current speed
void WiredTransferSocket::calculateSpeed()
{
	qlonglong tmpDiff = pTransfer.pDoneSize-pLastDone; // difference between past and now
	int tmpTime = pTimer.restart(); // time difference

	// Calc the average speed
	int tmpCurrentSpeed = (tmpDiff/tmpTime)*1000;
	if(pSpeedList.count()>=5)
		pSpeedList.removeFirst();
	pSpeedList.append(tmpCurrentSpeed);
	QListIterator<int> i(pSpeedList);
	while(i.hasNext()) { pTransfer.pCurrentSpeed += i.next(); }
	pTransfer.pCurrentSpeed = pTransfer.pCurrentSpeed/pSpeedList.count();

	pLastDone = pTransfer.pDoneSize;
	emit fileTransferStatus(pTransfer);
}


