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

WiredTransferSocket::WiredTransferSocket(QObject *parent)
 : QThread(parent)
{
	pSendingFile = false;
	pSocket = new QSslSocket(this);
	pSocket->setProtocol(QSsl::TlsV1);
	connect( pSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(on_socket_sslErrors(QList<QSslError>))); 
	connect( pSocket, SIGNAL(encrypted()), this, SLOT(onSocketConnected()) );
	connect( pSocket, SIGNAL(readyRead()), this, SLOT(onSocketReady()) );
	connect( pSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()) );
	connect( pSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendNextFileChunk(qint64)) );
	connect( pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)) );
	qDebug() << "Create Transfer thread:"<<currentThreadId();
}


void WiredTransferSocket::run() {
	qDebug() << "Start Transfer thread:"<<currentThreadId();
	startTransfer();
	exec();
}



// Called by the socket and indicates the an SSL error has occoured.
void WiredTransferSocket::on_socket_sslErrors(const QList<QSslError> & errors) {
	qDebug() << "WiredSocket.on_socket_sslErrors(): "<<errors;
	pSocket->ignoreSslErrors();
}

WiredTransferSocket::~WiredTransferSocket() {
	qDebug() << "Destroyed thread"<<currentThreadId();
	
}

/**
 * Connect to the remote host and initiate a file transfer.
 */
void WiredTransferSocket::startTransfer() {
	if( pTransfer.pTransferType==0 ) { // === Download ===
		QString tmpPath = pTransfer.pLocalPath+QString(".WiredTransfer");
		pFile = new QFile(tmpPath, this);
		if( !pFile->open(QIODevice::Append) ) {
			qDebug() << "WiredTransfer: Unable to open the file:"<<tmpPath<<":"<<pFile->errorString();
			quit();
			return;
		}
		
	} else if(pTransfer.pTransferType==1) { // === Upload ===
		pFile = new QFile(pTransfer.pLocalPath, this);
		if(!pFile->open(QIODevice::ReadOnly) ) {
			qDebug() << "WiredTransfer: Unable to open the file:"<<pTransfer.pLocalPath<<":"<<pFile->errorString();
			quit();
			return;
		}
		pFile->seek(pTransfer.pOffset);
		
	}
	
	if(!pTransfer.pEncryptTransfer)
		pSocket->setCiphers("NULL"); // <-- if we want no encryption ;) I guess morris' mac is just too slow
	
	pSocket->connectToHostEncrypted(pServerHost, pServerPort);
	qDebug() << "WiredTransfer: Connecting to"<<pServerHost<<":"<<pServerPort<<"hash"<<pTransfer.pHash<<"offset"<<pTransfer.pOffset;	
}


// Set the server address and port.
void WiredTransferSocket::setServer(QString theServer, int thePort) {
	pServerHost = theServer;
	pServerPort = thePort+1; // n+1 for file transfers
}


void WiredTransferSocket::onSocketConnected() {
	qDebug() << "WiredTransfer: Connected, sending handshake for"<<pTransfer.pHash;
	QByteArray tmpCmd("TRANSFER ");
	tmpCmd += pTransfer.pHash;
	tmpCmd += 0x04; // EOT
	pSocket->write(tmpCmd);

	// Start traffic timer
	pTimer.start();
	pTimerID = startTimer(1000);

	// Let others know we are on it
	emit fileTransferStarted(pTransfer);
	
	if( pTransfer.pTransferType==1 ) { // Upload
		pSendingFile=true;
		sendNextFileChunk(0);
	}
}


void WiredTransferSocket::onSocketReady() {
	QByteArray tmpData = pSocket->readAll();
	if( pTransfer.pTransferType==0 ) { // Download
		pFile->write(tmpData);
		pTransfer.pDoneSize += tmpData.size();
	}
}


void WiredTransferSocket::onSocketDisconnected() {

	qDebug() << "WiredTransfer: Connection closed:"<<pTransfer.pDoneSize<<" of "<<pTransfer.pTotalSize;
	pFile->flush();
	pFile->close();
	
	if( pTransfer.pDoneSize==pTransfer.pTotalSize ) {
		// File complete
		if(pTransfer.pTransferType==0) {
			// Rename the file on download, if needed.
			
			QString tmpPath = pTransfer.pLocalPath+QString(".WiredTransfer");
			QFile tmpFile(tmpPath);
			if(tmpFile.exists() && tmpFile.size()==pTransfer.pTotalSize) {
				qDebug() << "WiredTransfer: Renaming the file";
				tmpFile.rename(pTransfer.pLocalPath);
			}
		}
		emit fileTransferDone(pTransfer);
		qDebug() << "WiredTransfer: We are done!";
	} else {
		// Not complete, possibly an error
		emit fileTransferError(pTransfer);
		qDebug() << "WiredTransfer: Not completed, Reason:"<<pSocket->errorString();
	}
	
	killTimer(pTimerID);
	killTransfer();
	quit();
	
}


// Stop the file transfer.
void WiredTransferSocket::cancelTransfer() {
	qDebug() << "WiredTransfer: Stopping transfer"<<pTransfer.pHash;
	pSocket->disconnectFromHost();
	killTimer(pTimerID);
	emit fileTransferDone(pTransfer);
	killTransfer();
}


// Timer event, mainly for launching the status event
void WiredTransferSocket::timerEvent(QTimerEvent * event) {
	Q_UNUSED(event)
	// Update current speed
	qlonglong tmpDiff = pTransfer.pDoneSize-pLastDone; // difference between past and now
	int tmpTime = pTimer.restart(); // time difference
	pTransfer.pCurrentSpeed = (tmpDiff/tmpTime)*1000;
	pLastDone = pTransfer.pDoneSize;
	emit fileTransferStatus(pTransfer);
}


// Upload the next chunk to the server.
void WiredTransferSocket::sendNextFileChunk(qint64 theOffset) {
	Q_UNUSED(theOffset)
	const int tmpChunkSize = 1024*1024;
	
	if( pSendingFile && pTransfer.pTransferType==1 ) {
		pTransfer.pDoneSize = pFile->pos();
		if(!pFile->atEnd()) { // Done!
			//qDebug() << "Done writing"<<theOffset<<"bytes, at position"<<pFile->pos();
			QByteArray tmpData = pFile->read(tmpChunkSize);
			pSocket->write(tmpData);
			if(pFile->atEnd()) {
				qDebug() << "We are done. Close.";
				pSocket->disconnectFromHost();
				killTransfer();
			}
		}
	}
}

void WiredTransferSocket::killTransfer() {
	if(pSocket && pSocket->isEncrypted()) {
		qDebug() << "Disconnecting thread"<<currentThreadId();
		pSocket->waitForDisconnected();
	}
	
	qDebug() << "Waiting for thread to finish:"<<currentThreadId();
	quit();
	wait();
	
	qDebug() << "Thread finished, deleting:"<<currentThreadId();
	this->deleteLater();
}

void WiredTransferSocket::onSocketError(QAbstractSocket::SocketError error) {
	qDebug() << "Failed to connect to the transfer port.";
	emit fileTransferSocketError(error);
	killTransfer();
	
}



