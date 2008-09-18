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

 
#include "widgetuserinfo.h"
#include <QProgressBar>

WidgetUserInfo::WidgetUserInfo(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	pUserID = 0;
}


WidgetUserInfo::~WidgetUserInfo()
{
}

void WidgetUserInfo::setUser(ClassWiredUser theUser)
{
	fStatus->setText( theUser.pStatus );
	fNick->setText( theUser.pNick );
	fLogin->setText( theUser.pLogin );
	fID->setText( QString::number(theUser.pUserID) );
	fAddress->setText( theUser.pIP );
	fHost->setText( theUser.pHost );
	fVersion->setText( theUser.pClientVersion );
	fCipher->setText( theUser.pCipherName+QString(" (%1 bits)").arg(theUser.pCipherBits) );
	fLoginTime->setText( theUser.pLoginTime.toString() );
	fIdleTime->setText( theUser.pIdleTime.toString() );
	
	QPixmap icon;
	icon.loadFromData(theUser.pImage);
	if( !icon.isNull() ) {
		if(icon.width()>128 || icon.height()>128)
			icon = icon.scaled(128, 128, Qt::KeepAspectRatio);
		fIcon->setPixmap(icon);
	}
	setWindowTitle(tr("User Information: %1").arg(theUser.pNick));
	pUserID = theUser.pUserID;
	setTransfers(0, theUser);
	setTransfers(1, theUser);
	
}

void WidgetUserInfo::setTransfers(int theType, const ClassWiredUser theUser)
{
	// theType: 0=Downloads, 1=Uploads
	QByteArray tmpData;
	QGroupBox *tmpGroupBox;
	if( theType==0 ) {
		// Downloads
		tmpData = theUser.pDownloads;
		tmpGroupBox = fGroupDownloads;
	} else {
		// Uploads
		tmpData = theUser.pUploads;
		tmpGroupBox = fGroupUploads;
	}
	
	QVBoxLayout *tmpLayDown = new QVBoxLayout(fGroupDownloads);
	if( tmpData.isEmpty() ) {
		tmpGroupBox->setVisible(false);
	} else {
		// List Transfers
		QList<QByteArray> tmpList = tmpData.split(0x1D);
		QListIterator<QByteArray> i(tmpList);
		while( i.hasNext() ) {
			QList<QByteArray> tmpFields = i.next().split(0x1E);
			QString tmpPath = tmpFields.value(0);
			int tmpDone = tmpFields.value(1).toInt();
			int tmpSize = tmpFields.value(2).toInt();
			float tmpSpeed = tmpFields.value(3).toFloat();
			
			QFont font;
			font.setPixelSize(10);
			font.setBold(false);
			
			// Name
			QLabel *tmpLab = new QLabel( tmpPath, tmpGroupBox);
			tmpLab->setFont(font);
			tmpLayDown->addWidget(tmpLab);
			
			// Progress
			QProgressBar *tmpStat = new QProgressBar(tmpGroupBox);
			tmpStat->setFixedHeight(10);
			tmpStat->setTextVisible(false);
			tmpStat->setMaximum(tmpSize/1024/1024);
			tmpStat->setValue(tmpDone/1024/1024);
			tmpLayDown->addWidget(tmpStat);
			
			// More Info
			QString tmpInfo = tr("%1 MB of %2 MB at %3 KiB/sec")
					.arg( (float)tmpDone/1024/1024 )
					.arg( (float)tmpSize/1024/1024 )
					.arg( tmpSpeed/1024 );
			
			QLabel *tmpMore = new QLabel( tmpInfo, tmpGroupBox);
			tmpMore->setFont(font);
			tmpLayDown->addWidget(tmpMore);
			
			if( i.hasNext() )
				tmpLayDown->addSpacing(8);
			
			qDebug() <<"Path:"<<tmpPath <<"Done:"<< tmpDone <<"Size:"<<tmpSize <<"Speed:"<<tmpSpeed;
		}
	}
	tmpLayDown->setSpacing(0);
	tmpLayDown->setMargin(6);
	tmpGroupBox->setLayout(tmpLayDown);
}


