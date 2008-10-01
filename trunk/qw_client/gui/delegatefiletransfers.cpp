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

 
#include "delegatefiletransfers.h"
#include "wired/classwiredtransfer.h"
#include "wired/classwiredfile.h"

#include <QtGui>

DelegateFileTransfers::DelegateFileTransfers(QObject *parent)
 : QAbstractItemDelegate(parent)
{ }


DelegateFileTransfers::~DelegateFileTransfers()
{ }


QSize DelegateFileTransfers::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
	return QSize( 400, 50 );
}


void DelegateFileTransfers::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if(!index.isValid()) return;
	if (option.state & QStyle::State_Selected)
		painter->fillRect(option.rect, Qt::gray);

	painter->save();
	painter->translate(0, option.rect.y());
	painter->setRenderHint(QPainter::Antialiasing, true);
	
	ClassWiredTransfer tmpT = index.data(Qt::UserRole).value<ClassWiredTransfer>();

	// Progressbar
	QStyleOptionProgressBar progressBarOption;
	progressBarOption.state = QStyle::State_Enabled;
	progressBarOption.direction = QApplication::layoutDirection();
	QRect tmp;
	int tmpX = option.rect.width()-10-(option.rect.width()*0.4);
	tmp.setRight(option.rect.width()-10);
	tmp.setBottom(26);
	tmp.setLeft(tmpX);
	tmp.setTop(8);
	progressBarOption.rect = tmp;
	progressBarOption.fontMetrics = QApplication::fontMetrics();
	progressBarOption.minimum = 0;
	progressBarOption.maximum = 100;
	progressBarOption.textAlignment = Qt::AlignCenter;
	progressBarOption.textVisible = true;
	
	
	// File Name
	QFont font;
	font.setBold(true);
	font.setPixelSize(11);
	QFontMetrics fontmetrics(font);
	painter->setFont(font);
	if(tmpT.pTransferType==WiredTransfer::TypeFolderDownload)
			painter->drawText( 10+32+8, 10+fontmetrics.ascent(), tmpT.pRemoteFolder.section("/",-1,-1) );
	else	painter->drawText( 10+32+8, 10+fontmetrics.ascent(), tmpT.fileName());
	
	// Set up the progressbar
	int progress=0;
	if(tmpT.pTransferType==WiredTransfer::TypeFolderDownload)
			progress = (float(tmpT.pFolderDone+tmpT.pDoneSize)/float(tmpT.pFolderSize))*100;
	else	progress = (float(tmpT.pDoneSize)/float(tmpT.pTotalSize))*100;
	
	progressBarOption.progress = progress<0 ? 0 : progress;

	if(tmpT.pStatus == WiredTransfer::StatusActive) {
		QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
		painter->drawText(tmpX, 40, tr("%1/sec").arg(ClassWiredFile::humanReadableSize(tmpT.pCurrentSpeed)));
		
	} else if(tmpT.pStatus==WiredTransfer::StatusQueuedLocal) {
		painter->drawText(tmpX, 40, tr("Queued locally"));
		
	} else if(tmpT.pStatus==WiredTransfer::StatusDone) {
		if(tmpT.pTransferType==WiredTransfer::TypeFolderDownload)
				painter->drawText(tmpX, 40, tr("Completed - %1 items - %2").arg(tmpT.pFilesCount).arg(ClassWiredFile::humanReadableSize(tmpT.pFolderSize)));
		else	painter->drawText(tmpX, 40, tr("Completed - %1").arg(ClassWiredFile::humanReadableSize(tmpT.pTotalSize)));
		
	} else if(tmpT.pStatus==WiredTransfer::StatusWaitingForStat) {
		if(tmpT.pTransferType==WiredTransfer::TypeFolderDownload)
				painter->drawText(tmpX, 40, tr("Indexing files and directories..."));
		  else	painter->drawText(tmpX, 40, tr("Requesting transfer slot..."));
		
	} else if(tmpT.pStatus==WiredTransfer::StatusQueued) {
		painter->drawText(tmpX, 40, tr("Queued (at position %1)").arg(tmpT.pQueuePosition));
	}
	
	// Icon
	painter->save();
	painter->translate(10, 9);
	if(tmpT.pStatus == WiredTransfer::StatusWaitingForStat) {
		painter->drawPixmap(0, 0, QPixmap(":/icons/32x32/network-server.png"));
	} else if(tmpT.pStatus == WiredTransfer::StatusDone) {
		painter->drawPixmap(0, 0, QPixmap(":/icons/32x32/face-smile.png"));
	} else if(tmpT.pStatus == WiredTransfer::StatusQueued) {
		painter->drawPixmap(0, 0, QPixmap(":/icons/32x32/start-here.png"));
	} else if(tmpT.pStatus == WiredTransfer::StatusQueuedLocal) {
		painter->drawPixmap(0, 0, QPixmap(":/icons/32x32/start-here.png"));
	} else if(tmpT.pStatus == WiredTransfer::StatusActive) {
		if(tmpT.pTransferType == WiredTransfer::TypeDownload) {
			painter->drawPixmap(0, 0, QPixmap(":/icons/32x32/go-down.png"));
		} else if(tmpT.pTransferType == WiredTransfer::TypeFolderDownload) {
			painter->drawPixmap(-3, -3, QPixmap(":/icons/32x32/go-down.png"));
			painter->drawPixmap(3, 3, QPixmap(":/icons/32x32/go-down.png"));
		} else if(tmpT.pTransferType == WiredTransfer::TypeUpload) {
			painter->drawPixmap(0, 0, QPixmap(":/icons/32x32/go-up.png"));
		}
	}
	painter->restore();

	// Remaining
	font.setBold(false);
	painter->setFont(font);

	if(tmpT.pStatus==WiredTransfer::StatusActive) {
		if(tmpT.pTransferType==WiredTransfer::TypeFolderDownload) {
			painter->drawText( 10+32+8, 10+fontmetrics.height()+fontmetrics.ascent(),
				tr("%1 (%2 items) of %3 (%4 items) completed")
				.arg(ClassWiredFile::humanReadableSize(tmpT.pFolderDone))
				.arg(tmpT.pFilesDone)
				.arg(ClassWiredFile::humanReadableSize(tmpT.pFolderSize))
				.arg(tmpT.pFilesCount));
		} else {
			painter->drawText( 10+32+8, 10+fontmetrics.height()+fontmetrics.ascent(),
				tr("%1 of %2 completed (%3%) %4")
				.arg(ClassWiredFile::humanReadableSize(tmpT.pDoneSize))
				.arg(ClassWiredFile::humanReadableSize(tmpT.pTotalSize))
				.arg(progress) );
		}
// 			moreInfo += tr(" [%2 files remaining]").arg(tmpT.pFilesCount-tmpT.pFilesDone);
		
		
	}
	
	// Bottom Line
	painter->setPen(QColor(190,190,190));
	painter->drawLine(0, option.rect.height()-1, option.rect.width(), option.rect.height()-1);
	//painter->fillRect( QRect(0,0,50,50), Qt::cyan );

	painter->restore();
}

