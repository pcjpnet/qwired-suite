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

#include "../general/wiredcommon.h"
#include "delegateuserlist.h"
#include <QtCore>
#include <QtGui>

DelegateUserlist::DelegateUserlist ( QObject *parent )
		: QItemDelegate ( parent )
{
	// Notification manager
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );
	reloadPreferences();
}


DelegateUserlist::~DelegateUserlist()
{ }

void DelegateUserlist::reloadPreferences() {
	QSettings s;
	pListFont.fromString(s.value("interface/userlist/font", QApplication::font().toString()).toString());
	pCompactMode = s.value("interface/userlist/compact",false).toBool();
	pAlternateRowBg = s.value("interface/userlist/alternate_bg",false).toBool();
}

QSize DelegateUserlist::sizeHint ( const QStyleOptionViewItem &, const QModelIndex &index ) const {
	if(!index.isValid()) return QSize();
	if(pCompactMode) {
		return QSize(160,24);
	} else {
		return QSize(160,34);
	}
}

void DelegateUserlist::paint ( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	//ClassWiredUser usr = index.data(Qt::UserRole).value<ClassWiredUser>();
	QString status = index.data ( Qt::UserRole+4 ).toString();
	QString nick = index.data ( Qt::DisplayRole ).toString();
	bool admin = index.data ( Qt::UserRole+1 ).toBool();
	bool idle = index.data ( Qt::UserRole+2 ).toBool();

	painter->save();
	painter->translate ( 0, option.rect.y() );
	painter->setRenderHint ( QPainter::Antialiasing, true );

	// Set nick color
	QColor nickColor=Qt::black;
	QColor statusColor=Qt::black;
	if(admin) nickColor=Qt::red;
	//if(idle)  nickColor=nickColor.lighter(200);
	
	//if(idle) statusColor=Qt::darkGray;

	
	// Alternate Bg
	if(pAlternateRowBg && (index.row() % 2)==1) {
		painter->fillRect(0,0,option.rect.width(), option.rect.height(), QColor(238,238,255) );
	}

	// Background
	if ( option.state & QStyle::State_Selected )
		painter->fillRect ( 0, 0, option.rect.width(), option.rect.height(), Qt::gray );

	if(idle) painter->setOpacity(0.5);

	// User Icon/Picture
	painter->save();
	
	QImage icon = index.data(Qt::DecorationRole ).value<QImage>();
	
	if (icon.isNull()) // Fallback icon
		icon = QImage(":/icons/icon_missing.png");
	
	if(pCompactMode) // Resize
		icon = icon.scaled(22,22,Qt::KeepAspectRatio);
	
	painter->drawImage(1,1,icon);
	painter->restore();
	
	if(pCompactMode || status.isEmpty()) {
		// Draw nick only
		QFont f = pListFont;
		painter->save();
		QFontMetrics fm(f);
		painter->setFont(f);
		painter->setPen(nickColor);
		if(pCompactMode) {
			painter->drawText(28, (option.rect.height()/2)+fm.descent(), nick);
		} else {
			painter->drawText(40, (option.rect.height()/2)+fm.descent(), nick);
		}
		painter->restore();
		
	} else {
		// Draw nick + status
		QFont f = pListFont;

		// Status
		painter->save();
		f.setPointSizeF( f.pointSize()*0.85 );
		QFontMetrics fm ( f );
		painter->setFont ( f );
		painter->setPen ( statusColor );
		painter->drawText ( 40, ( option.rect.height() /2 ) +fm.ascent() +1, status );
		painter->restore();

		// Nick
		painter->save();
		f = pListFont;
		painter->setFont ( f );
		painter->setPen ( nickColor );
		painter->drawText ( 40, ( option.rect.height() /2 )-1, nick );
		painter->restore();

	}
	painter->restore();
}



