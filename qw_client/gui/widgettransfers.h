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

 
#ifndef WIDGETTRANSFERS_H
#define WIDGETTRANSFERS_H

#include <QWidget>
#include "ui_WidgetTransfers.h"
#include "../wired/classwiredtransfer.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WidgetTransfers
	: public QWidget, public Ui::WidgetTransfers
{
	
Q_OBJECT
		
public:
    WidgetTransfers(QWidget *parent = 0);
    ~WidgetTransfers();
	void init();

signals:
	void transferCancelled(ClassWiredTransfer);
	
private slots:
	void on_fBtnCancel_clicked(bool);
	void transferListSelectionChanged(const QItemSelection &, const QItemSelection &);

};

#endif
