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

 
#ifndef DELEGATEUSERLIST_H
#define DELEGATEUSERLIST_H

#include <QItemDelegate>

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class DelegateUserlist : public QItemDelegate
{
Q_OBJECT
public:
	DelegateUserlist(QObject *parent = 0);
	~DelegateUserlist();
	
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	QFont pListFont;
	bool pCompactMode;
	bool pAlternateRowBg;

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private slots:
	void reloadPrefs();
};

#endif
