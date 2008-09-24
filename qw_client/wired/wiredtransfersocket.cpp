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
	pTransferLimit = 0; /* do not use, not done yet -basti */
	qDebug() << this << "Start transfer thread";
	pSendingFile = false;
	pSocket = new QSslSocket;
	pSocket->setProtocol(QSsl::TlsV1);
	pSocket->setReadBufferSize(pTransferLimit*2);

	pSpeedList << 0 << 0 << 0 << 0 << 0; // some fake data for the average calculation

	connect( pSocket, SIGNAL(sslErrors(const QList<QSslError> &)),
			 this, SLOT(on_socket_sslErrors(const QList<QSslError> &)), Qt::DirectConnection);
	connect( pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			 this, SLOT(onSocketError(QAbstractSocket::SocketError)), Qt::DirectConnection );
	
	connect( pSocket, SIGNAL(encrypted()), this, SLOT(onSocketConnected()) );
	connect( pSocket, SIGNAL(readyRead()), this, SLOT(onSocketReady()) );
	connect( pSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()) );
	connect( pSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendNextFileChunk(qint64)) );

	connect( this, SIGNAL(finished()), this, SLOT(onTheadFinished()) );
	
	startTransfer();
	exec();
}



// Called by the socket and indicates the an SSL error has occoured.
void WiredTransferSocket::on_socket_sslErrors(const QList<QSslError> & errors) {
	qDebug() << this << "WiredSocket.on_socket_sslErrors(): "<<errors;
	pSocket->ignoreSslErrors();
}


WiredTransferSocket::~WiredTransferSocket() {
	qDebug() << this << "TransferSocket: Destroyed thread";
	if(!pFile.isNull()) pFile->deleteLater();
	if(!pSocket.isNull()) pSocket->deleteLater();
}


void WiredTransferSocket::startTransfer()
{
	emit fileTransferStarted(pTransfer);
	if( pTransfer.pTransferType == WiredTransfer::TypeDownload) {
		QString tmpPath = pTransfer.pLocalPath+QString(".WiredTransfer");
		pFile = new QFile(tmpPath);
		if( !pFile->open(QIODevice::Append) ) {
			qDebug() << this << " Unable to open the file:"<<tmpPath<<":"<<pFile->errorString();
			quit();
			return;
		}
		
	} else if(pTransfer.pTransferType == WiredTransfer::TypeUpload) {
		pFile = new QFile(pTransfer.pLocalPath);
		if(!pFile->open(QIODevice::ReadOnly) ) {
			qDebug() << this << " Unable to open the file:"<<pTransfer.pLocalPath<<":"<<pFile->errorString();
			quit();
			return;
		}
		pFile->seek(pTransfer.pOffset);
		
	}

	if(!pTransfer.pEncryptTransfer)
		pSocket->setCiphers("NULL"); // <-- if we want no encryption ;) I guess morris' mac is just too slow

	pSocket->connectToHostEncrypted(pServerHost, pServerPort);
	qDebug() << this << "TransferSocket: Connecting to"<<pServerHost<<":"<<pServerPort<<"hash"<<pTransfer.pHash<<"offset"<<pTransfer.pOffset;
}


// Set the server address and port.
void WiredTransferSocket::setServer(QString theServer, int thePort)
{
	pServerHost = theServer;
	pServerPort = thePort+1; // n+1 for file transfers
}


void WiredTransferSocket::onSocketConnected()
{
	qDebug() << this << "Connected, sending handshake for"<<pTransfer.pHash;
	QByteArray tmpCmd("TRANSFER ");
	tmpCmd += pTransfer.pHash;
	tmpCmd += 0x04; // EOT
	pSocket->write(tmpCmd);

	// Start traffic timer
	pTimer.start();
	pTimerID = startTimer(650);

	// Let others know we are on it
 	//emit fileTransferStarted(pTransfer);
	
	if( pTransfer.pTransferType == WiredTransfer::TypeUpload ) {
		pSendingFile = true;
		sendNextFileChunk(0);
	}
}


// Some data is available in the socket's buffer.
void WiredTransferSocket::onSocketReady() {

	if(pTransferLimit) {
		while(pSocket->bytesAvailable()) {
			QByteArray tmpData = pSocket->read(pTransferLimit);
			if( pTransfer.pTransferType == WiredTransfer::TypeDownload ) {
				pFile->write(tmpData);
				pTransfer.pDoneSize += tmpData.size();
			}
			msleep(1000);
		}
	} else {
		// Normal, full speed
		QByteArray tmpData = pSocket->readAll();
		if( pTransfer.pTransferType == WiredTransfer::TypeDownload ) {
			pFile->write(tmpData);
			pTransfer.pDoneSize += tmpData.size();
		}
	}
	
 		
	
}


void WiredTransferSocket::onSocketDisconnected() {

	qDebug() << this << "Connection closed:"<<pTransfer.pDoneSize<<" of "<<pTransfer.pTotalSize;
	pFile->flush();
	pFile->close();
	
	if( pTransfer.pDoneSize==pTransfer.pTotalSize ) {
		// File complete
		qDebug() << this << "TRANSFER COMPLETED!";
		if(pTransfer.pTransferType == WiredTransfer::TypeDownload) {
			// Rename the file on download, if needed.
			QString tmpPath = pTransfer.pLocalPath+QString(".WiredTransfer");
			QFile tmpFile(tmpPath);
			if(tmpFile.exists() && tmpFile.size()==pTransfer.pTotalSize) {
				qDebug() << this << " Renaming the file";
				tmpFile.rename(pTransfer.pLocalPath);
			}
		}
		pTransfer.pStatus = 3;
		//emit fileTransferDone(pTransfer);
		killTransfer();
		
	} else {
		// Not complete, possibly an error
		emit fileTransferError(pTransfer);
		qDebug() << this << "Not completed, Reason:"<<pSocket->errorString();
		killTransfer();
	}
	
	killTimer(pTimerID);
	//killTransfer();
	//quit();
	
}


// Stop the file transfer.
void WiredTransferSocket::cancelTransfer() {
	qDebug() << this << "Stopping transfer"<<pTransfer.pHash;
	pSocket->disconnectFromHost();
	killTimer(pTimerID);
	pTransfer.pStatus = 3;
// 	emit fileTransferDone(pTransfer);
	killTransfer();
}


// Timer event, mainly for launching the status event
void WiredTransferSocket::timerEvent(QTimerEvent *event) {
	Q_UNUSED(event)
			
	// Update current speed
	qlonglong tmpDiff = pTransfer.pDoneSize-pLastDone; // difference between past and now
	int tmpTime = pTimer.restart(); // time difference

	// Calc the average speed
	int tmpCurrentSpeed = (tmpDiff/tmpTime)*1000;
	pSpeedList.removeFirst();
	pSpeedList.append(tmpCurrentSpeed);
	QListIterator<int> i(pSpeedList);
	while(i.hasNext()) { pTransfer.pCurrentSpeed += i.next(); }
	pTransfer.pCurrentSpeed = pTransfer.pCurrentSpeed/pSpeedList.count();

	
	pLastDone = pTransfer.pDoneSize;
	emit fileTransferStatus(pTransfer);
	//qDebug() << "Received"<<pTransfer.pDoneSize;


}


/// Upload the next chunk to the server.
void WiredTransferSocket::sendNextFileChunk(qint64 theOffset)
{
	//qDebug() << this << "Send next file chunk" << theOffset;
	Q_UNUSED(theOffset)
	const int tmpChunkSize = 1024*1024;
	
	if( pSendingFile && pTransfer.pTransferType == WiredTransfer::TypeUpload ) {
		pTransfer.pDoneSize = pFile->pos();
		if(!pFile->atEnd()) {
			
			QByteArray tmpData = pFile->read(tmpChunkSize);
			pSocket->write(tmpData);
			//qDebug() << this << "Wrote"<<theOffset<<"bytes, at position"<<pFile->pos()<<"of"<<pTransfer.pTotalSize;

			if(pFile->atEnd()) {
				qDebug() << this << "Transfer complete";
				pTransfer.pStatus = 3;
				killTransfer();
			}
		}
	}
}


void WiredTransferSocket::killTransfer() {
	if(pSocket && pSocket->state()==QAbstractSocket::ConnectedState ) {
		qDebug() << this << "Disconnecting thread";
		pSocket->close();
	}
	
	qDebug() << this << "TransferSocket: Terminating event loop of thread";
	killTimer(pTimerID);
	exit();
	
	//wait();
	//this->deleteLater();
}


void WiredTransferSocket::onSocketError(QAbstractSocket::SocketError error) {
	if(error == QAbstractSocket::ConnectionRefusedError) {
		qDebug() << this << "--> Error was a"<<error;
		emit fileTransferSocketError(error);
	//killTransfer();
	} else if(error == QAbstractSocket::RemoteHostClosedError) {
		qDebug() << this << "Remote host closed the connection.";
	} else {
		qDebug() << this << "(SocketError) State:"<<pSocket->state()<<" Reason:" << error;
	}
}

void WiredTransferSocket::onTheadFinished()
{
	qDebug() << this << "Thread has finished (deleting)";
	emit fileTransferDone(pTransfer);
	this->deleteLater();
}

bool WiredTransferSocket::isActive()
{
	return pTransfer.pStatus != 3;
}



