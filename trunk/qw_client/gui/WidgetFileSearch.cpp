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

 
#include "WidgetFileSearch.h"
#include <QtGui>

WidgetFileSearch::WidgetFileSearch(QWidget *parent)
 : QWidget(parent)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
}


WidgetFileSearch::~WidgetFileSearch()
{
}

void WidgetFileSearch::updateResults(QList< ClassWiredFile > theResults) {
	QListIterator<ClassWiredFile> i(theResults);
	while(i.hasNext()) {
		ClassWiredFile tmpRes = i.next();
		QListWidgetItem *item = new QListWidgetItem(fResults);
		item->setText(tmpRes.fileName());
		item->setIcon( QIcon(":/icons/icn_file_18.png") );
		item->setData(Qt::UserRole, tmpRes.path);
		item->setData(Qt::UserRole+1, tmpRes.type);
	}

}

void WidgetFileSearch::on_fSearch_returnPressed() {
	QString tmpSearch = fSearch->text();
	if(!tmpSearch.isEmpty()) {
		fResults->clear();
		emit search(tmpSearch);
	}
}

void WidgetFileSearch::on_fBtnDownload_clicked(bool)
{
	QList<QListWidgetItem*> items = fResults->selectedItems();
	QListIterator<QListWidgetItem*> i(items);
	while(i.hasNext()) {
		QListWidgetItem *item = i.next();
		if(item->data(Qt::UserRole+1).toInt()==0) {
			// Only files! :)
			QString tmpPath = item->data(Qt::UserRole).toString();

			QSettings settings;
			QString tmpName = tmpPath.section("/",-1,-1);

			QDir tmpDownloadFolder( settings.value("files/download_dir", QDir::homePath()).toString() );
			QFile tmpDir( tmpDownloadFolder.absoluteFilePath(tmpName) );
			if(tmpDir.exists()) {
				QMessageBox messageBox(this);
				messageBox.setWindowTitle(tr("File Exists"));
				messageBox.setIcon(QMessageBox::Warning);
				messageBox.setText(tr("The file you are trying to download already exists in your download directory. Overwrite it?") );
				QAbstractButton *rejectButton = messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
				QAbstractButton *deleteButton = messageBox.addButton(tr("Overwrite"), QMessageBox::AcceptRole);
				messageBox.exec();
				if (messageBox.clickedButton()!=deleteButton)
					return;
			}
			emit downloadFile( item->data(Qt::UserRole).toString() );

		}

			
			
	}
}

void WidgetFileSearch::on_fResults_currentRowChanged(int row) {
	fBtnDownload->setEnabled( row!=-1);
	fBtnReveal->setEnabled( row!=-1);
}

void WidgetFileSearch::on_fBtnReveal_clicked(bool ) {
	QList<QListWidgetItem*> items = fResults->selectedItems();
	if(items.count()) {
		QListWidgetItem *item = items[0];
		emit revealFile( item->data(Qt::UserRole).toString() );
	}
}



