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


QSize DelegateFileTransfers::sizeHint(const QStyleOptionViewItem& , const QModelIndex& ) const {
	return QSize( 400, 50 );
}

void DelegateFileTransfers::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	
	if( !index.isValid() ) return;
	
	if ( option.state & QStyle::State_Selected ) {
		painter->fillRect ( 0, 0, option.rect.width(), option.rect.height(), Qt::gray );
	}

	painter->save();
	painter->translate ( 0, option.rect.y() );
	painter->setRenderHint ( QPainter::Antialiasing, true );
	
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
	tmp.setTop(10);
	progressBarOption.rect = tmp;
	progressBarOption.fontMetrics = QApplication::fontMetrics();
	progressBarOption.minimum = 0;
	progressBarOption.maximum = 100;
	progressBarOption.textAlignment = Qt::AlignCenter;
	progressBarOption.textVisible = true;
	
	
	// File Name
	QFont font("sans-serif", 11, QFont::Bold);
	font.setPixelSize(11);
	QFontMetrics fontmetrics(font);
	painter->setFont(font);
	painter->drawText( 10+32+8, 10+fontmetrics.ascent(), tmpT.fileName());
	
	// Set up the progressbar
	int progress = (float(tmpT.pDoneSize)/float(tmpT.pTotalSize))*100;
	progressBarOption.progress = progress < 0 ? 0 : progress;

	if(tmpT.pStatus==2) {
		QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
	
		// Draw the speed info
		painter->drawText(tmpX, 40, tr("%1/sec").arg(ClassWiredFile::humanReadableSize(tmpT.pCurrentSpeed)));
	} else if(tmpT.pStatus==0) {
		painter->drawText(tmpX, 40, tr("Requesting transfer slot..."));
	} else if(tmpT.pStatus==1) {
		painter->drawText(tmpX, 40, tr("Queued (at position %1)").arg(tmpT.pQueuePosition));
	}
	
	// Icon
	if(tmpT.pTransferType==0) { // Download
		QPixmap icn(":/icons/32_download.png");
		painter->drawPixmap(10, 9, icn);
	} else {
		QPixmap icn(":/icons/32_upload.png");
		painter->drawPixmap(10, 9, icn);
	}
	
	
	
	// Remaining
	font.setBold(false);
	painter->setFont(font);

	if(tmpT.pStatus==2) {
		painter->drawText( 10+32+8, 10+fontmetrics.height()+fontmetrics.ascent(),
						tr("%1 of %2 completed (%3%)").arg(ClassWiredFile::humanReadableSize(tmpT.pDoneSize))
						.arg(ClassWiredFile::humanReadableSize(tmpT.pTotalSize))
						.arg(progress) );
	}
	
	// Bottom Line
	painter->setPen(QColor(190,190,190));
	painter->drawLine(0, option.rect.height()-1, option.rect.width(), option.rect.height()-1);
	//painter->fillRect( QRect(0,0,50,50), Qt::cyan );

	painter->restore();
}

