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

 
#include "widgettransfers.h"
#include "delegatefiletransfers.h"

WidgetTransfers::WidgetTransfers(QWidget *parent)
 : QWidget(parent), Ui::WidgetTransfers()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
	
	DelegateFileTransfers *tmpD = new DelegateFileTransfers(this);
	fTransfers->setItemDelegate(tmpD);


	
}


WidgetTransfers::~WidgetTransfers()
{
}




void WidgetTransfers::on_fBtnCancel_clicked(bool ) {
	QModelIndex idx = fTransfers->currentIndex();
	if(idx.isValid()) {
		ClassWiredTransfer tmpT = idx.data(Qt::UserRole).value<ClassWiredTransfer>();
		qDebug() << "Cancelling transfer"<<tmpT.pHash<<tmpT.pRemotePath;
		emit transferCancelled(tmpT);
		fBtnCancel->setEnabled(false);
	}

}

void WidgetTransfers::transferListSelectionChanged(const QItemSelection &, const QItemSelection &) {
	fBtnCancel->setEnabled( fTransfers->selectionModel()->hasSelection() );
}

void WidgetTransfers::init() {
	connect( fTransfers->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			 this, SLOT(transferListSelectionChanged(const QItemSelection &, const QItemSelection &)) );
}
