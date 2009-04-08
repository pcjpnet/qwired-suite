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

#include "QwcServerInfoWidget.h"

QwcServerInfoWidget::QwcServerInfoWidget(QWidget * parent)
{
	Q_UNUSED(parent)
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
}

void QwcServerInfoWidget::loadInfo(QwcSocket *socket)
{
	if(!socket) this->close();
        fBanner->setPixmap(QPixmap::fromImage(socket->pServerBanner));
        fServerName->setText( socket->serverInfo.name );
        fUptime->setText( socket->serverInfo.startTime.toString() );
        fDescription->setText( socket->serverInfo.description );
        fURL->setText( QString("wired://%1/").arg(socket->sslSocket()->peerAddress().toString() ) );
        fFiles->setText( QString::number(socket->serverInfo.filesCount) );
        fSize->setText( QString("%1 GB").arg( (float)socket->serverInfo.filesSize/1024/1024/1024 )  );
        fVersion->setText( socket->serverInfo.serverVersion );
        fProtocol->setText(socket->serverInfo.protocolVersion);
	
		// Protocol
        QSsl::SslProtocol proto = socket->sslSocket()->protocol();
	QString protoname = "Unknown";
	switch(proto){
		case QSsl::SslV3: protoname="SSLv3"; break;
		case QSsl::SslV2: protoname="SSLv2"; break;
		case QSsl::TlsV1: protoname="TLSv1"; break;
		default: protoname="Unknown"; break;
	}
	fSslProtocol->setText( protoname );
};

QwcServerInfoWidget::~QwcServerInfoWidget()
{ }

