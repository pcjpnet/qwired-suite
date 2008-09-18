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

 
#include <QtNetwork>
#include <QSslSocket>
#include <QSslCipher>
#include "widgetserverinfo.h"
#include "general/classwiredsession.h"

WidgetServerInfo::WidgetServerInfo(QWidget *parent)
 : QWidget(parent)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
}


WidgetServerInfo::~WidgetServerInfo()
{
}

void WidgetServerInfo::loadInfo(ClassWiredSession *theSession)
{
	if( !theSession ) this->close();
	

	fBanner->setPixmap(theSession->pWiredSocket->pServerBanner );
	fServerName->setText( theSession->pWiredSocket->pServerName );
	fUptime->setText( theSession->pWiredSocket->pServerStartTime.toString() );
	fDescription->setText( theSession->pWiredSocket->pServerDescription );	
	fURL->setText( QString("wired://%1/").arg(theSession->pWiredSocket->pSocket->peerAddress().toString() ) );
	fFiles->setText( QString::number(theSession->pWiredSocket->pServerFileCount) );
	fSize->setText( QString("%1 GB").arg( (float)theSession->pWiredSocket->pServerFileSize/1024/1024/1024 )  );
	fVersion->setText( theSession->pWiredSocket->pServerAppVersion );
	fProtocol->setText(theSession->pWiredSocket->pServerProtoVersion);
	
	// Protocol
	QSsl::SslProtocol proto = theSession->pWiredSocket->pSocket->protocol();
	QString protoname = "Unknown";
	switch(proto){
		case QSsl::SslV3: protoname="SSLv3"; break;
		case QSsl::SslV2: protoname="SSLv2"; break;
		case QSsl::TlsV1: protoname="TLSv1"; break;
		default: protoname="Unknown"; break;
		}
	fSslProtocol->setText( protoname );
}


